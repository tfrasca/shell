#include <stdio.h>

int main(int argc, char *argv[]) {
	FILE *fp;
	char *line = NULL;
	size_t len = 0;
	ssize_t line_len = 0;

	fp = fopen(argv[1], "r");

	while(line_len != -1) {
		line_len = getline(&line, &len, fp);
		printf("%s", line);
	}

	fclose(fp);
}