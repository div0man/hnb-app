#include "jsmn-extra.h"

#define HNB_API_URL "https://api.hnb.hr/tecajn/v1"
#define HNB_API_KEYS { "Broj tečajnice", "Datum primjene", "Država", \
		"Šifra valute", "Valuta", "Jedinica", "Kupovni za devize", \
		"Srednji za devize", "Prodajni za devize" };
#define HNB_API_NKEYS 9

/* Uses curl to get http response and writes it into dynamic array addressed
 * in the ts object and uses jsmn parser to populate token data.
 * Returns number of tokens found. */
int hnbapi_http_get_jsmnts(char *url, char *post_fields, jsmnts_t *ts);
