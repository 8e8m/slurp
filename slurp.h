#ifndef SLURP_H_
#define SLURP_H_

/* slurp.h -- Public Domain */

#include <stddef.h>

/* error value <- NONNULL filename, NONNULL return buffer, NULLABLE return length. */
int slurp(char const * const, char * * const, size_t * const);

#endif /* SLURP_H_ */
