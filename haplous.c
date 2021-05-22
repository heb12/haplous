#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "haplous.h"

static bool strbool(const char *str, int *error)
{
	if (strcmp(str, "true") == 0) {
		return true;
	} else if (strcmp(str, "false") == 0) {
		return false;
	}

	*error = -1;
	return false;
}

static void check_or_err(const char *string, int *err)
{
	if (strcmp(string, "") == 0) {
		*err = HAPLOUS_META_MISSING;
	}
}

struct haplous_work haplous_work_init(const char *path, int *error)
{
	struct haplous_work w;

	w.path = path;
	w.file = fopen(path, "r");
	if (w.file == NULL) {
		*error = HAPLOUS_WORK_NOT_FOUND;
		return w;
	}

	// strcpy is fine here because haplous_work_metadata_get ensures a NULL
	// terminator
	// TODO make error handling less verbose
	strcpy(w.metadata.lang, haplous_work_metadata_get(w.file, "lang"));
	check_or_err(w.metadata.lang, error);

	strcpy(w.metadata.title, haplous_work_metadata_get(w.file, "title"));
	check_or_err(w.metadata.title, error);

	char *public_domain_str =
		haplous_work_metadata_get(w.file, "public_domain");
	check_or_err(public_domain_str, error);

	int err = 0;
	w.metadata.public_domain = strbool(public_domain_str, &err);
	if (err != 0)
		*error = HAPLOUS_META_MISSING;

	strcpy(w.metadata.id, haplous_work_metadata_get(w.file, "id"));
	check_or_err(w.metadata.id, error);

	strcpy(w.metadata.type, haplous_work_metadata_get(w.file, "type"));
	check_or_err(public_domain_str, error);

	*error = HAPLOUS_OK;
	return w;
}

// Free buffers and close files
int haplous_work_cleanup(struct haplous_work *w)
{
	fclose(w->file);
	return 0;
}
