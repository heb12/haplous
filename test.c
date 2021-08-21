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
		to + 1,
	};

	struct haplous_reader reader = haplous_reader_new(work, ref, &err);
	assert(err == 0);

	while (haplous_next(&reader) == HAPLOUS_CONTINUE) {
		//puts(reader.verse);
		free(reader.verse);
	}

	free(reader.verse);

	if (err != HAPLOUS_OK) {
		printf("Error parsing: %s %d %d:%d (%d)\n", name, chapter, start, to, err);
	}
}

int main()
{
	work = haplous_work_init("../kjv.txt", &err);
	if (err != HAPLOUS_OK) {
		puts("Err");
		return -1;
	}

	clock_t start_time = clock();
	testRef("John", 3, 16, 20);

	double elapsed_time =
		(double)(clock() - start_time) / CLOCKS_PER_SEC;

	printf("Done in %f seconds\n", elapsed_time);
	haplous_work_cleanup(&work);
}
