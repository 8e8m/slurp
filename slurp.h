#ifndef SLURP_H_
#define SLURP_H_

/* slurp.h -- Public Domain */

#if !defined(_POSIX_C_SOURCE) || _POSIX_C_SOURCE < 200809L
#error Please provide -D_POSIX_C_SOURCE=200809L or greater to relavant compilation units.
#endif

#include <stddef.h>

/* return buffer <- NONNULL filename, NULLABLE return length. */
char * slurp(char const * const, size_t * const);
/* return buffer <- POSITIVE VALID file descriptor, NULLABLE return length. */
char * slurpfd(const int, size_t * const);

#endif /* SLURP_H_ */
