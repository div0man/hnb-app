#ifndef __JSTAB_H_
#define __JSTAB_H_

#include <stddef.h>
#include <stdio.h>

typedef struct {
	char **keys;	/* keys row stored separately for convenience */
	int nc;			/* number of columns */
	char ***rows;	/* pointer to pointer to first cell in the row */
	int nr;			/* number of rows */
	char *blob;		/* actual json data as one big array,
					   sliced using '\0' bytes */
	size_t sz;		/* size of the blob */
} jstab_t;

/* Takes a json string, goes to specific token and if it's an array containing
 * only key-value pairs, converts it to a table. On success, it steals the 
 * string and de-references str pointer. Returns number of rows read */
int jstab_from_json_array(char *str, size_t len, jstab_t *tab, int ind);

/* clar tab */
void jstab_clear(jstab_t *tab);

/* print tab */
void jstab_fprint_delim(FILE *stream, jstab_t *tab, char *delim);

#endif /* __JSTAB_H_ */
