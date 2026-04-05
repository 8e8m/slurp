/* slurp.c -- Public Domain */
/* should work under non-gnu POSIX-"enabled" C89 */

#define _POSIX_C_SOURCE 200809L

#include <fcntl.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>

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
_Static_assert(sizeof(size_t) >= sizeof(offset_t), "size_t must be at least as large as " STRINGIFY(offset_t));
#undef _STRINGIFY
#undef STRINGIFY
#endif

int slurp(char const * const path, char ** const final_buffer, size_t * const final_buffer_length)
{ int f;
  int rv = 0;
  struct stat st;
  char * block;
  ssize_t count;
  size_t total = 0;

  if (!path
  ||  !final_buffer)
  { return -1;
  }

  f = open(path, O_RDONLY | O_BINARY);
  if (f < 0)
  { return 1;
  }

  if (fstat(f, &st) < 0)
  { rv = 1;
    goto end;
  }

  if (S_ISREG(st.st_mode))
  { offset_t length = seek(f, 0, SEEK_END);
    if (length < 0
    ||  seek(f, 0, SEEK_SET) < 0)
    { rv = 1;
      goto end;
    }

    block = malloc(length + 1);
    if (!block)
    { rv = 1;
      goto end;
    }

    while (total < (size_t) length)
    { count = read(f, block + total, (size_t) length - total);
      if (count == 0)
      { break;
      }
      if (count < 0)
      { free(block);
        rv = 1;
        goto end;
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
    { rv = 1;
      goto end;
    }

    for (;;)
    { if (total == limit)
      { limit <<= 1;

        temporary_buffer = realloc(block, limit);
        if (!temporary_buffer)
        { free(block);
          rv = 1;
          goto end;
        }
        block = temporary_buffer;
      }
      count = read(f, block + total, limit - total);
      if (count == 0)
      { break;
      }
      if (count < 0)
      { free(block);
        rv = 1;
        goto end;
      }
      total += count;
    }

    temporary_buffer = realloc(block, total + 1);
    if (!temporary_buffer)
    { free(block);
      rv = 1;
      goto end;
    }
    block = temporary_buffer;
    block[total] = '\0';

    if (final_buffer_length)
    { *final_buffer_length = total;
    }
  }
  *final_buffer = block;
end:
  close(f);
  return rv;
}

#ifdef LOCALLY_O_BINARY
#undef LOCALLY_O_BINARY
#undef O_BINARY
#endif

#undef offset_t
#undef seek
