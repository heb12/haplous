#ifndef HAPLOUS_H_
#define HAPLOUS_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdbool.h>

// numbers from the spec
#define MAX_ID_LEN 20

enum haplous_error {
	HAPLOUS_OK = 0,
	HAPLOUS_OTHER_ERROR = -1,
	HAPLOUS_INVALID_REF = -2,
	HAPLOUS_REF_NOT_FOUND = -3,
	HAPLOUS_END_TOO_BIG = -4,

	HAPLOUS_INVALID_HAPLOUS = -5,

	HAPLOUS_META_MISSING = -6,
	HAPLOUS_META_FOUND_BOOK = -7,

	HAPLOUS_WORK_NOT_FOUND = -8,

	HAPLOUS_OUT_OF_MEMORY = -9,
};

struct haplous_reference {
	char *id;
	size_t chapter;
	size_t verse_start;
	size_t verse_end;
};

struct haplous_work_metadata {
	char lang[MAX_ID_LEN];
	char title[MAX_ID_LEN];
	char id[MAX_ID_LEN];
	bool public_domain;
	char type[MAX_ID_LEN];
	// this is only the required metadata,
	// other metadata may be available via work_metadata_get(id)
};

struct haplous_work {
	const char *path;
	FILE *file;
	struct haplous_work_metadata metadata;
};

struct haplous_work haplous_work_init(const char *, int *);
int haplous_work_cleanup(struct haplous_work *);

char *haplous_work_chapter_get(FILE *, struct haplous_reference, int *);
char *haplous_work_verses_get(FILE *, struct haplous_reference, int *);

char *haplous_work_metadata_get(FILE *, const char[MAX_ID_LEN]);

#ifdef __cplusplus
}
#endif

#endif