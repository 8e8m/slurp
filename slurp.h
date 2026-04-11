#ifndef SLURP_H_
#define SLURP_H_

/* slurp.h -- Public Domain */

#if !defined(_POSIX_C_SOURCE) || _POSIX_C_SOURCE < 200809L
#error Please provide -D_POSIX_C_SOURCE=200809L or greater to relevant compilation units.
#endif

#include <stddef.h>
#include <stdio.h>

/* return buffer <- NONNULL filename / POSITIVE fd / VALID FILE *, NULLABLE return length. */
extern char * slurpfile(char const * const, size_t * const);
extern char * slurpfd(const int, size_t * const);
extern char * slurpfp(FILE * const, size_t * const);

#if defined(__STDC_VERSION__) && __STDC_VERSION__ >= 201112L
#define slurp(a,b) _Generic(a, char *: slurpfile, char const *: slurpfile, \
                               int: slurpfd, \
                               FILE *: slurpfp)(a,b)
#else
#define slurp(a,b) slurpfile((a),(b))
#endif

#ifdef SLURP_IMPLEMENTATION
/* should work under non-gnu POSIX-"enabled" C89 */
#include <fcntl.h>
#include <limits.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>

#ifdef _LARGEFILE64_SOURCE
#define offset_t off64_t
#define seek lseek64
#define fpseek fseeko64
#define fptell ftello64
#else
#define offset_t off_t
#define seek lseek
#define fpseek fseeko
#define fptell ftello
#endif

#ifndef O_BINARY
#define O_BINARY 0
#define LOCALLY_O_BINARY
#endif

#define OFFSET_MAX ((((offset_t) 1 << (sizeof(offset_t) * CHAR_BIT - 2)) - 1) * 2 + 1)

#if defined(__STDC_VERSION__) && __STDC_VERSION__ >= 201112L
#define STRINGIFY_(...) #__VA_ARGS__
#define STRINGIFY(...) STRINGIFY_(__VA_ARGS__)
/* we care to upgrade offset_t into size_t, hence: */
_Static_assert(sizeof(size_t) >= sizeof(offset_t), "size_t must be at least as large as " STRINGIFY(offset_t));
_Static_assert(sizeof(size_t) * CHAR_BIT >= 16, "size_t bitsize must be greater than 16");
#undef STRINGIFY_
#undef STRINGIFY
#endif

char * slurpfp(FILE * const fp, size_t * const final_buffer_length)
{ char * buffer = NULL, * tmp;
  size_t count;
  size_t total = 0;
  offset_t prior;

  if (fp == NULL)
  { return buffer;
  }

  if ((prior = fptell(fp)) >= 0)
  { offset_t length;
    if (fpseek(fp, 0, SEEK_END) < 0
    ||  (length = fptell(fp)) < 0
    ||  length == OFFSET_MAX
    ||  !(buffer = malloc((size_t) length + 1)))
    { fpseek(fp, prior, SEEK_SET);
      return buffer;
    }
  
    fpseek(fp, 0, SEEK_SET);
    
    while (total < (size_t) length)
    { count = fread(buffer + total, 1, (size_t) length - total, fp);
      total += (size_t) count;

      if (ferror(fp))
      { goto end;
      }

      if (feof(fp))
      { break;
      }
    }
  
    fpseek(fp, prior, SEEK_SET);

    if (total < (size_t) length)
    { tmp = realloc(buffer, total + 1);
      if (!tmp)
      { goto end;
      }
      buffer = tmp;
    }
  }
  else
  { size_t limit = (size_t) 1 << 16;
    buffer = malloc(limit);
  
    if (!buffer)
    { return buffer;
    }

    if (ferror(fp))
    { goto end;
    }

    while (!feof(fp))
    { if (total == limit)
      { if (limit > SIZE_MAX >> 1)
        { goto end;
        }

        limit <<= 1;

        tmp = realloc(buffer, limit);

        if (!tmp)
        { goto end;
        }

        buffer = tmp;
      }

      count = fread(buffer + total, 1, limit - total, fp);

      if (ferror(fp))
      { goto end;
      }

      if (count == 0)
      { break;
      }

      total += (size_t) count;
    }    

    tmp = realloc(buffer, total + 1);
    if (!tmp)
    { goto end;
    }

    buffer = tmp;      
  }

  buffer[total] = '\0';

  if (final_buffer_length)
  { *final_buffer_length = (size_t) total;
  }

  return buffer;
end:
  free(buffer);
  return NULL;
}

char * slurpfd(const int f, size_t * const final_buffer_length)
{ struct stat st;
  char * buffer = NULL, * tmp;
  ssize_t count;
  size_t total = 0;

  if (f < 0)
  { return buffer;
  }

  if (fstat(f, &st) < 0)
  { goto end;
  }

  if (S_ISREG(st.st_mode))
  { offset_t prior = seek(f, 0, SEEK_CUR);
    offset_t length = seek(f, 0, SEEK_END);

    if (length < 0
    ||  length == OFFSET_MAX)
    { seek(f, prior, SEEK_SET);
      goto end;
    }

    if (seek(f, 0, SEEK_SET) < 0)
    { goto end;
    }

    buffer = malloc((size_t) length + 1);

    if (!buffer)
    { seek(f, prior, SEEK_SET);
      goto end;
    }

    while (total < (size_t) length)
    { count = read(f, buffer + total, (size_t) length - total);
      if (count == 0)
      { break;
      }

      if (count < 0)
      { goto end;
      }

      total += (size_t) count;
    }

    seek(f, prior, SEEK_SET);

    if (total < (size_t) length)
    { tmp = realloc(buffer, total + 1);
      if (!tmp)
      { goto end;
      }
      buffer = tmp;
    }
  }
  else
  { size_t limit = (size_t) 1 << 16;
    buffer = malloc(limit);

    if (!buffer)
    { goto end;
    }

    for (;;)
    { if (total == limit)
      { if (limit > SIZE_MAX >> 1)
        { goto end;
        }

        limit <<= 1;

        tmp = realloc(buffer, limit);

        if (!tmp)
        { goto end;
        }

        buffer = tmp;
      }
      count = read(f, buffer + total, limit - total);

      if (count == 0)
      { break;
      }

      if (count < 0)
      { goto end;
      }

      total += count;
    }

    tmp = realloc(buffer, total + 1);
    if (!tmp)
    { goto end;
    }

    buffer = tmp;
  }

  buffer[total] = '\0';

  if (final_buffer_length)
  { *final_buffer_length = total;
  }

  return buffer;

end:
  free(buffer);
  return NULL;
}

char * slurpfile(char const * const path, size_t * const final_buffer_length)
{ char * r;
  int f;
  if (!path)
  { return NULL;
  }
  f = open(path, O_RDONLY | O_BINARY);
  if (f < 0)
  { return NULL;
  }
  r = slurpfd(f, final_buffer_length);
  close(f);
  return r;
}

#ifdef LOCALLY_O_BINARY
#undef LOCALLY_O_BINARY
#undef O_BINARY
#endif

#undef offset_t
#undef seek
#undef fpseek
#undef fptell
#undef OFFSET_MAX
#endif /* SLURP_IMPLEMENTATION */

#endif /* SLURP_H_ */
