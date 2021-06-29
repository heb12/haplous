#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "haplous.h"

// seeks through a file line by line (with fgets) until it gets to the requested
// book
static int haplous_work_book_seek(FILE *file, char *id)
{
	int n = 0;
	char line[17]; // #book:exod (ids can be up to 10 characters long)
	while (fgets(line, 17, file)) {
		if (line[0] == '\n') {
			n++;
			continue;
		}

		if (strncmp(line, "#book:", 6) == 0) {
			for (size_t i = 0; i != 10; ++i) {
				if (line[i + 6] != id[i] || i == strlen(id)) {
					goto continue_while;
				}

				if (line[i + 7] == '\n') {
					return n;
				}
			}
		}

	continue_while:
		n++;
	}

	return HAPLOUS_REF_NOT_FOUND;
}

// run haplous_work_book_find first
static int haplous_work_chapter_seek(FILE *file, size_t chapter)
{
	int n = 0;
	char line[13]; // #chapter:150

	size_t c = 0;

	while (fgets(line, 13, file)) {
		if (strncmp(line, "#chapter:", 9) == 0) {
			c++; // increase it first since "chapter" starts at 1
			if (c == chapter) {
				return n;
			}
		}

		n++;
	}

	return HAPLOUS_REF_NOT_FOUND;
}

// Get a range of verses
// Allocates memory which caller owns in the end
// TODO support custom allocators
char *haplous_work_verses_get(FILE *file, struct haplous_reference ref,
			      int *err)
{
	if (ref.verse_start == 0) {
		*err = HAPLOUS_INVALID_REF;
		return NULL;
	}
	if (ref.verse_start > ref.verse_end) {
		*err = HAPLOUS_INVALID_REF;
		return NULL;
	}

	size_t buf_size = 1000;
	char *buffer = calloc(buf_size, sizeof(char));

	if (file == NULL) {
		*err = HAPLOUS_WORK_NOT_FOUND;
		return NULL;
	}
	fseek(file, 0, SEEK_SET);
	int line = 0;
	line = haplous_work_book_seek(file, ref.id);
	line = haplous_work_chapter_seek(file, ref.chapter);
	if (line < 0) {
		*err = HAPLOUS_REF_NOT_FOUND;
		return NULL;
	}

	int prev = '\0';
	int c;
	size_t bufi = 0;
	size_t verse = 1;
	while ((c = getc(file)) != EOF) {
		// detect end of the chapter
		if (prev == '\n' && c == '^') {

			// if at the end of the chapter, make sure it has found
			// all the required verses
			// TODO test
			if (verse < ref.verse_end) {
				*err = HAPLOUS_END_TOO_BIG; // TODO maybe
							    // OUT_OF_RANGE?
				return NULL;
			}
		}

		if (verse >= ref.verse_start && verse <= ref.verse_end) {
			if (bufi >= buf_size) {
				// Increase by 2 to make sure there's always
				// space for the NULL terminator
				buf_size += 2;
				buffer = realloc(buffer, buf_size);
				if (buffer == NULL) {
					*err = HAPLOUS_OUT_OF_MEMORY;
					return buffer;
				}
			}
			buffer[bufi] = (char)c;
			bufi++;
		} else if (verse >= ref.verse_end) {
			break;
		}

		if (c == '\n') {
			verse++;
		}

		prev = c;
	}

	buffer[bufi] = '\0';

	*err = HAPLOUS_OK;
	return buffer;
}

// Get text from a full chapter separated by "\n"
// disregards any verse information in ref
// returns HAPLOUS_REF_NOT_FOUND upon errors
// Allocates same as haplous_work_verses_get
// It can return NULL, but never when an error is not set
// TODO figure out error handling again
// TODO decide whether or not to initialize buffer within or ourside of function
char *haplous_work_chapter_get(FILE *file, struct haplous_reference ref,
			       int *err)
{
	if (ref.chapter == 0) {
		*err = HAPLOUS_INVALID_REF;
		return NULL;
	}

	if (file == NULL) {
		*err = HAPLOUS_WORK_NOT_FOUND;
		return NULL;
	}
	fseek(file, 0, SEEK_SET);
	int line = 0;
	line = haplous_work_book_seek(file, ref.id);
	line = haplous_work_chapter_seek(file, ref.chapter);
	if (line < 0) {
		*err = HAPLOUS_REF_NOT_FOUND;
		return NULL;
	}

	size_t buf_size = 1000;
	char *buffer = malloc(buf_size);
	if (buffer == NULL) {
		*err = HAPLOUS_OUT_OF_MEMORY;
		return NULL;
	}

	int c;
	size_t i = 0;
	while ((c = getc(file)) != EOF) {
		if (c == '^') {
			break;
		}

		if (i >= buf_size) {
			buf_size += 1;
			buffer = realloc(buffer, buf_size);
			if (buffer == NULL) {
				*err = HAPLOUS_OUT_OF_MEMORY;
				return buffer;
			}
		}

		buffer[i] = (char)c;
		i++;
	}

	buffer[i] = '\0';

	*err = HAPLOUS_OK;
	return buffer;
}

struct haplous_reader haplous_reader_new(struct haplous_work work, struct haplous_reference ref, int *err) {
	struct haplous_reader reader = {
		.work = work,
		.reference = ref,
		.verse = NULL,
		.current_verse = 1,
	};

	fseek(reader.work.file, 0, SEEK_SET);
	int line = 0;
	line = haplous_work_book_seek(reader.work.file, reader.reference.id);
	line = haplous_work_chapter_seek(reader.work.file, reader.reference.chapter);
	if (line < 0) {
		*err = HAPLOUS_REF_NOT_FOUND;
		return reader;
	}

	return reader;
}

// haplous_next provides an interface for obtaining a reference verse-by-verse
int haplous_next(struct haplous_reader *reader) {

	// seek to the first verse
	int c;
	while ((c = getc(reader->work.file)) != EOF) {
		if (c == '\n') {
			reader->current_verse += 1;
		}

		if (reader->current_verse >= reader->reference.verse_start) {
			break;
		}
	}

	size_t buf_size = 500;
	char *buffer = malloc(buf_size);
	if (buffer == NULL) {
		return HAPLOUS_OUT_OF_MEMORY;
	}

	// load the verse to the buffer
	size_t i = 0;
	while ((c = getc(reader->work.file)) != EOF) {
		if (c == '\n') {
			break;
		}

		if (i >= buf_size) {
			buf_size += 1;
			buffer = realloc(buffer, buf_size);
			if (buffer == NULL) {
				free(buffer);
				return HAPLOUS_OUT_OF_MEMORY;
			}
		}

		buffer[i] = (char)c;
		i++;
	}
	buffer[i] = '\0';

	reader->verse = buffer;
	if (reader->current_verse <= reader->reference.verse_end) {
		reader->current_verse += 1;
		return HAPLOUS_CONTINUE;
	} else {
		return HAPLOUS_OK;
	}
}
