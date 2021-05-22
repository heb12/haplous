#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <time.h>

#include "haplous.h"

struct haplous_work work;
int err = 0;

void testRef(char *name, int chapter, int start, int to)
{
	struct haplous_reference ref = {
		name,
		chapter,
		start,
		to,
	};
	
	char *text =
		haplous_work_verses_get(work.file, ref, &err);

	if (err != HAPLOUS_OK) {
		printf("Error parsing: %s %d %d:%d (%d)\n", name, chapter, start, to, err);
	}

	// Demo didn't have this - leaked a lot of memory..
	free(text);
}

int main()
{
	work = haplous_work_init("kjv.txt", &err);
	if (err != HAPLOUS_OK) {
		puts("Err");
		return -1;
	}

	clock_t start_time = clock();
	for (int i = 0; i < 2; i++) {
		testRef("Rev", 1, 1, 1);
	}

	double elapsed_time =
		(double)(clock() - start_time) / CLOCKS_PER_SEC;

	printf("Done in %f seconds\n", elapsed_time);
	
	haplous_work_cleanup(&work);
}
