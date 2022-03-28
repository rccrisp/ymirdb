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


int cmpfunc(const void * a, const void * b){
	const element * ea = a;
	const element * eb = b;
	return (ea->value - eb->value);
}

entry * find_key(char * line, entry * ptr){
	char * key_to_find = strtok(line, " \n");
	// head is always NULL entry
	entry * iter = ptr;
	while(iter){
		if(strcmp(iter->key,key_to_find) == 0){
			return iter;
		}
		iter = iter->prev;
	}

	return NULL;

}

snapshot * find_snapshot(char * line, snapshot * head){
	int snapshot_to_find = atoi(strtok(line, " \n"));
	// head is always NULL entry
	snapshot * iter = head->next;
	while(iter){
		if(iter->id == snapshot_to_find){
			return iter;
		}
		iter = iter->next;
	}

	return NULL;
}

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

element * reverse(element * these_values, int number){
	int hold;
	for(int i = 0, j = number -1; i < j; i++, j--){
		hold = these_values[i].value;
		these_values[i].value = these_values[j].value;
		these_values[j].value = hold;
	}

	return these_values;
}

element * uniq(element * these_values, size_t * size){

	int hold = these_values[0].value;
	int new_length = 1;
	int i,j;
	for(i = 1, j = 1; i < *size; i++){
		if(hold != these_values[i].value){
			new_length++;
			hold = these_values[i].value;
			these_values[j].value = these_values[i].value;
			j++;
		}
	}

	these_values = realloc(these_values,sizeof(element)*new_length);
	*size = new_length;
	return these_values;
}

void remove_value_from_index(entry * this_entry, int index){
	element * these_values = this_entry->values;
	int size_before_remove = this_entry->length;
	int j = 0;
	for(int i = 0; i < size_before_remove; i++){
		if(i == index-1){
			i++;
		}
		if(i != size_before_remove){
			these_values[j].value = these_values[i].value;
		}
		
		j++;
	}

	these_values = realloc(these_values, sizeof(element)*(size_before_remove-1));
	this_entry->values = these_values;
	this_entry->length--;
	return ;
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

void print_values(entry this_entry){

	element * these_values = this_entry.values;
	int number = this_entry.length;
	printf("[");
	int i = 0;
	if(i<number){
		for(; i < number-1; i++){
			if(these_values[i].type == 0){
				printf("%d ", these_values[i].value);
			}else{
				printf("%s ", these_values[i].entry->key);
			}
			
		}
		if(these_values[i].type == 0){
			printf("%d", these_values[i].value);
		}else{
			printf("%s", these_values[i].entry->key);
		}
	}
	
	printf("]\n");
}

void include_entry_in_values(entry * main_entry, entry * sub_entry){
	// increase the size of the reference tracker
	main_entry->forward_size++;
	sub_entry->backward_size++;

	// include the new reference in the reference list
	main_entry->forward = realloc(main_entry->forward, sizeof(entry*)*(main_entry->forward_size));
	main_entry->forward[main_entry->forward_size-1] = sub_entry;
	sub_entry->backward = realloc(sub_entry->backward, sizeof(entry*)*(sub_entry->backward_size));
	sub_entry->backward[sub_entry->backward_size] = main_entry;

	return;

}

// given a character array of values, include these values in the correct key, from a given index
bool populate_values(entry ** ptr, entry * this_entry, char * new_values[], int index){
	bool simple = true;
	int size = this_entry->length;
	element * entry_values = malloc(sizeof(element)*size);
	entry * entry_to_include;
	int j = 1;
	for(int i = index; i < size; i++){
		if(isnumber(new_values[j])){
			entry_values[i].value = atoi(new_values[j]);
			entry_values[i].type = INTEGER;
		}else{
			entry_to_include = find_key(new_values[j],*ptr);
			// if this isnt a valid key or is a self-reference
			if(entry_to_include == NULL || entry_to_include == this_entry){
				free(entry_values);
				return false;
			}else{
				entry_values[i].entry = entry_to_include;
				entry_values[i].type = ENTRY;
				simple = false;
			}
			
		}
		j++;
	}

	// if we have gone through all the values, and all are valid, add them to this entry
	for(int i = index; i < size; i++){
		this_entry->values[i] = entry_values[i];
		if(entry_values[i].type == 1){
			include_entry_in_values(this_entry,entry_values[i].entry);
		}
		
	}

	free(entry_values);
	this_entry->is_simple = simple;
	return true;
}

bool append(entry ** ptr, char * some_values[], int num_new){
	entry * this_entry = *ptr;

	// find the size of values after including the new values
	int num_old = this_entry->length;
	int size_after_append = num_new + num_old;

	// reallocate memory
	this_entry->values = realloc(this_entry->values,sizeof(element)*size_after_append);

	this_entry->length = size_after_append;

	// populate the values with the new values
	return populate_values(ptr,this_entry,some_values,num_old);
	
}

bool push(entry ** ptr, entry * this_entry, char * push_values[], int num_new){

	// this is the size of the entry before pushing
	int num_old = this_entry->length;

	// this is the size after pushing
	int size_after_push = num_new + num_old;

	// initialise a temp element array to store the new elements
	element * new_values = malloc(sizeof(element)*size_after_push);

	// initialise a ptr to keep track of where we are
	element * value_ptr = new_values;

	// intialise a boolean to store whether this is a simple or general entry
	bool simple = true;

	// create a dummy entry value for if we need to include an entry in the values
	entry * entry_to_include;

	// loop through the proposed push values and make sure they are all valid
	int j = 1;
	for(int i = 0; i < num_new; i++){
		// if its a number, add it to the values array
		if(isnumber(push_values[j])){
			new_values[i].value = atoi(push_values[j]);
			new_values[i].type = INTEGER;
		}else{
			entry_to_include = find_key(push_values[j],*ptr);
			// if this isnt a valid key or is a self-reference
			if(entry_to_include == NULL || entry_to_include == this_entry){
				free(new_values);
				return false;
			}else{
				new_values[i].entry = entry_to_include;
				new_values[i].type = ENTRY;
				simple = false;
			}
			
		}
		j++;
		value_ptr++;
	}

	// if we have gone through all the values, and all are valid, add them to this entry
	for(int i = 0; i < num_new; i++){
		this_entry->values[i] = new_values[i];
		if(new_values[i].type == 1){
			include_entry_in_values(this_entry,new_values[i].entry);
		}		
	}


	this_entry->is_simple = simple;

	element * these_values = this_entry->values;
	for(int i = 0; i < num_old; i++){
		value_ptr->value = these_values[i].value;
		value_ptr++;
	}

	this_entry->values = realloc(this_entry->values,sizeof(element)*size_after_push);
	memcpy(this_entry->values,new_values,sizeof(element)*size_after_push);

	free(new_values);

	this_entry->length = size_after_push;

	return true;
}


// deals with the linked list references when adding a new entry
void list_add(entry ** last_entry_ptr, entry * new_entry){
	entry * last_entry = *last_entry_ptr;

	if(*last_entry_ptr!=NULL){
		last_entry->next = new_entry;
	}

	new_entry->prev = last_entry;
	new_entry->next = NULL;
	*last_entry_ptr = new_entry;
	return;
}

bool delete_references(entry * this_entry){
	if(this_entry->backward_size != 0){
		return false;
	}

	entry * forward_entry;
	for(int i = 0; i < this_entry->forward_size; i++){
		forward_entry = this_entry->forward[i];
		int num_removed = 0;
		for(int j = 0; j < forward_entry->backward_size;j++){
			if(forward_entry->backward[j] == this_entry){
				num_removed++;
			}
			if(j+num_removed<forward_entry->backward_size){
				forward_entry->backward[j] = forward_entry->backward[j+num_removed];
			}else{
				break;
			}
		}
		forward_entry->backward_size = forward_entry->backward_size - num_removed;
		forward_entry->backward = realloc(forward_entry->backward,sizeof(entry *)*(forward_entry->backward_size));
	}

	return true;
}

bool list_delete(entry ** ptr, entry * delete_entry){
	// if we are deleting the value currently pointed to by the pointer, update the pointer
	if(*ptr == delete_entry){
		*ptr = delete_entry->prev;
	}else{
		entry * next_entry = delete_entry->next;
		entry * prev_entry = delete_entry->prev;
		if(next_entry!=NULL){
			next_entry->prev = prev_entry;
		}
		prev_entry->next = next_entry;
	}

	// if we can delete the current entry without causing an invalid state
	if(delete_entry->is_simple || delete_references(delete_entry)){
		delete_entry->next = NULL;
		delete_entry->prev = NULL;
		free(delete_entry->values);
		free(delete_entry);
		return true;
	}else{
		return false;
	}

	
}

void list_free(entry * ptr){
	entry * iter = ptr;
	entry * current;
	if(iter == NULL){
		return;
	}
	while(iter){
		current = iter;
		iter = iter->next;
		free(current->values);
		free(current);
	}

	return ;
}

snapshot * snapshot_list_init(){
	snapshot * head = malloc(sizeof(snapshot));
	head->next = NULL;
	head->prev = NULL;
	head->id = 0;
	return head;
}

// add a new snapshot 
int snapshot_list_add(snapshot ** head, entry * head_entries){
  	struct snapshot * last_snapshot = *head;
    while (last_snapshot->next) {
        last_snapshot = last_snapshot->next;
    }

    static snapshot * new_snapshot;
	new_snapshot->id = last_snapshot->id + 1;
    new_snapshot->entries = head_entries;
    new_snapshot->next = NULL;
	new_snapshot->prev = last_snapshot;
	last_snapshot->next = new_snapshot;

	return new_snapshot->id;
}

void snapshot_list_delete(snapshot* head, snapshot* n){

	snapshot * prev_snapshot = head;
	while(prev_snapshot->next!=n){
		prev_snapshot = prev_snapshot->next;
	}
	prev_snapshot->next = n->next;
	n->next->prev = prev_snapshot;
	n->next = NULL;
	n->prev = NULL;
	list_free(n->entries);
}

snapshot * snapshot_list_next(snapshot * n){
	return n->next;
}

void snapshot_list_free(snapshot * head){
	snapshot * iter = head->next;
	snapshot * current;
	if(iter == NULL){
		return;
	}
	while(iter->next){
		current = iter;
		iter = iter->next;
		list_free(current->entries);
	}

	return ;
}

void command_bye() {
	printf("bye\n");
}

void command_help() {
	printf("%s\n", HELP);
}

void command_list_keys(entry ** ptr){
	entry * iter = *ptr;
	bool empty = true;
	while(iter){
		empty = false;
		printf("%s\n",iter->key);
		iter = iter->prev;
	}

	if(empty){
		printf("no keys\n");
	}
	
	printf("\n");
	return;
}

void command_list_entries(entry ** ptr){
	entry * iter = *ptr;
	bool empty = true;
	while(iter){
		empty = false;
		printf("%s ",iter->key);
		print_values(*iter);
		iter = iter->prev;
	}

	if(empty){
		printf("no entries\n");
	}

	printf("\n");
	return;
}

void command_list_snapshots(snapshot * snapshots){
	snapshot * iter = snapshots->next;
	if(iter == NULL){
		printf("no snapshots\n");
	}else{
		while(iter){
			printf("%d\n", iter->id);
			iter = iter->next;
		}
	}
	printf("\n");
	return ; 
}

void command_get(char * line, entry ** ptr){
	entry * this_entry = find_key(line,*ptr);

	if(this_entry!=NULL){
		print_values(*this_entry);
	}else{
		printf("no such key\n");
	}

	printf("\n");

	return;

}

void command_del(char * line, entry ** ptr){
	entry * this_entry = find_key(line,*ptr);

	if(this_entry!=NULL){
		if(list_delete(ptr,this_entry)){
			printf("ok\n\n");
		}else{
			printf("not permitted\n\n");
		}
		
	}else{
		printf("no such key\n\n");
	}
}

void command_purge(){
	printf("deletes entry from current state and snapshots\n");
}

void command_set(char * line, entry ** ptr){
	// find the values to push to the key
	char *this_line[MAX_LINE];
	int length_of_line = strip_values(line,this_line);


	if(sizeof(this_line[0])>MAX_KEY){
		return;
	}

	// see if this key already exists
	entry * this_entry = find_key(line,*ptr);

	bool valid;

	// if key doesnt exist, make a new key
	if(this_entry==NULL){
		// initialise entry struct
		entry * this_entry = malloc(sizeof(entry));
		

		// set the key
		strcpy(this_entry->key,this_line[0]);

		// set the values
		this_entry->values = malloc(sizeof(struct element)*(length_of_line));
		this_entry->length = length_of_line; // update this later to include entries

		valid = populate_values(ptr,this_entry,this_line,0);

		if(valid){
			// add this key to the linked list of keys
			list_add(ptr,this_entry);
			*ptr = this_entry;
		}
		
	}else{
		this_entry->values = realloc(this_entry->values,sizeof(element)*(length_of_line));
		this_entry->length = length_of_line;

		valid = populate_values(ptr,this_entry,this_line,0);
	}

	if(valid){
		printf("ok\n\n");
	}else{
		printf("not a valid entry\n\n");
	}
	
	return;


}

void command_push(char * line, entry ** ptr){
	// find the values to push to the key
	char *push_values[MAX_LINE];
	int number_of_values = strip_values(line,push_values);


	// find the key to push to from linked list
	entry * push_entry = find_key(line,*ptr);

	// push the values into the key
	if(push_entry!=NULL){
		// 
		if(push(ptr,push_entry,push_values,number_of_values)){
			printf("ok\n\n");
		}else{
			printf("not permitted\n");
		}
		
		
	}else{
		printf("no such key\n\n");
	}
}


void command_append(char * line, entry ** ptr){
	// find the values to append to the key
	char *append_values[MAX_LINE];
	int number_of_values = strip_values(line,append_values);

	// find the key to append to from linked list
	entry * append_entry = find_key(line,*ptr);
	
	bool valid;
	// append the values into the key
	if(append_entry!=NULL){
		// 
		valid = append(&append_entry,append_values,number_of_values);
		
		if(valid){
			printf("ok\n\n");
		}else{
			printf("not a valid entry");
		}
		
		
	}else{
		printf("no such key\n\n");
	}
}

void command_pick(char * line, entry ** ptr){
	// find the value to pick from this key
	char *pick_index[MAX_LINE];
	strip_values(line,pick_index);

	// find the key to pick from from the linked list
	entry * pick_entry = find_key(line,*ptr);

	// pick the value from the given index from the key
	if(pick_entry!=NULL){
		int index = atoi(pick_index[1]);
		if(pick_entry->length < index || index <= 0){
			printf("index out of range\n");
		}else{
			printf("%d\n", pick_entry->values[index-1].value);
		}
		
		
	}else{
		printf("no such key\n");
	}

	printf("\n");
	return;
}

void command_pluck(char * line, entry ** ptr){
	// find the value to pluck from this key
	char *pluck_index[MAX_LINE];
	strip_values(line,pluck_index);

	// find the key to pluck from from the linked list
	entry * pluck_entry = find_key(line,*ptr);

	// pluck the value from the given index from the key
	if(pluck_entry!=NULL){
		int index = atoi(pluck_index[1]);
		if(pluck_entry->length < index || index <= 0){
			printf("index out of range\n");
		}else{
			printf("%d\n", pluck_entry->values[index-1].value);
			remove_value_from_index(pluck_entry,index);
		}
		
		
	}else{
		printf("no such key\n");
	}

	printf("\n");
	return;
}

void command_pop(char * line, entry ** ptr){
	// find the key to pop from from the linked list
	entry * pop_entry = find_key(line,*ptr);

	// pop the value from the given index from the key
	if(pop_entry!=NULL){
		if(pop_entry->length == 0){
			printf("nil\n");
		}else{
			printf("%d\n", pop_entry->values[0].value);
			remove_value_from_index(pop_entry,1);
		}
		
		
	}else{
		printf("no such key\n");
	}

	printf("\n");
	return;
}

void command_drop(char * line, snapshot *snapshots){
	// find the snapshot to delete
	snapshot * this_snapshot = find_snapshot(line,snapshots);

	if(this_snapshot!=NULL){
		snapshot_list_delete(snapshots,this_snapshot);
		printf("ok\n");
	}else{
		printf("no such snapshot\n");
	}
	printf("\n");

	return;
}

void command_rollback(char * line, entry ** ptr, snapshot * snapshots){
	snapshot * this_snapshot = find_snapshot(line,snapshots);

	if(this_snapshot!=NULL){
		// delete the current state as we are going to replace it
		list_free(*ptr);

		// set current state to snapshot state
		entry * snapshot_entries = this_snapshot->entries->next;
		entry * this_entry;
		while(snapshot_entries){
			this_entry = malloc(sizeof(element)*(snapshot_entries->length));
			this_entry = snapshot_entries;
			list_add(ptr,this_entry);
			snapshot_entries = snapshot_entries->next;
		}

		// delete all newer snapshots
		this_snapshot = this_snapshot->next;
		snapshot * holder;
		while(this_snapshot){
			holder = this_snapshot->next;
			list_free(this_snapshot->entries);
			this_snapshot = holder;
		}
		printf("ok\n");
	}else{
		printf("no such snapshot\n");
	}
	printf("\n");

	return;
}

void command_checkout(char * line, entry ** ptr, snapshot * snapshots){
	snapshot * this_snapshot = find_snapshot(line,snapshots);

	if(this_snapshot!=NULL){
		// delete the current state as we are going to replace it
		list_free(*ptr);

		entry * snapshot_entries = this_snapshot->entries->next;
		entry * this_entry;
		while(snapshot_entries){
			this_entry = malloc(sizeof(element)*(snapshot_entries->length));
			this_entry = snapshot_entries;
			list_add(ptr,this_entry);
			snapshot_entries = snapshot_entries->next;
		}
		printf("ok\n");
	}else{
		printf("no such snapshot\n");
	}
	printf("\n");

	return;
}

void command_snapshot(entry ** ptr, snapshot * snapshots){
	entry * iter = *ptr;

	entry * entry_ptr = NULL;

	entry * current_state_entry;
	entry this_entry;
	while(iter){
		current_state_entry = iter;
		strcpy(this_entry.key,current_state_entry->key);
		this_entry.values = malloc(sizeof(element)*current_state_entry->length);
		for(int i = 0; i < current_state_entry->length; i++){
			// will have to change for complex entry
			this_entry.values[i].value = current_state_entry->values[i].value;
		}
		this_entry.length = current_state_entry->length;
		list_add(&entry_ptr,&this_entry);
		iter = iter->next;
	}

	int id = snapshot_list_add(&snapshots,entry_ptr);
	printf("saved as snapshot %d\n\n",id);
	return;
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

void command_rev(char * line, entry ** ptr){
	// find the key to sort from from the linked list
	entry * rev_entry = find_key(line,*ptr);

	if(rev_entry!=NULL){
		printf("ok\n");
		rev_entry->values = reverse(rev_entry->values, rev_entry->length);

	}else{
		printf("no such key\n");
	}

	printf("\n");
	return;
}

void command_uniq(char * line, entry ** ptr){
	// find the key to use from from the linked list
	entry * uniq_entry = find_key(line,*ptr);

	if(uniq_entry!=NULL){
		printf("ok\n");
		element * values_to_uniq = uniq_entry->values;
		uniq_entry->values = uniq(values_to_uniq, &(uniq_entry->length));
	}else{
		printf("no such key\n");
	}

	printf("\n");
	return;
}

void command_sort(char * line, entry ** ptr){
	// find the key to sort from from the linked list
	entry * sort_entry = find_key(line,*ptr);

	if(sort_entry!=NULL){
		printf("ok\n");
		element * values_to_sort = sort_entry->values;
		qsort(values_to_sort,sort_entry->length,sizeof(element),cmpfunc);
	}else{
		printf("no such key\n");
	}

	printf("\n");
	return;
	
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

int command_interpreter(char command[], entry ** ptr, snapshot * snapshots){
	char * line;
	if(strncasecmp(command,"bye",3)==0){
		list_free(*ptr);
		snapshot_list_free(snapshots);
		command_bye();
		return -1;
	}else if(strncasecmp(command,"help",4)==0){
		command_help();
	}else if(strncasecmp(command,"list keys",9)==0){
		command_list_keys(ptr);
	}else if(strncasecmp(command,"list entries",12)==0){
		command_list_entries(ptr);
	}else if(strncasecmp(command,"list snapshots",14)==0){
		command_list_snapshots(snapshots);
	}else if(strncasecmp(command,"get",3)==0){
		line = &command[0]+4;
		command_get(line,ptr);
	}else if(strncasecmp(command,"del",3)==0){
		line = &command[0]+4;
		command_del(line,ptr);
	}else if(strncasecmp(command,"purge",5)==0){
		command_purge();
	}else if(strncasecmp(command,"set",3)==0){
		line = &command[0]+4;
		command_set(line,ptr);
	}else if(strncasecmp(command,"push",4)==0){
		line = &command[0]+5;
		command_push(line,ptr);
	}else if(strncasecmp(command,"append",6)==0){
		line = &command[0]+7;
		command_append(line,ptr);
	}else if(strncasecmp(command,"pick",4)==0){
		line = &command[0]+5;
		command_pick(line,ptr);
	}else if(strncasecmp(command,"pluck",5)==0){
		line = &command[0]+6;
		command_pluck(line,ptr);
	}else if(strncasecmp(command,"pop",3)==0){
		line = &command[0]+4;
		command_pop(line,ptr);
	}else if(strncasecmp(command,"drop",4)==0){
		line = &command[0]+5;
		command_drop(line,snapshots);
	}else if(strncasecmp(command,"rollback",8)==0){
		line = &command[0]+9;
		command_rollback(line,ptr,snapshots);
	}else if(strncasecmp(command,"checkout",8)==0){
		line = &command[0]+9;
		command_checkout(line,ptr,snapshots);
	}else if(strncasecmp(command,"snapshot",8)==0){
		command_snapshot(ptr,snapshots);
	}else if(strncasecmp(command,"min",3)==0){
		command_min();
	}else if(strncasecmp(command,"max",3)==0){
		command_max();
	}else if(strncasecmp(command,"sum",3)==0){
		command_sum();
	}else if(strncasecmp(command,"len",3)==0){
		command_len();
	}else if(strncasecmp(command,"rev",3)==0){
		line = &command[0]+4;
		command_rev(line,ptr);
	}else if(strncasecmp(command,"uniq",4)==0){
		line = &command[0]+4;
		command_uniq(line,ptr);
	}else if(strncasecmp(command,"sort",4)==0){
		line = &command[0]+5;
		command_sort(line,ptr);
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
	entry * ptr = NULL;
	snapshot * snapshots = snapshot_list_init();

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
		
		if(command_interpreter(line, &ptr, snapshots) == -1){
			return 0;
		}

  	}
	

	return 0;
}