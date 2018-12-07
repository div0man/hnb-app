#include "jstab.h"
#include "jsmn-extra.h"

/* check whether we have suitable data */
static int jstab_verify_data(jsmnts_t *ts, int ind);

int jstab_from_json_array(char *str, size_t len, jstab_t *tab, int ind)
{
	jsmnts_t ts = (jsmnts_t){str, len, NULL, 0};

	/* parse json data */
	int ntok = jsmnts_parse(&ts);

	if (ntok <= 0) {
		fprintf(stderr, "ERROR: jsmnts_parse failed: %d\n", ntok);
		ts.str = NULL; /* we don't want to clear the string */
		jsmnts_clear(&ts);
		return 0;
	}

	/* clean up any garbage from before */
	jstab_clear(tab);

	tab->nr = jstab_verify_data(&ts, ind);
	if (tab->nr > 0) {
		/* if we're here, it means that all tokens in the array are objects with
		 * only key-value pairs and consistent number of rows */
		tab->nc = ts.tok[ind + 1].size;

		/* allocate storage for all table pointers at once to avoid multiple
		 * syscalls */
		tab->keys = malloc(( (size_t)tab->nc + 
							 (size_t)tab->nr + 
							 (size_t)tab->nr * (size_t)tab->nc ) * 
							sizeof(char**));

		for (int i = 0, j = ind + 2; i < tab->nc; ++i, j+=2) {
			tab->keys[i] = &ts.str[ts.tok[j].start];
			ts.str[ts.tok[j].end] = '\0';
		}

		/* set cell pointers and slice the data blob */
		tab->rows = (char ***)&tab->keys[tab->nc];
		for (int k = 0; k < tab->nr; ++k) {
			tab->rows[k] = &tab->keys[tab->nc + tab->nr + k * tab->nc];
			for (int i = 0, j = ind + 3 + k * (tab->nc*2+1); i < tab->nc; ++i, 
					j+=2) {
				tab->rows[k][i] = &ts.str[ts.tok[j].start];
				ts.str[ts.tok[j].end] = '\0';
			}
		}

		/* hand over the blob from ts to tab */
		tab->blob = ts.str;
		ts.str = NULL;
		tab->sz = ts.len + 1;

		/* clean up */
		jsmnts_clear(&ts);

		return tab->nr;
	}

	return 0;
}

/* clar tab */
void jstab_clear(jstab_t *tab)
{
	free(tab->blob);
	free(tab->keys);
	*tab = (jstab_t){0};
}

/* print table */
void jstab_fprint_delim(FILE *stream, jstab_t *tab, char *delim)
{
	/* print header */
	for (int i = 0, last = tab->nc - 1; i < tab->nc; ++i) {
		if (i != last)
			fprintf(stream, "%s%s", tab->keys[i], delim);
		else
			fprintf(stream, "%s\n", tab->keys[i]);
	}

	/* print data */
	for (int k = 0; k < tab->nr; ++k) {
		for (int i = 0, last = tab->nc - 1; i < tab->nc; ++i) {
			if (i != last)
				fprintf(stream, "%s%s", tab->rows[k][i], delim);
			else
				fprintf(stream, "%s\n", tab->rows[k][i]);
		}
	}
}

static int jstab_verify_data(jsmnts_t *ts, int ind)
{
	/* check that we actually have some data to work with */
	if (ts->str == NULL || ts->len == 0 || ts->tok == NULL || ts->tsz == 0
			|| ind + 1 > ts->tsz) {
		fprintf(stderr, "ERROR: Invalid string or token data\n");
		return -1;
	}
	/* check root element */
	if (ts->tok[ind].end > (int) ts->len) { /* safe, we limit len to INT_MAX */
		fprintf(stderr, "ERROR: Root token size and string length mismatch\n");
		return -1;
	}
	if (ts->tok[ind].type != JSMN_ARRAY) {
		fprintf(stderr, "ERROR: Expected array as root element\n");
		return -1;
	}
	if (ts->tok[ind].size == 0) {
		/* this is acceptable, but give warning */
		fprintf(stderr, "WARNING: Empty array, no data\n");
		return 0;
	}

	int cols = 0;
	/* loop through objects in the array*/
	for (int i = ind + 1; ts->tok[i].type != JSMN_ARRAY && i < ts->tsz; ) {
		/* must be an object */
		if (ts->tok[i].type != JSMN_OBJECT) {
			fprintf(stderr, "ERROR: Expected only objects in the array\n");
			ts->str[ts->tok[i].end] = 0;
			return -1;
		}
		/* initialize `cols` with count of key-value pairs, must be >0 */
		if (cols == 0) {
			if (ts->tok[i].size > 0) {
				cols = ts->tok[i].size;
			}
			else {
				fprintf(stderr, "WARNING: Empty first object, no data\n");
				return 0;
			}
		}
		/* check objects for consistent number of key-value pairs */
		if (cols != ts->tok[i].size) {
			fprintf(stderr, "ERROR: Inconsistent number of keys\n");
			return -1;
		}
		/* confirm that our object contains only key-value pairs */
		int j;
		int last = i + cols*2 - 1; /* index of last key */
		for (j = i + 1; j <= last; j+=2)
			if (ts->tok[j].type != JSMN_STRING) {
				fprintf(stderr, "ERROR: Expected key\n");
				return -1;
			}
		++last; /* index of last value */
		for (j = i + 2; j <= last; j+=2)
			if (ts->tok[j].type != JSMN_STRING && 
					ts->tok[j].type != JSMN_PRIMITIVE) {
				fprintf(stderr, "ERROR: Expected value\n");
				return -1;
			}
		i = last + 1;
	}
	return ts->tok[ind].size; /* number of rows */
}
