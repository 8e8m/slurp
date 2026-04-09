## STB-style slurp.h

Include directly in your project.

Written C89-only, POSIX expected: `-D_POSIX_C_SOURCE=200809L` or later.

`#define SLURP_IMPLEMENTATION` in one compilation unit.

Supports reading streams and regular files.

Code is correct. (Note that this is only provable by usage, so be warned.)

```h
/* slurp.h -- Public Domain */

#if !defined(_POSIX_C_SOURCE) || _POSIX_C_SOURCE < 200809L
#error Please provide -D_POSIX_C_SOURCE=200809L or greater to relavant compilation units.
#endif

#include <stddef.h>

/* return buffer <- NONNULL filename, NULLABLE return length. */
char * slurp(char const * const, size_t * const);
/* return buffer <- POSITIVE VALID file descriptor, NULLABLE return length. */
char * slurpfd(const int, size_t * const);
/* return buffer <- POSITIVE VALID FILE pointer, NULLABLE return length. */
char * slurpfp(FILE *, size_t * const);

/* Please define SLURP_IMPLEMENTATION in one compilation unit */
```

## cat
POSIXLY correct cat implementation using slurp, purely as an example.
Note the code within may be hazardous to ones health, and may cause tumors within the brain.
catfp just exists to test slurpfp, additionally.

## Copyright
Public Domain.



