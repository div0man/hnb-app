#include <limits.h>
#include <stdlib.h>
#include <stdio.h>
#include "jsmn.h"

#ifndef __JSMN_EXTRA_H_
#define __JSMN_EXTRA_H_

/* convenient storage of both the string and token data */
typedef struct {
	char *str;
	size_t len;
	jsmntok_t *tok;
	int tsz;
} jsmnts_t;

void jsmnts_clear(jsmnts_t *ts);

int jsmnts_parse(jsmnts_t *ts);

#endif /* __JSMN_EXTRA_H_ */
