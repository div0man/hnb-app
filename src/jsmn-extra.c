#include "jsmn-extra.h"

void jsmnts_clear(jsmnts_t *ts)
{
	if (ts->str != NULL)
		free(ts->str);
	if (ts->tok != NULL)
		free(ts->tok);
	*ts = (jsmnts_t){0};
}

int jsmnts_parse(jsmnts_t *ts)
{
	jsmn_parser parser;
	jsmn_init(&parser);

	/* while our string is size_t, jsmn lib limited to `int`. */
	if (ts->len > INT_MAX - 1) {
		fprintf(stderr, "ERROR: Data too big for jsmn_parse\n");
		return 0;
	}

	/* run parser to get required number of json tokens */
	ts->tsz = jsmn_parse(&parser, ts->str, ts->len, NULL, 0);
	if (ts->tsz < 0) {
		fprintf(stderr, "ERROR: jsmn_parse failed: %d\n", 
				ts->tsz);
		return 0;
	}

	/* allocate token storage and parse string into tokens */
	jsmn_init(&parser);
	ts->tok = (jsmntok_t *)realloc(ts->tok, (size_t)ts->tsz * 
			sizeof(jsmntok_t));
	if (ts->tok == NULL) {
		fprintf(stderr, "ERROR: Insufficient memory for token data\n");
		return 0;
	}
	jsmn_parse(&parser, ts->str, ts->len, ts->tok, 
			(unsigned int) ts->tsz);
	return ts->tsz;
}
