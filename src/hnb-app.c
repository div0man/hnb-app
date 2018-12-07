#include "hnb-api.h"
#include "jstab.h"

int main(int argc, char **argv)
{
	char *url = HNBAPI_URL;
	char *query;
	char *delim;
	char *blankstr = "";
	char *delim_default = "\t";

	/* set query */
	if (argc > 1)
		query = argv[1];
	else
		query = blankstr;

	/* set table delimiter */
	if (argc > 2)
		delim = argv[2];
	else
		delim = delim_default;

	jstab_t tab = {0};
	{ /* limit the scope for temporaries */
		char *str = NULL;
		size_t len = 0;
		if (hnbapi_http_get(url, query, &str, &len) == 0) {
			if (jstab_from_json_array(str, len, &tab, HNBAPI_TABLE_TOKEN_INDEX))
				jstab_fprint_delim(stdout, &tab, delim);
		}
	}
	jstab_clear(&tab);

	return 0;
}
