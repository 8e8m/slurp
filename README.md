## slurp.[ch]
Provides `./slurp.c` & `./slurp.h` see `./slurp.h` for usage.
Include directly in your project.

Written C89-only, POSIX expected.

Supports reading streams and regular files.

```c
/* error value <- NONNULL filename, NONNULL return buffer, NULLABLE return length. */
int slurp(char const * const, char * * const, size_t * const);
```

Returns 1 on error, check errno.
Returns -1 on parameter failure (bad function call.)

Does not modify errno directly.

## Copyright
Public Domain.
