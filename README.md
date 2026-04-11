## STB-style slurp.h

Include directly in your project.

Written C89-only, POSIX expected: `-D_POSIX_C_SOURCE=200809L` or later.

`#define SLURP_IMPLEMENTATION` in one compilation unit.

Provides `slurp(char * filename / int file descriptor / FILE * io file pointer, int * optional length return)` for C11.

Below describes with exact functionality `slurpfile`, `slurpfd`, `slurpfp`, respectively.

Supports reading streams and regular files.

Code is correct. (Note that this is only provable by usage, so be warned.)

```h
/* slurp generic for C11+:
   return buffer <- NONNULL filename / POSITIVE fd / VALID FILE *, NULLABLE return length. */
#if defined(__STDC_VERSION__) && __STDC_VERSION__ >= 201112L
#define slurp(a,b) _Generic(a, char *: slurpfile, char const *: slurpfile, \
                               int: slurpfd, \
                               FILE *: slurpfp)(a,b)
#else
define slurp(a,b) slurpfile((a),(b))
#endif

/* return buffer <- NONNULL filename / VALID fd / VALID FILE * respectively, NULLABLE return length. */
extern char * slurpfile(char const * const, size_t * const);
extern char * slurpfd(const int, size_t * const);
extern char * slurpfp(FILE * const, size_t * const);

/* Please define SLURP_IMPLEMENTATION in one compilation unit */
```

## test/cat
POSIXLY correct cat implementation using slurp, purely as an example.
Note the code within may be hazardous to ones health, and may cause tumors within the brain.
catfp/cat11 just exists to test things.

## Copyright
Public Domain.



