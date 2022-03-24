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
#include <ctype.h>

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

int isnumber(char s[]){
    for (int i = 0; s[i]!= '\0'; i++){
        if (isdigit(s[i]) == 0)
              return 0;
    }
    return 1;
}

int cmpfunc(const void* a,const void* b){
	return (*(int*)b-*(int*)a);
}

void command_bye() {
	printf("bye\n");
}

void command_help() {
	printf("%s\n", HELP);
}

void command_list_keys(){
	printf("displays all keys in current state\n");
}

void command_list_entries(){
	printf("displays all entries in current state\n");
}

void command_list_snapshots(){
	printf("displays all snapshots in the database\n");
}

void command_get(char * line, struct entry * all_entries){
	// char * this_key = strtok(line, " ");
	// struct entry an_entry;
	// bool found = false;
	// for(int i = 0; i < sizeof(all_entries);i++){
	// 	an_entry = all_entries[i];
	// 	printf("find key: %s current key: %s\n", this_key, an_entry.key);
	// 	if(strcmp(this_key,an_entry.key)){
	// 		found = true;
	// 		break;
	// 	}
	// }

	// if(found){
	// 	printf("[");
	// 	for(int i = 0; i < an_entry.length; i++){
	// 		printf("%d", an_entry.values[i].value);
	// 	}
	// 	printf("]\n");
	// }

	return;

}

struct entry * command_del(char * line, struct entry *all_entries){
	// char * this_key = strtok(line, " ");
	// struct entry * an_entry;
	// bool found = false;
	// for(int i = 0; i < sizeof(all_entries); i++){
	// 	an_entry = all_entries[i];
	// 	if(strcmp(this_key,an_entry->key)){
	// 		free(an_entry);
	// 		all_entries[i] = NULL;
	// 		found = true;
	// 		break;
	// 	}
	// }

	// if(!found){
	// 	printf("Key Not Found");
	// 	return NULL;
	// }

	// // sort entries to hopefully end up with null value at the end of the allocated
	// // memory CHECK THIS!!!
	// qsort(all_entries,sizeof(all_entries),sizeof(struct entry *), cmpfunc);

	// // shrink all_entries to remove deleted entry
	// all_entries = realloc(all_entries, sizeof(all_entries) - sizeof(struct entry *));
	return all_entries;
}

void command_purge(){
	printf("deletes entry from current state and snapshots\n");
}

void command_set(char * line, struct entry * to_set){
	char * ptr = &line[0] + 4;
	if(!isalpha(*ptr)){
		printf("KEY MUST BE ALPHANUMERIC, BEGININNING WITH AN ALPHABETICAL CHARACTER\n");
		return;
	}

	// read the key
	char this_key[MAX_KEY];
	char * token = strtok(ptr," ");
	for(int i = 0; i < sizeof(this_key); i++){
		this_key[i] = *token;
		token++;
	}

	struct element this_values[MAX_LINE];
	struct element *value_ptr = &this_values[0];
	token = strtok(ptr, " ");

	while(token != NULL){
		if(isnumber(token) == 1){
			value_ptr->value = atoi(token);
		} // else{} for if value is a reference to another entry, deal with later
		token = strtok(NULL, " ");
		value_ptr++;
	}

	int this_length = value_ptr-&this_values[0];

	struct entry this_entry;
	strcpy(this_entry.key,this_key);
	this_entry.values = malloc(sizeof(struct element)*this_length);
	this_entry.length = this_length;
	*to_set = this_entry;
	return;
}

void command_push(){
	printf("pushes values to the front\n");
}

void command_append(){
	printf("appends values to the back\n");
}

void command_pick(){
	printf("displays value at index\n");
}

void command_pluck(){
	printf("displays and removes value at index\n");
}

void command_pop(){
	printf("displays and removes the front value\n");
}

void command_drop(){
	printf("deletes snapshot\n");
}

void command_rollback(){
	printf("restores to snapshot and deletes newer snapshots\n");
}

void command_checkout(){
	printf("replaces current state with a copy of snapshot\n");
}

void command_snapshot(){
	printf("saves the current state as a snapshot\n");
}

void command_min(){
	printf("displays minimum value\n");
}

void command_max(){
	printf("displays maximum value\n");
}

void command_sum(){
	printf("displays sum of values\n");
}

void command_len(){
	printf("displays number of values");
}

void command_rev(){
	printf("reverses order of values (simple entry only)\n");
}

void command_uniq(){
	printf("removes repeated adjacent values (simple entry only)\n");
}

void command_sort(){
	printf("sorts values in ascending order (simple entry only)\n");
}

void command_forward(){
	printf("lists all the forward references of this key\n");
}

void command_backward(){
	printf("lists all the backward references of this key\n");
}

void command_type(){
	printf("displays if the entry of this key is simple or general\n");
}

int command_interpreter(char * command, struct entry * all_entries, int * ptr_number_of_entries){

	if(strncasecmp(command,"bye",3)==0){
		for(int i = 0; i < *ptr_number_of_entries;i++){
			free(all_entries[i].values);
			all_entries[i].values = NULL;
		}
		free(all_entries);
		all_entries = NULL;
		command_bye();
		return -1;
	}else if(strncasecmp(command,"help",4)==0){
		command_help();
	}else if(strncasecmp(command,"list keys",9)==0){
		command_list_keys();
	}else if(strncasecmp(command,"list entries",12)==0){
		command_list_entries();
	}else if(strncasecmp(command,"list snapshots",14)==0){
		command_list_snapshots();
	}else if(strncasecmp(command,"get",3)==0){
		// char * line = &command[0]+4;
		// command_get(line, all_entries);
	}else if(strncasecmp(command,"del",3)==0){
		// char * line = &command[0]+4;
		// command_del(line, all_entries);
	}else if(strncasecmp(command,"purge",5)==0){
		command_purge();
	}else if(strncasecmp(command,"set",3)==0){
		all_entries = realloc(all_entries,(*ptr_number_of_entries + 1) + sizeof(struct entry));
		*ptr_number_of_entries++;
		command_set(command,all_entries[*ptr_number_of_entries-1]);
	}else if(strncasecmp(command,"push",4)==0){
		command_push();
	}else if(strncasecmp(command,"append",6)==0){
		command_append();
	}else if(strncasecmp(command,"pick",4)==0){
		command_pick();
	}else if(strncasecmp(command,"pluck",5)==0){
		command_pluck();
	}else if(strncasecmp(command,"pop",3)==0){
		command_pop();
	}else if(strncasecmp(command,"drop",3)==0){
		command_drop();
	}else if(strncasecmp(command,"rollback",8)==0){
		command_rollback();
	}else if(strncasecmp(command,"checkout",8)==0){
		command_checkout();
	}else if(strncasecmp(command,"snapshot",8)==0){
		command_snapshot();
	}else if(strncasecmp(command,"min",3)==0){
		command_min();
	}else if(strncasecmp(command,"max",3)==0){
		command_max();
	}else if(strncasecmp(command,"sum",3)==0){
		command_sum();
	}else if(strncasecmp(command,"len",3)==0){
		command_len();
	}else if(strncasecmp(command,"rev",3)==0){
		command_rev();
	}else if(strncasecmp(command,"uniq",4)==0){
		command_uniq();
	}else if(strncasecmp(command,"sort",4)==0){
		command_sort();
	}else if(strncasecmp(command,"forward",7)==0){
		command_forward();
	}else if(strncasecmp(command,"backward",8)==0){
		command_backward();
	}else if(strncasecmp(command,"type",4)==0){
		command_type();
	}else{ 
		printf("INVALID COMMAND: TYPE HELP FOR A LIST OF VALID COMMANDS\n");
	}

	free(all_entries);
	all_entries = NULL;
	return 0;
}
int main(void) {

	char line[MAX_LINE];
	// create an array of entry structs
	int number_of_entries = 0;
	struct entry *all_entries = malloc(sizeof(struct entry));

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

		if(command_interpreter(line,all_entries,&number_of_entries) == -1){
			return 0;
		}

  	}
	

	return 0;
}
