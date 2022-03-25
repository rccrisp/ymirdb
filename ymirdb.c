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

bool isnumber(char s[]){
    for (int i = 0; s[i]!= '\0'; i++){
        if (isdigit(s[i]) == 0 && s[i]!= '\n'){
			return false;
		}
              
    }
    return true;
}

node * list_init(){
	node * head = malloc(sizeof(node));
	head->next = NULL;
	return head;
}

void list_add(node * head, entry this_entry){
	node * last_node = head;
	while(last_node->next){
		last_node = last_node->next;
	}
	
	node * new_node = malloc(sizeof(node));
	new_node->item = this_entry;
	new_node->next = NULL;
	last_node->next = new_node;

	return;
}

void list_delete(node* head, node* n){

	node * prev_node = head;
	while(prev_node->next!=n){
		prev_node = prev_node->next;
	}
	prev_node->next = n->next;
	free(n);
}

node * list_next(node * n){
	return n->next;
}

void list_free(node * head){
	node * iter = head;
	while(iter->next){
		node * current = iter;
		iter = iter->next;
		free(current);
	}

	return ;
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

void command_get(char * line, node * head){
	char * key_to_find = strtok(line, " \n");
	bool found = false;
	// head is always NULL entry
	node * iter = head->next;
	entry this_entry;
	while(iter){
		this_entry = iter->item;
		if(strcmp(this_entry.key,key_to_find) == 0){
			found = true;
			break;
		}
		iter = iter->next;
	}

	
	if(found){
		printf("[");
		int i = 0;
		for(; i < this_entry.length-1; i++){
			printf("%d ", this_entry.values[i].value);
		}
		printf("%d", this_entry.values[i].value);
		printf("]\n\n");
	}else{
		printf("no such key\n\n");
	}

	return;

}

void command_del(char * line, node * head){
	char * key_to_find = strtok(line, " \n");
	bool found = false;
	// head is always NULL entry
	node * iter = head->next;
	entry this_entry;
	while(iter){
		this_entry = iter->item;
		if(strcmp(this_entry.key,key_to_find) == 0){
			found = true;
			break;
		}
		iter = iter->next;
	}

	
	if(found){
		list_delete(head, iter);
		free(this_entry.values);
		printf("ok\n\n");
	}else{
		printf("no such key\n\n");
	}
}

void command_purge(){
	printf("deletes entry from current state and snapshots\n");
}

void command_set(char command[], node * head){
	// dont bother reading first 4 (this is just command set)
	char * line = command + 4;
	char * token = strtok(line, " ");
	char *this_line[MAX_LINE];
	int length_of_line = 0;
	for(int i = 0; i < sizeof(this_line); i++){
		if(token == NULL){
			break;
		}
		this_line[i] = token;
		token = strtok(NULL," ");
		length_of_line++;
	}

	if(sizeof(this_line[0])>MAX_KEY){
		return;
	}

	// initialise entry struct
	struct entry this_entry;

	// set the key
	strcpy(this_entry.key,this_line[0]);
	// printf("%s\n", this_entry.key);

	// set the values
	this_entry.values = malloc(sizeof(struct element)*(length_of_line-1));
	struct element * value_ptr = this_entry.values;
	for(int i = 1; i < length_of_line; i++){
		if(isnumber(this_line[i])){
			value_ptr->value = atoi(this_line[i]);
		}//else{} for case if value is a pointer to another entry
		value_ptr++;
	}
	
	this_entry.length = length_of_line-1; // update this later to include entries

	list_add(head,this_entry);
	printf("ok\n\n");
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

int command_interpreter(char command[], node * head){

	if(strncasecmp(command,"bye",3)==0){
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
		char * line = &command[0]+4;
		command_get(line,head);
	}else if(strncasecmp(command,"del",3)==0){
		char * line = &command[0]+4;
		command_del(line,head);
	}else if(strncasecmp(command,"purge",5)==0){
		command_purge();
	}else if(strncasecmp(command,"set",3)==0){
		command_set(command,head);
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

	return 0;
}
int main(void) {

	char line[MAX_LINE];
	node * head = list_init();

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

		if(command_interpreter(line, head) == -1){
			return 0;
		}

  	}
	

	return 0;
}
