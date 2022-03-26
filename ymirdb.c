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
		if(i==0 && s[i] == -1){
			continue;
		}else if(isdigit(s[i]) == 0 && s[i]!= '\n'){
			return false;
		}
              
    }
    return true;
}

element * remove_value_from_index(element * these_values, int index, int size_before_remove){
	int j = 0;
	for(int i = 0; i < size_before_remove; i++){
		these_values[j].value = these_values[i].value;
		if(i == index-2){
			i++;
		}
		j++;
	}

	these_values = realloc(these_values, sizeof(element)*(size_before_remove-1));
	return these_values;
}

int strip_values(char * line, char * strip_values[]){
	char * token = strtok(line, " \n");
	int number_of_values = -1; // because first value is the key
	for(int i = 0; i < MAX_LINE; i++){
		if(token == NULL){
			break;
		}
		strip_values[i] = token;
		token = strtok(NULL," ");
		number_of_values++;
	}
	return number_of_values;
}

void print_values(element * print_values, int number){
	printf("[");
	int i = 0;
	for(; i < number-1; i++){
		printf("%d ", print_values[i].value);
	}
	printf("%d", print_values[i].value);
	printf("]\n");
}

// given a character array of values, include these values in the correct key, from a given index
element * populate_values(element * entry_values, char * new_values[], int index, int size){
	int j = 1;
	for(int i = index; i < size; i++){
		if(isnumber(new_values[j])){
			entry_values[i].value = atoi(new_values[j]);
		}//else{} if it is an entry
		j++;
	}
	return entry_values;
}

element * append(element * old_values, int num_old, char * some_values[], int num_new){
	// find the size of values after including the new values
	int size_after_append = num_new + num_old;

	// reallocate memory
	old_values = realloc(old_values,sizeof(element)*size_after_append);

	// populate the values with the new values
	old_values = populate_values(old_values,some_values,num_old,size_after_append);

	return old_values;
}

element * push(element * old_values, int num_old, char * some_values[], int num_new){
	int size_after_push = num_new + num_old;
	element * new_values = malloc(sizeof(element)*size_after_push);
	element * ptr = new_values;
	for(int i = 0; i < num_new; i++){
		if(isnumber(some_values[num_new-i])){
			ptr->value = atoi(some_values[num_new-i]);
		}//else{} if it is an entry
		
		ptr++;
	}

	for(int i = 0; i < num_old; i++){
		ptr->value = old_values[i].value;
		ptr++;
	}

	old_values = realloc(old_values,sizeof(element)*size_after_push);
	for(int i = 0; i < size_after_push; i++){
		old_values[i].value = new_values[i].value;
	}

	free(new_values);

	return old_values;
}

node * find_key(char * line, node * head){
	char * key_to_find = strtok(line, " \n");
	// head is always NULL entry
	node * iter = head->next;
	entry this_entry;
	while(iter){
		this_entry = iter->item;
		if(strcmp(this_entry.key,key_to_find) == 0){
			return iter;
		}
		iter = iter->next;
	}

	return NULL;

}

node * list_init(){
	node * head = malloc(sizeof(node));
	head->next = NULL;
	return head;
}

// push a node to the start of the linked list
void list_add(node * head, entry this_entry){
	node * new_node = malloc(sizeof(node));
	new_node->item = this_entry;
	new_node->next = head->next;
	head->next = new_node;

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
		free(iter->item.values);
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

void command_list_keys(node * head){
	node * iter = head->next;
	entry this_entry;
	bool empty = true;
	while(iter){
		empty = false;
		this_entry = iter->item;
		printf("%s\n",this_entry.key);
		iter = iter->next;
	}

	if(empty){
		printf("no keys\n");
	}
	
	printf("\n");
	return;
}

void command_list_entries(node * head){
	node * iter = head->next;
	entry this_entry;
	bool empty = true;
	while(iter){
		empty = false;
		this_entry = iter->item;
		printf("%s ",this_entry.key);
		print_values(this_entry.values, this_entry.length);
		iter = iter->next;
	}

	if(empty){
		printf("no entries\n");
	}

	printf("\n");
	return;
}

void command_list_snapshots(){
	printf("displays all snapshots in the database\n");
}

void command_get(char * line, node * head){
	node * this = find_key(line,head);

	if(this!=NULL){
		entry this_entry = this->item;
		print_values(this_entry.values, this_entry.length);
	}else{
		printf("no such key\n");
	}

	printf("\n");

	return;

}

void command_del(char * line, node * head){
	node * this = find_key(line,head);

	if(this!=NULL){
		entry this_entry = this->item;
		list_delete(head, this);
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

	// find the values to push to the key
	char *this_line[MAX_LINE];
	int length_of_line = strip_values(line,this_line);


	if(sizeof(this_line[0])>MAX_KEY){
		return;
	}

	// see if this key already exists
	node * key_node = find_key(line,head);

	// if key doesnt exist, make a new key
	if(key_node==NULL){
		// initialise entry struct
		struct entry this_entry;

		// set the key
		strcpy(this_entry.key,this_line[0]);

		// set the values
		this_entry.values = malloc(sizeof(struct element)*(length_of_line));
		this_entry.length = length_of_line; // update this later to include entries

		this_entry.values = populate_values(this_entry.values,this_line,0,this_entry.length);
		
		// add this key to the linked list of keys
		list_add(head,this_entry);
	}else{
		struct entry * this_entry = &(key_node->item);
		this_entry->values = realloc(this_entry->values,sizeof(element)*(length_of_line));
		this_entry->length = length_of_line;

		this_entry->values = populate_values(this_entry->values,this_line,0,this_entry->length);
	}

		
	printf("ok\n\n");
	return;


}

void command_push(char * line, node * head){
	// find the values to push to the key
	char *push_values[MAX_LINE];
	int number_of_values = strip_values(line,push_values);


	// find the key to push to from linked list
	node * push_node = find_key(line,head);

	// push the values into the key
	if(push_node!=NULL){
		// 
		entry * entry_to_push_to = &(push_node->item);
		element ** values_to_push_to = &(entry_to_push_to->values);
		int number_of_old_values = entry_to_push_to->length;
		entry_to_push_to->values = push(*values_to_push_to,number_of_old_values,push_values,number_of_values);
		entry_to_push_to->length = number_of_values + number_of_old_values;
		
		printf("ok\n\n");
		
	}else{
		printf("no such key\n\n");
	}
}


void command_append(char * line, node * head){
	// find the values to append to the key
	char *append_values[MAX_LINE];
	int number_of_values = strip_values(line,append_values);

	// find the key to append to from linked list
	node * append_node = find_key(line,head);

	// append the values into the key
	if(append_node!=NULL){
		// 
		entry * entry_to_append_to = &(append_node->item);
		element ** values_to_append_to = &(entry_to_append_to->values);
		int number_of_old_values = entry_to_append_to->length;
		entry_to_append_to->values = append(*values_to_append_to,number_of_old_values,append_values,number_of_values);
		entry_to_append_to->length = number_of_values + number_of_old_values;
		
		printf("ok\n\n");
		
	}else{
		printf("no such key\n\n");
	}
}

void command_pick(char * line, node * head){
	// find the value to pick from this key
	char *pick_index[MAX_LINE];
	strip_values(line,pick_index);

	// find the key to pick from from the linked list
	node * pick_node = find_key(line,head);

	// pick the value from the given index from the key
	if(pick_node!=NULL){
		entry this_entry = pick_node->item;
		int index = atoi(pick_index[1]);
		if(this_entry.length < index || index <= 0){
			printf("index out of range\n");
		}else{
			printf("%d\n", this_entry.values[index-1].value);
		}
		
		
	}else{
		printf("no such key\n");
	}

	printf("\n");
	return;
}

void command_pluck(char * line, node * head){
	// find the value to pluck from this key
	char *pluck_index[MAX_LINE];
	strip_values(line,pluck_index);

	// find the key to pluck from from the linked list
	node * pluck_node = find_key(line,head);

	// pluck the value from the given index from the key
	if(pluck_node!=NULL){
		entry * this_entry = &(pluck_node->item);
		int index = atoi(pluck_index[1]);
		if(this_entry->length < index || index <= 0){
			printf("index out of range\n");
		}else{
			printf("%d\n", this_entry->values[index-1].value);
			this_entry->values = remove_value_from_index(this_entry->values,index,this_entry->length);
			this_entry->length--;
		}
		
		
	}else{
		printf("no such key\n");
	}

	printf("\n");
	return;
}

void command_pop(char * line, node * head){
	// find the key to pop from from the linked list
	node * pop_node = find_key(line,head);

	// pop the value from the given index from the key
	if(pop_node!=NULL){
		entry * this_entry = &(pop_node->item);
		if(this_entry->length == 0){
			printf("nil\n");
		}else{
			printf("%d\n", this_entry->values[0].value);
			this_entry->values = remove_value_from_index(this_entry->values,1,this_entry->length);
			this_entry->length--;
		}
		
		
	}else{
		printf("no such key\n");
	}

	printf("\n");
	return;
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
	char * line;
	if(strncasecmp(command,"bye",3)==0){
		list_free(head);
		command_bye();
		return -1;
	}else if(strncasecmp(command,"help",4)==0){
		command_help();
	}else if(strncasecmp(command,"list keys",9)==0){
		command_list_keys(head);
	}else if(strncasecmp(command,"list entries",12)==0){
		command_list_entries(head);
	}else if(strncasecmp(command,"list snapshots",14)==0){
		command_list_snapshots();
	}else if(strncasecmp(command,"get",3)==0){
		line = &command[0]+4;
		command_get(line,head);
	}else if(strncasecmp(command,"del",3)==0){
		line = &command[0]+4;
		command_del(line,head);
	}else if(strncasecmp(command,"purge",5)==0){
		command_purge();
	}else if(strncasecmp(command,"set",3)==0){
		command_set(command,head);
	}else if(strncasecmp(command,"push",4)==0){
		line = &command[0]+5;
		command_push(line,head);
	}else if(strncasecmp(command,"append",6)==0){
		line = &command[0]+7;
		command_append(line,head);
	}else if(strncasecmp(command,"pick",4)==0){
		line = &command[0]+5;
		command_pick(line,head);
	}else if(strncasecmp(command,"pluck",5)==0){
		line = &command[0]+6;
		command_pluck(line,head);
	}else if(strncasecmp(command,"pop",3)==0){
		line = &command[0]+4;
		command_pop(line,head);
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
