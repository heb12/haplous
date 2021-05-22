// Getting metadata and other information from a Work
#include <stdio.h>
#include <string.h>
#include "haplous.h"

// Gets metadata in the form of `#id:value` from a work
// Returns empty string if the ID is not found before a book
// or if the associated value is empty
char *haplous_work_metadata_get(FILE *file, const char id[MAX_ID_LEN])
{
	fseek(file, 0, SEEK_SET);

	char line[MAX_ID_LEN * 2 + 1];
	static char value[MAX_ID_LEN + 1];
	strcpy(value, "");

	char cmpid[MAX_ID_LEN + 2];
	strcpy(cmpid, "#");
	strncat(cmpid, id, MAX_ID_LEN);

	while (fgets(line, MAX_ID_LEN * 2 + 1, file)) {
		if (strncmp(line, "#book", 5) == 0) {
			break;
		}

		size_t index = 0;
		while (line[index] != '\0' && index < MAX_ID_LEN + 2) {
			if (line[index] == ':')
				break;
			index++;
		}

		// index must be greater than 2 to allow for at least #i:v
		if (index > 2 && strncmp(line, cmpid, index - 1) == 0) {
			size_t i = index + 1;
			while (i < index + MAX_ID_LEN && line[i] != '\0'
			       && line[i] != '\n') {
				value[i - index - 1] = line[i];
				i++;
			}

			value[i - index - 1] = '\0';
			break;
		}
	}

	value[MAX_ID_LEN] = '\0'; // just in case

	return value;
}
