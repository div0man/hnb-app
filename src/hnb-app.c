#include "hnb-api.h"
#include "jstab.h"

int main(int argc, char **argv)
{
	char *url = HNB_API_URL;
	char *query;
	char *delim;
	char *blankstr = "";
	char *defdelim = "\t";

	/* set query */
	if (argc > 1)
		query = argv[1];
	else
		query = blankstr;
	/* set table delimiter */
	if (argc > 2)
		delim = argv[2];
	else
		delim = defdelim;

	jstab_t tab = {0};
	for (jsmnts_t ts = {0}; hnbapi_http_get_jsmnts(url, query, &ts); ) {
		if (jstab_morph(&ts, &tab, 0))
			jstab_fprint_delim(stdout, &tab, delim);
		break;
	}
	jstab_clear(&tab);

	return 0;
}
