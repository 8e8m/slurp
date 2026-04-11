/* catfp, reduced for testing purposes @BAKE cc -I.. -o $* $@ -std=c11 -D_LARGEFILE64_SOURCE -D_POSIX_C_SOURCE=200809L $+ @STOP */
#define SLURP_IMPLEMENTATION
#include "slurp.h"
#include <string.h>

int main(int ac, char ** av)
{ FILE * fp = stdin;
  if (!(ac - 1))
  { goto print;
  }

  while (++av, --ac)
  { size_t length;
    char * buffer;
    fp = !strcmp(*av, "-") ? stdin : fopen(*av, "rb");
print:
    if (!fp) { return 1; }
    buffer = slurp(fp, &length);
    if (fp != stdin)
    { fclose(fp);
    }
    if (!buffer)
    { return 1; }
    fwrite(buffer, 1, length, stdout);
    free(buffer);
  }
  return 0;
}
