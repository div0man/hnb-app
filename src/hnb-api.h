#include <stddef.h>

#define HNBAPI_URL "https://api.hnb.hr/tecajn/v1"
#define HNBAPI_TABLE_TOKEN_INDEX 0
#define HNBAPI_TABLE_KEYS { "Broj tečajnice", "Datum primjene", "Država", \
		"Šifra valute", "Valuta", "Jedinica", "Kupovni za devize", \
		"Srednji za devize", "Prodajni za devize" };
#define HNBAPI_TABLE_NKEYS 9

/* Uses curl to get http response and writes it into a string.
 * Assumes str == NULL and *len == 0.
 * Returns 0 for success, -1 for failure. */
int hnbapi_http_get(char *url, char *post_fields, char **str, size_t *len);
