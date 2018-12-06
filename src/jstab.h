#ifndef __JSTAB_H_
#define __JSTAB_H_

#include "jsmn-extra.h"

typedef struct {
	char **keys;	/* keys row stored separately for convenience */
	int nc;			/* number of columns */
	char ***rows;	/* pointer to pointer to first cell in the row */
	int nr;			/* number of rows */
	char *blob;		/* actual json data as one big array,
					   sliced using '\0' bytes */
	size_t sz;		/* size of the blob */
} jstab_t;

/* convert ts to tab, destroys ts */
int jstab_morph(jsmnts_t *ts, jstab_t *tab, int ind);

/* clar tab */
void jstab_clear(jstab_t *tab);

/* print tab */
void jstab_fprint_delim(FILE *stream, jstab_t *tab, char *delim);

#endif /* __JSTAB_H_ */
