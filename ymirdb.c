/**
 * comp2017 - assignment 2
 * <your name>
 * <your unikey>
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <stdbool.h>

#include "ymirdb.h"

//
// We recommend that you design your program to be
// modular where each function performs a small task
//
// e.g.
//
// command_bye
// command_help
// ...
// entry_add
// entry_delete
// ...
//

void command_bye() {
	printf("bye\n");
}

void command_help() {
	printf("%s\n", HELP);
}

int main(void) {

	char line[MAX_LINE];

	while (true) {
		printf("> ");

		if (NULL == fgets(line, MAX_LINE, stdin)) {
			printf("\n");
			command_bye();
			return 0;
		}

		//
		// TODO
		//
  	}

	return 0;
}
