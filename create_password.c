/* create_password.c */

#include <time.h>
#include <stdio.h>
#include <stdlib.h>

enum { default_password_length = 10 , default_password_count = 1 };


int compare_str(char *str1, char *str2) {
	/* 0 - not equal; 1 - equal */

	for (; *str1 || *str2; str1++, str2++) {
		if (*str1 != *str2) {
			return 0;
		}
	}
	return 1;
}
int str_len(const char *str) {
	int len = 0;
	
	for (; *str; str++) {
		len++;
	}

	return len;
}
int arr_contains_str(int arr_len, char **arr, char *str) {
	int i;
	for (i = 0; i < arr_len; i++) {
		if (compare_str(arr[i], str)) {
			return 1;
		}
	}
	return 0;
}
void show_help_msg(char *program_name) {
	printf("Program creates password using a user-defined charset.\n");
	printf("Usage: %s [arguments]\n\n", program_name);
	printf("Arguments:\n");
	printf("\t-h\t\t\tShow this help message\n");

	printf("\t-l [value]\t\tLength of the password (%d by default)\n", default_password_length);

	printf("\t-c <value>\t\tCount of generated passwords\n");

	printf("\t-cs <charset>\t\tUsing charset (required) in format:\n");
	printf("\t\ta-z - any lowercase value from 'a' to 'z'\n");
	printf("\t\tA-Z - any uppercase value from 'A' to 'Z'\n");
	printf("\t\t0-9 - any digit from '0' to '9'\n");
	printf("\t\tad;8D - provided values\n");

	printf("\t-v\t\t\tVerbose (with using length and charset)\n");

	printf("\n");
}
int *expand_sequence(const char seq_start, const char seq_end, const char *charset, char *p);

char *expand_charset(const char *charset) {
	char *result = malloc(sizeof(char) * 128);
	char *p = result;
	int *ptr_offset;

	while (*charset) {
		if (*charset >= 'a' && *charset <= 'z') {
			ptr_offset = expand_sequence('a', 'z', charset, p);
			charset += ptr_offset[0];
			p += ptr_offset[1];
			free(ptr_offset);
			continue;
		}

		if (*charset >= 'A' && *charset <= 'Z') {
			ptr_offset = expand_sequence('A', 'Z', charset, p);
			charset += ptr_offset[0];
			p += ptr_offset[1];
			free(ptr_offset);
			continue;
		}

		if (*charset >= '0' && *charset <= '9') {
			ptr_offset = expand_sequence('0', '9', charset, p);
			charset += ptr_offset[0];
			p += ptr_offset[1];
			free(ptr_offset);
			continue;
		}

		if (*charset > 32 && *charset < 127) {
			*p = *charset;
			p++;
			charset++;
		}

	}
	*p = '\0';
	return result;
}
int *expand_sequence(const char seq_start, const char seq_end, const char *charset, char *p) {
	char c, start_c, end_c;
	int *ptr_offset = malloc(sizeof(int) * 2);
	ptr_offset[0] = 0;
	ptr_offset[1] = 0;

	start_c = charset[ptr_offset[0]++];
	c = charset[ptr_offset[0]];
	if (c == '-') {
		c = charset[++ptr_offset[0]];

		if (c >= seq_start && c <= seq_end) {
			end_c = c;
		} else {
			end_c = start_c;
			ptr_offset[0]--;
		}

		do {
			p[ptr_offset[1]++] = start_c;
			start_c++;
		} while (start_c <= end_c);
		ptr_offset[0]++;
	} else {
		p[ptr_offset[1]++] = start_c;
	}
	return ptr_offset;
}
void generate_password(int count, int length, char *charset) {
	int i, j;
	int charset_length = str_len(charset);

	srand(time(NULL));
	i = 0;
	while (i < count) {
		char *password = malloc(sizeof(char) * length + 1);
		for (j = 0; j < length; j++) {
			short charset_index = rand() % charset_length;
			*(password + j) = charset[charset_index];
		}
		*(password + j) = '\0';
		printf("\nGenerated password: %s\n", password);

		i++;
		free(password);
	}
	
}
int main(int argc, char **argv) {
	int i;
	int verbose = 0;
	char *available_charset = NULL;
       	int password_count = default_password_count;
	int password_length = default_password_length;

	/* if user requested -h, then show him a help message and exit */
	if (arr_contains_str(argc, argv, "-h")) {
		show_help_msg(argv[0]);
		return 0;
	}

	/* if no -h, then read for provided params */
	for (i = 1; i < argc; i++) {
		if (compare_str(argv[i], "-v")) {
			verbose = 1;
		}
		if (compare_str(argv[i], "-c")) {
			i++;
			if (!argv[i]) {
				printf("Error: didn't provide count of passwords\n");
				return 1;
			}
			password_count = atoi(argv[i]);
		}
		if (compare_str(argv[i], "-l")) {
			i++;
			if (!argv[i]) {
				printf("Error: didn't provide password length\n");
				return 1;
			}
			password_length = atoi(argv[i]);
		}
		if (compare_str(argv[i], "-cs")) {
			i++;
			if (!argv[i]) {
				printf("Error: didn't provide charset\n");
				return 1;
			}
			available_charset = expand_charset(argv[i]);
		}

	}

	if (!available_charset) {
		printf("Please, provide a charset with '-cs' arg!\n");
		return 1;
	}


	generate_password(password_count, password_length, available_charset);

	/* if -v, get information about what was used for generation */
	if (verbose) {
		printf("\n");
		printf("Password count: %d\n", password_count);
		printf("Password length: %d\n", password_length);
		printf("Using charset: %s\n", available_charset);
		printf("\n");
	}

	return 0;
}
