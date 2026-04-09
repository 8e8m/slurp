/* IEEE Std 1003.1 2024 Compliant */
/* cat @BAKE cc -o $* $@ -std=c89 -D_LARGEFILE64_SOURCE -D_POSIX_C_SOURCE=200809L $+ @STOP */
#define SLURP_IMPLEMENTATION
#include "slurp.h"
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define HELP                                                                                                 \
  "cat [-uh?] [--] [- as /dev/stdin] [FILES...]\n"                                                           \
  "POSIXLY correct.\n"
#define NO_OPT "cat: No such option: "
#define NO_FILE "cat: Could not open: "
#define BAD_STDIN "cat: Could not read stdin: "

int print(int fd, char * buffer, size_t length)
{ size_t total = 0;
  ssize_t count;
  int errno_local;
  while ((count = write(fd, buffer + total, length - total)) > 0)
  { if (count + total < total)
    { return 1;
    }
    total += count;
    if (total >= length)
      break;
  }
  errno_local = errno;
  if (count < 0)
  { if (fd != STDERR_FILENO)
    { char * error = strerror(errno_local);
      if (print(STDERR_FILENO, "cat: ", strlen("cat: ")))
      { return 1;
      }
      print(STDERR_FILENO, error, strlen(error));
    }
    /* ahem... */
    return 1;
  }
  return 0;
}
int main(int ac, char * * av)
{ int skip = 0, stdin_read = 0, r = 0;
  char * file;
  int errno_local = 0;

  size_t buffer_length;
  char * buffer;

  if (!(ac - 1))
  { stdin_read = 1;
    goto print;
  }

  while (++av, --ac)
  { file = *av;
    if (av[0][0] == '-')
    { if (!stdin_read && av[0][1] == '\0')
      { stdin_read = 1;
        goto print;
      }
      else if (!skip)
      again:
      { switch (av[0][1])
        { case 'u':
          break; /* the only option, which does nothing, hence we don't need robust options support */
        case '-':
        { if (av[0][2] == '\0')
          { skip = ~0;
            break;
          }
          av[0]++; /* hack exclusively for --help */
          goto again;
        }
        case 'h':
        case '?':
          return print(STDOUT_FILENO, HELP, strlen(HELP));
        default:
        { if (print(STDERR_FILENO, NO_OPT, strlen(NO_OPT))
          ||  print(STDERR_FILENO, av[0] + 1, strlen(av[0] + 1))
          ||  print(STDERR_FILENO, "\n", 1));
          return 1;
        }
        }
        continue;
      }
    }

  print:
    if (stdin_read)
    { buffer = slurpfd(STDIN_FILENO, &buffer_length);
    }
    else
    { buffer = slurp(file, &buffer_length);
    }
    errno_local = errno;

    if (!buffer)
    { r = 1;
      char * error = strerror(errno_local);
      if (stdin_read)
      { if (print(STDERR_FILENO, BAD_STDIN, strlen(BAD_STDIN))
        ||  print(STDERR_FILENO, error, strlen(error))
        ||  print(STDERR_FILENO, "\n", 1));
        return 1;
      }
      if (print(STDERR_FILENO, NO_FILE, strlen(NO_FILE))
      ||  print(STDERR_FILENO, file, strlen(file))
      ||  print(STDERR_FILENO, ": ", strlen(": "))
      ||  print(STDERR_FILENO, error, strlen(error))
      ||  print(STDERR_FILENO, "\n", 1))
      { return 1;
      }
      stdin_read = 0;
      continue;
    }

    if (print(STDOUT_FILENO, buffer, buffer_length))
    { free(buffer);
      return 1;
    }
    free(buffer);
    stdin_read = 0;
  }
  return r;
}
