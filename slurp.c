/* slurp.c -- Public Domain */
/* should work under non-gnu POSIX-"enabled" C89 */

#if !defined(_POSIX_C_SOURCE) || _POSIX_C_SOURCE < 200809L
#undef _POSIX_C_SOURCE
#define _POSIX_C_SOURCE 200809L
#endif

#include <fcntl.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
#include <limits.h>
#include <stdint.h>

#ifdef _LARGEFILE64_SOURCE
#define offset_t off64_t
#define seek lseek64
#else
#define offset_t off_t
#define seek lseek
#endif

#ifndef O_BINARY
#define O_BINARY 0
#define LOCALLY_O_BINARY
#endif

#if __STDC_VERSION__ >= 201112L
#define _STRINGIFY(...) #__VA_ARGS__
#define STRINGIFY(...) _STRINGIFY(__VA_ARGS__)
/* we care to upgrade offset_t into size_t, hence: */
_Static_assert(sizeof(size_t) >= sizeof(offset_t), "size_t must be at least as large as " STRINGIFY(offset_t));
_Static_assert(sizeof(size_t) * CHAR_BIT >= 16, "size_t bitsize must be greater than 16");
#undef _STRINGIFY
#undef STRINGIFY
#endif

char * slurpfd(const int f, size_t * const final_buffer_length)
{ struct stat st;
  char * block = NULL;
  ssize_t count;
  size_t total = 0;

  if (f < 0)
  { return block;
  }

  if (fstat(f, &st) < 0)
  { goto end;
  }

  if (S_ISREG(st.st_mode))
  { offset_t length = seek(f, 0, SEEK_END);

    if (length < 0
    ||  seek(f, 0, SEEK_SET) < 0)
    { goto end;
    }

    block = malloc((size_t) length + 1);

    if (!block)
    { goto end;
    }

    while (total < (size_t) length)
    { count = read(f, block + total, (size_t) length - total);

      if (count == 0)
      { break;
      }

      if (count < 0)
      { goto end;
      }

      total += (size_t) count;
    }

    block[length] = '\0';

    if (final_buffer_length)
    { *final_buffer_length = length;
    }
  }
  else
  { char * temporary_buffer;
    size_t limit = (size_t) 1 << 16;
    block = malloc(limit);

    if (!block)
    { goto end;
    }

    for (;;)
    { if (total == limit)
      { if (limit > SIZE_MAX >> 1)
        { goto end;
        }

        limit <<= 1;

        temporary_buffer = realloc(block, limit);

        if (!temporary_buffer)
        { goto end;
        }

        block = temporary_buffer;
      }
      count = read(f, block + total, limit - total);

      if (count == 0)
      { break;
      }

      if (count < 0)
      { goto end;
      }

      total += count;
    }

    temporary_buffer = realloc(block, total + 1);
    if (!temporary_buffer)
    { goto end;
    }

    block = temporary_buffer;
    block[total] = '\0';

    if (final_buffer_length)
    { *final_buffer_length = total;
    }
  }

  return block;

end:
  free(block);
  return NULL;
}

char * slurp(char const * const path, size_t * const final_buffer_length) {
  char * r;
  if (!path)
  { return NULL;
  }
  const int f = open(path, O_RDONLY | O_BINARY);
  if (f < 0) { return NULL; }
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
