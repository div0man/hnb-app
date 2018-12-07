#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include "hnb-api.h"

/* args wrapper, we can pass only 1 arg to curl write function */
typedef struct {
	char *str;
	size_t *len;
} store_t;

/* curl callback function to write received data */
static size_t hnbapi_write_callback(void *response, size_t bs, 
		size_t blocks, void *store);

/* query the API and write the response into str and update *len */
int hnbapi_http_get(char *url, char *post_fields, char **str, size_t *len)
{
	CURL *curl;
	CURLcode res;
	store_t store = (store_t){*str, len};
	int ret;

	curl_global_init(CURL_GLOBAL_ALL);

	curl = curl_easy_init();
	if(curl) {
		/* configure http request */
		curl_easy_setopt(curl, CURLOPT_URL, url);
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, post_fields);
 
		/* configure curl callback */
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, 
				hnbapi_write_callback);
 		curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&store);

		/* execute http request */
		res = curl_easy_perform(curl);

		/* data copied, clean up curl_easy */
		curl_easy_cleanup(curl);

		if(res == CURLE_OK) {
			*str = store.str;
			ret = 0;
		}
		else {
			fprintf(stderr, "ERROR: curl_easy_perform() failed: %s\n",
					curl_easy_strerror(res));
			ret = -1;
		}
	}
	else {
		fprintf(stderr, "ERROR: curl_easy_init() failed");
		ret = -1;
	}

	curl_global_cleanup();
	return ret;
}

static size_t hnbapi_write_callback(void *response, size_t bs,
		size_t blocks, void *store)
{
	size_t sz = bs * blocks;
	store_t *st = (store_t *)store;

	/* guard against overflow */
	if (*st->len > SIZE_MAX - sz - 1) {
		fprintf(stderr, "ERROR: Data too big to address\n");
		return 0;
	}

	/* store response into a string, expand as needed */
	char *ps = realloc(st->str, *st->len + sz + 1);
	if (ps == NULL) {
		fprintf(stderr, "ERROR: Insufficient memory for storing HTTP response "
				"string\n");
		return 0;
	}
	st->str = ps;
	memcpy(&(st->str[*st->len]), response, sz);
	*st->len += sz;
	st->str[*st->len] = '\0';

	return sz;
}
