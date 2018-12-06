#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include "hnb-api.h"

/* curl callback function to write received data */
static size_t hnbapi_write_jsmnts_callback(void *response, size_t bs, 
		size_t blocks, void *store);

/* query the API and write the response into ts */
int hnbapi_http_get_jsmnts(char *url, char *post_fields, jsmnts_t *ts)
{
	CURL *curl;
	CURLcode res;

	/* clean up any garbage from before */
	jsmnts_clear(ts);

	int ntok = 0;

	curl_global_init(CURL_GLOBAL_ALL);

	curl = curl_easy_init();

	if(curl) {
		/* configure http request */
		curl_easy_setopt(curl, CURLOPT_URL, url);
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, post_fields);
 
		/* configure curl callback */
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, 
				hnbapi_write_jsmnts_callback);
 		curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)ts);

		/* execute http request */
		res = curl_easy_perform(curl);

		/* data copied, clean up curl_easy */
		curl_easy_cleanup(curl);

		if(res == CURLE_OK) {
			ntok = jsmnts_parse(ts);
			if (ntok <= 0) {
				fprintf(stderr, "ERROR: jsmnts_parse failed: %d\n", ntok);
				jsmnts_clear(ts);
				ntok = 0;
			}
		}
		else {
			fprintf(stderr, "ERROR: curl_easy_perform() failed: %s\n",
					curl_easy_strerror(res));
			jsmnts_clear(ts);
			ntok = 0;
		}
	}

	curl_global_cleanup();
	return ntok;
}

static size_t hnbapi_write_jsmnts_callback(void *response, size_t bs,
		size_t blocks, void *store)
{
	size_t sz = bs * blocks;
	jsmnts_t *ts = (jsmnts_t *)store;

	/* guard against overflow */
	if (ts->len > SIZE_MAX - sz - 1) {
		fprintf(stderr, "ERROR: Data too big to address\n");
		return 0;
	}

	/* store response into a string, expand as needed */
	char *ps = realloc(ts->str, ts->len + sz + 1);
	if (ps == NULL) {
		fprintf(stderr, "ERROR: Insufficient memory for storing HTTP response "
				"string\n");
		return 0;
	}
	ts->str = ps;
	memcpy(&(ts->str[ts->len]), response, sz);
	ts->len += sz;
	ts->str[ts->len] = '\0';

	return sz;
}

