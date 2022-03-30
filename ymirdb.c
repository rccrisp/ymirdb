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

// establish a global variable to track snapshot number
int snapshot_id = 1;


int cmpfunc(const void * a, const void * b){
	const element * ea = a;
	const element * eb = b;
	return (ea->value - eb->value);
}

int cmpalpha(const void * a, const void * b){
	return(strcmp(*(char**)a,*(char**)b));

	
}

void delete_references(entry * this_entry){
	entry * forward_ref;

	for(int i = 0; i < this_entry->forward_size;i++){
		forward_ref = this_entry->forward[i];
		int skip = 0;
		for(int j = 0; j < forward_ref->backward_size; j++){
			if(strcmp(forward_ref->backward[j]->key,this_entry->key)==0){
				skip++;
			}
			if(forward_ref->backward_size <= skip+j ){
				break;
			}
			forward_ref->backward[j] = forward_ref->backward[j+skip];
		}
		forward_ref->backward_size-=skip;
		forward_ref->backward = realloc(forward_ref->backward,sizeof(entry*)*(forward_ref->backward_size));
	}
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

snapshot * find_snapshot(char * line, snapshot * head){
	int snapshot_to_find = atoi(strtok(line, " \n"));
	snapshot * iter = head;
	while(iter){
		if(iter->id == snapshot_to_find){
			return iter;
		}
		iter = iter->prev;
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

void include_entry_in_values(entry ** main_entry_ptr, entry ** sub_entry_ptr){
	entry * main_entry = *main_entry_ptr;
	entry * sub_entry = *sub_entry_ptr;
	// increase the size of the reference tracker
	main_entry->forward_size++;
	sub_entry->backward_size++;
	// include the new reference in the reference list
	// main_entry->forward = realloc(main_entry->forward,sizeof(entry)*(main_entry->forward_size));
	main_entry->forward = sub_entry_ptr;
	// sub_entry->backward = realloc(sub_entry->backward,sizeof(entry)*(sub_entry->backward_size));
	sub_entry->backward = main_entry_ptr;

	return;

}

void deal_with_references(entry * main_entry, entry * sub_entry){

	// increase the size of forward references
	main_entry->forward_size++;

	// we have a new reference, so reallocate forward memory
	main_entry->forward = realloc(main_entry->forward,sizeof(entry*)*main_entry->forward_size);

	// copy the memory across
	main_entry->forward[main_entry->forward_size-1] = sub_entry;

	// increase the size of backward references
	sub_entry->backward_size++;

	// we have a new reference, so reallocate forward memory
	sub_entry->backward = realloc(sub_entry->backward,sizeof(entry*)*sub_entry->backward_size);

	// copy the memory across
	sub_entry->backward[sub_entry->backward_size-1] = main_entry;

	return; 
}

// given a character array of values, include these values in the correct key, from a given index
bool populate_values(entry ** ptr, entry * this_entry, char * new_values[], int index, bool first){
	// boolean to store if this is a simple or general entry
	bool simple = true;

	// store the size of this entry
	int size = this_entry->length;

	// loop through and ensure this is a valid entry (First entry is the key we are assigning too)
	for(int i = 1; i < size; i++){
		// if its not a number
		if(!isnumber(new_values[i])){
			// if its not a key or is a self reference
			if(find_key(new_values[i],*ptr)==NULL || this_entry == find_key(new_values[i],*ptr)){
				return false;
			}
			// have found an entry
			simple = false;
		}
	}

	// if we have gone through all the values, and all are valid, add them to this entry
	
	// assign enough memory to store all the entries
	element * these_values = malloc(sizeof(element)*size);
	
	// if we are appending values, copy the first values across
	if(index>0){
		memcpy(these_values,this_entry->values,sizeof(element)*(index));
	}

	// start populating the new values

	// assign memory for forward and backward references (will reallocate later)
	if(first){
		this_entry->backward = malloc(sizeof(entry*));
		this_entry->backward_size = 0;
	}

	this_entry->forward = malloc(sizeof(entry*));
	this_entry->forward_size = 0;

	// variable to store any entries being added to this entry
	entry * sub_entry;

	// index to track place in the new entries
	int j = 1;
	for(int i = index; i < size; i++){
		// if it is a number
		if(isnumber(new_values[j])){
			these_values[i].value = atoi(new_values[j]);
			these_values[i].type = INTEGER;
		}else{
			sub_entry = find_key(new_values[j],*ptr);
			// copy the subentry pointer to values of this entry
			these_values[i].entry = sub_entry;
			// set type to entry
			these_values[i].type = ENTRY;

			// this function updates backwards and forwards references appropriately
			deal_with_references(this_entry,these_values[i].entry);
		}
		j++;
	}

	memcpy(this_entry->values,these_values,sizeof(element)*size);

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
	return populate_values(ptr,this_entry,some_values,num_old,false);
	
}

bool push(entry ** ptr, entry * this_entry, char * push_values[], int num_new){
	// this is the size of the entry before pushing
	int num_old = this_entry->length;

	// this is the size after pushing
	int size_after_push = num_new + num_old;

	// initialise a temp element array to store the new elements
	element * old_values = malloc(sizeof(element)*size_after_push);

	// copy across the old values
	for(int i = 0; i < num_old; i++){
		old_values[i] = this_entry->values[i];
	}
	// try and add the new values to the beginning of the values
	this_entry->values = realloc(this_entry->values,sizeof(entry*)*size_after_push);
	this_entry->length = num_new;
	bool success = populate_values(ptr,this_entry,push_values,0,false);
	if(success){
		for(int i = 0; i < size_after_push; i++){
			this_entry->values[num_new+i] = old_values[i]; 
		}
		this_entry->length = size_after_push;
	}else{
		this_entry->length = num_old;
	}

	free(old_values);

	return success;
	
}

bool list_delete(entry ** ptr, entry * delete_entry){
	// if we are deleting the value currently pointed to by the pointer, update the pointer
	if(*ptr == delete_entry){
		*ptr = delete_entry->prev;
	}
	// if we can delete the current entry without causing an invalid state
	if(delete_entry->backward_size==0){

		// patch up the linkedlist after the delete
		entry * next_entry = delete_entry->next;
		entry * prev_entry = delete_entry->prev;

		// if there is a previous entry CHECK THESE!!
		if(prev_entry != NULL){
			prev_entry->next = next_entry;
		}
		if(next_entry!=NULL){
			next_entry->prev = prev_entry;
		}
		

		// set pointers to null
		delete_entry->next = NULL;
		delete_entry->prev = NULL;

		// delete references
		delete_references(delete_entry);

		// free allocated memory
		free(delete_entry->backward);
		free(delete_entry->forward);
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

// add a new snapshot 
int snapshot_list_add(snapshot ** last_snapshot_ptr, entry * head_entries){
	snapshot * new_snapshot = malloc(sizeof(snapshot));
	snapshot * last_snapshot = * last_snapshot_ptr;

	// if this is not the first snapshot
	if(*last_snapshot_ptr!=NULL){
		last_snapshot->next = new_snapshot;
	}else{
	}

	new_snapshot->id = snapshot_id;
	snapshot_id++;
	new_snapshot->prev = last_snapshot;
	new_snapshot->next = NULL;
	new_snapshot->entries = head_entries;
	*last_snapshot_ptr = new_snapshot;

	return new_snapshot->id;
}

void snapshot_list_delete(snapshot ** ptr, snapshot* delete_snapshot){
	// if we are deleting the snapshot currently pointed to by the ptr, update ptr
	if(*ptr == delete_snapshot){
		*ptr = delete_snapshot->prev;
	}

	snapshot * next_snapshot = delete_snapshot->next;
	snapshot * prev_snapshot = delete_snapshot->prev;

	if(prev_snapshot != NULL){
		prev_snapshot->next = next_snapshot;
	}
	if(next_snapshot!=NULL){
		next_snapshot->prev = prev_snapshot;
	}

	entry * iter = delete_snapshot->entries;
	entry * hold;
	while(iter){
		free(iter->forward);
		free(iter->backward);
		hold = iter;
		free(hold->values);
		iter = iter->prev;
	}
	free(delete_snapshot->entries);
	free(delete_snapshot);

}

void snapshot_list_free(snapshot ** ptr){
	snapshot * iter = *ptr;
	snapshot * current;
	if(iter == NULL){
		return;
	}
	while(iter){
		current = iter;
		iter = iter->prev;
		snapshot_list_delete(ptr,current);
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

void command_list_snapshots(snapshot ** snapshots){
	snapshot * iter = *snapshots;
	if(iter == NULL){
		printf("no snapshots\n");
	}else{
		while(iter){
			printf("%d\n", iter->id);
			iter = iter->prev;
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

void command_purge(char * line, entry ** entry_ptr, snapshot ** snapshot_ptr){
	entry * this_entry = find_key(line,*entry_ptr);
	
	// create boolean to store whether this key exists in any current or previous state
	bool is_a_current_key = false;
	bool is_a_snapshot_key = false;

	// check if key exists in current state 
	if(this_entry != NULL){
		is_a_current_key = true;
	}

	// check if key exists in any snapshots
	snapshot * iter = *snapshot_ptr;
	while(iter){
		if(NULL != find_key(line,iter->entries)){
			is_a_snapshot_key = true;
		}
		iter = iter->prev;
	}

	// if the key doesnt exist in any state
	if(!is_a_current_key&&!is_a_snapshot_key){
		printf("ok\n\n");
		return ;
	}

	// if the key exists in a snapshot
	if(is_a_snapshot_key){
		iter = *snapshot_ptr;
		entry * snapshot_entry;
		// check if snapshots are still valid after removal
		while(iter){
			// find the entry in this snapshot
			snapshot_entry = find_key(line,iter->entries);
			// if the key exists in this snapshot
			if(snapshot_entry!=NULL){
				// if the snapshot has back references
				if(snapshot_entry->backward_size!=0){
					printf("not permitted\n\n");
					return;
				}
			}
			iter = iter->prev;
		}

		// if snapshots remain valid after removal, we may delete the entry from the snapshots
		iter = *snapshot_ptr;
		while(iter){
			// find the entry in this snapshot
			snapshot_entry = find_key(line,iter->entries);
			// if the entry exists
			if(snapshot_entry!=NULL){
				list_delete(&iter->entries, snapshot_entry);
			}
			iter = iter->prev;
		}
	}
	

	// if key exists in current state
	if(is_a_current_key){
		// check if current state is valid after removal
		if(this_entry->backward_size!=0){
			printf("not permitted\n\n");
			return;
		}
		
		// if current state is valid after removal
		list_delete(entry_ptr,this_entry);
	}
	

	printf("ok\n\n");
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

		valid = populate_values(ptr,this_entry,this_line,0,true);

		if(valid){
			// add this key to the linked list of keys
			list_add(ptr,this_entry);
			*ptr = this_entry;
		}else{
			free(this_entry);
		}
		
	}else{
		delete_references(this_entry);
		this_entry->values = realloc(this_entry->values,sizeof(element)*(length_of_line));
		this_entry->length = length_of_line;

		valid = populate_values(ptr,this_entry,this_line,0,false);
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

void command_drop(char * line, snapshot ** snapshots){
	// find the snapshot to delete
	snapshot * this_snapshot = find_snapshot(line,*snapshots);

	if(this_snapshot!=NULL){
		snapshot_list_delete(snapshots,this_snapshot);
		printf("ok\n");

		
	}else{
		printf("no such snapshot\n");
	}
	printf("\n");

	return;
}

void command_rollback(char * line, entry ** ptr, snapshot ** snapshots){
	snapshot * this_snapshot = find_snapshot(line,*snapshots);

	if(this_snapshot!=NULL){
		// if w
		if(*ptr!=NULL){
			// delete the current state as we are going to replace it
			list_free(*ptr);
			*ptr = NULL;
		}

		entry * iter = this_snapshot->entries;
		entry * this_entry;
		// go through all the snapshot entries and set their values in the current state
		while(iter){
			// allocate memory for the entry
			this_entry = malloc(sizeof(entry));

			// copy the key
			strcpy(this_entry->key,iter->key);

			// allocate memory for the values
			this_entry->values = malloc(sizeof(element)*iter->length);
			for(int i = 0; i < iter->length; i++){
				this_entry->values[i] = iter->values[i];
			}

			// copy the forward references
			this_entry->forward_size = iter->forward_size;
			this_entry->forward = malloc(sizeof(entry*)*this_entry->forward_size);
			for(int i = 0; i < this_entry->forward_size; i++){
				this_entry->forward[i] = iter->forward[i];
			}

			// copy the backward references
			this_entry->backward_size = iter->backward_size;
			this_entry->backward = malloc(sizeof(entry*)*this_entry->backward_size);
			for(int i = 0; i < this_entry->backward_size; i++){
				this_entry->backward[i] = iter->backward[i];
			}

			// copy the length
			this_entry->length = iter->length;

			list_add(ptr,this_entry);
			iter = iter->prev;
		}

		

		// delete all newer snapshots
		snapshot * iter_snapshots = *snapshots;
		snapshot * holder;
		while(this_snapshot->id != iter_snapshots->id){
			holder = iter_snapshots->prev;
			snapshot_list_delete(snapshots,iter_snapshots);
			iter_snapshots = holder;

		}
		printf("ok\n");
	}else{
		printf("no such snapshot\n");
	}
	printf("\n");

	return;
}


void command_checkout(char * line, entry ** ptr, snapshot ** snapshots){
	snapshot * this_snapshot = find_snapshot(line,*snapshots);
	if(this_snapshot!=NULL){
		// delete the current state as we are going to replace it
		list_free(*ptr);
		*ptr = NULL;

		entry * iter = this_snapshot->entries;
		entry * this_entry;
		// go through all the snapshot entries and set their values in the current state
		while(iter){
			// allocate memory for the entry
			this_entry = malloc(sizeof(entry));

			// copy the key
			strcpy(this_entry->key,iter->key);

			// allocate memory for the values
			this_entry->values = malloc(sizeof(element)*iter->length);
			for(int i = 0; i < iter->length; i++){
				this_entry->values[i] = iter->values[i];
			}

			// copy the forward references
			this_entry->forward_size = iter->forward_size;
			this_entry->forward = malloc(sizeof(entry*)*this_entry->forward_size);
			for(int i = 0; i < this_entry->forward_size; i++){
				this_entry->forward[i] = iter->forward[i];
			}

			// copy the backward references
			this_entry->backward_size = iter->backward_size;
			this_entry->backward = malloc(sizeof(entry*)*this_entry->backward_size);
			for(int i = 0; i < this_entry->backward_size; i++){
				this_entry->backward[i] = iter->backward[i];
			}

			// copy the length
			this_entry->length = iter->length;

			list_add(ptr,this_entry);
			iter = iter->prev;
		}
		printf("ok\n");
	}else{
		printf("no such snapshot\n");
	}
	printf("\n");

	return;
}

void command_snapshot(entry ** ptr, snapshot ** snapshots){
	// define a pointer to iterate through all the values in the current state
	entry * iter = *ptr;

	entry * entry_ptr = NULL;

	while(iter){
		// initialise entry struct
		entry * this_entry = malloc(sizeof(entry));

		// copy the key
		strcpy(this_entry->key,iter->key);

		// copy the values
		this_entry->values = malloc(sizeof(element)*iter->length);
		for(int i = 0; i < iter->length; i++){
			this_entry->values[i] = iter->values[i];
		}

		// copy the forward references
		this_entry->forward_size = iter->forward_size;
		this_entry->forward = malloc(sizeof(entry*)*this_entry->forward_size);
		for(int i = 0; i < this_entry->forward_size; i++){
			this_entry->forward[i] = iter->forward[i];
		}

		// copy the backward references
		this_entry->backward_size = iter->backward_size;
		this_entry->backward = malloc(sizeof(entry*)*this_entry->backward_size);
		for(int i = 0; i < this_entry->backward_size; i++){
			this_entry->backward[i] = iter->backward[i];
		}

		// copy the length
		this_entry->length = iter->length;

		// add to the snapshot list
		list_add(&entry_ptr,this_entry);
		iter = iter->prev;
	}

	int id = snapshot_list_add(snapshots,entry_ptr);
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

int forward_references(entry * this_entry, char ** reference_keys, int size){

	for(int i = 0; i <this_entry->forward_size;i++){
		size = forward_references(this_entry->forward[i], reference_keys, size);
	}

	size++;
	reference_keys[size-1] = this_entry->key;

	return size;
}

int count_forward_references(entry * this_entry){
	if(this_entry->forward_size == 0){
		return 1;
	}
	int total = 1;
	for(int i = 0; i < this_entry->forward_size;i++){
		total += count_forward_references(this_entry->forward[i]);
	}

	return total;
}

void command_forward(char * line, entry ** ptr){

	// find the key to print all forward references for
	entry * this_entry = find_key(line,*ptr);

	// if this key doesnt exist
	if(this_entry == NULL){
		printf("no such key\n\n");
		return;
	}

	// if there are no forward references
	if(this_entry->forward_size == 0){
		printf("nil\n\n");
		return;
	}

	// count all the forward references to assign the correct amount of memory
	int total = count_forward_references(this_entry)-1;

	// initialise an array to store all of the reference value keys
	char ** reference_keys = malloc(sizeof(char *)*total);

	// initialise a count to track how many references we have
	int size = 0;

	// loop through all the top level forward entries
	for(int i = 0; i<this_entry->forward_size;i++){
		size = forward_references(this_entry->forward[i], reference_keys, size);
	}
	
	// sort in lexicographical order
	qsort(reference_keys,size,sizeof(char*),cmpalpha);

	// print out the references
	int i = 0;

	for(; i < size-1;i++){
		// we dont want to print duplicate values
		if(strcmp(reference_keys[i],reference_keys[i+1])!=0){
			printf("%s, ", reference_keys[i]);
		}
		
	}
	printf("%s\n\n", reference_keys[i]);

	free(reference_keys);
	return;
	
}

int backward_references(entry * this_entry, char ** reference_keys, int size){

	for(int i = 0; i <this_entry->backward_size;i++){
		size = backward_references(this_entry->backward[i], reference_keys, size);
	}

	size++;
	reference_keys[size-1] = this_entry->key;

	return size;
}

int count_backward_references(entry * this_entry){
	if(this_entry->backward_size == 0){
		return 1;
	}
	int total = 1;
	for(int i = 0; i < this_entry->backward_size;i++){
		total += count_backward_references(this_entry->backward[i]);
	}

	return total;
}

void command_backward(char * line, entry ** ptr){
		// find the key to print all backward references for
	entry * this_entry = find_key(line,*ptr);

	// if this key doesnt exist
	if(this_entry == NULL){
		printf("no such key\n\n");
		return;
	}

	// if there are no backward references
	if(this_entry->backward_size == 0){
		printf("nil\n\n");
		return;
	}

	// count all the backward references to assign the correct amount of memory
	int total = count_backward_references(this_entry)-1;

	// initialise an array to store all of the reference value keys
	char ** reference_keys = malloc(sizeof(char *)*total);

	// initialise a count to track how many references we have
	int size = 0;

	// loop through all the top level backward entries
	for(int i = 0; i<this_entry->backward_size;i++){
		size = backward_references(this_entry->backward[i], reference_keys, size);
	}
	
	// sort in lexicographical order
	qsort(reference_keys,size,sizeof(char*),cmpalpha);

	// print out the references
	int i = 0;

	for(; i < size-1;i++){
		// we dont want to print duplicate values
		if(strcmp(reference_keys[i],reference_keys[i+1])!=0){
			printf("%s, ", reference_keys[i]);
		}
		
	}
	printf("%s\n\n", reference_keys[i]);

	free(reference_keys);
}

void command_type(char * line, entry ** ptr){
	// find the key to sort from from the linked list
	entry * type_entry = find_key(line,*ptr);

	if(type_entry!=NULL){
		if(type_entry->is_simple){
			printf("simple");
		}else{
			printf("general");
		}
	}else{
		printf("no such key");
	}

	printf("\n\n");
}

void command_size(char * line, entry ** ptr){
		// find the key to sort from from the linked list
	entry * this_entry = find_key(line,*ptr);

	if(this_entry!=NULL){
		printf("backward size %ld\nforward size %ld\n\n",this_entry->backward_size,this_entry->forward_size);
	}else{
		printf("no such key");
	}

	printf("\n\n");
}

int command_interpreter(char command[], entry ** entry_ptr, snapshot ** snapshot_ptr){
	char * line;
	if(strncasecmp(command,"bye",3)==0){
		list_free(*entry_ptr);
		snapshot_list_free(snapshot_ptr);
		command_bye();
		return -1;
	}else if(strncasecmp(command,"help",4)==0){
		command_help();
	}else if(strncasecmp(command,"list keys",9)==0){
		command_list_keys(entry_ptr);
	}else if(strncasecmp(command,"list entries",12)==0){
		command_list_entries(entry_ptr);
	}else if(strncasecmp(command,"list snapshots",14)==0){
		command_list_snapshots(snapshot_ptr);
	}else if(strncasecmp(command,"get",3)==0){
		line = &command[0]+4;
		command_get(line,entry_ptr);
	}else if(strncasecmp(command,"del",3)==0){
		line = &command[0]+4;
		command_del(line,entry_ptr);
	}else if(strncasecmp(command,"purge",5)==0){
		line = &command[0]+6;
		command_purge(line,entry_ptr,snapshot_ptr);
	}else if(strncasecmp(command,"set",3)==0){
		line = &command[0]+4;
		command_set(line,entry_ptr);
	}else if(strncasecmp(command,"push",4)==0){
		line = &command[0]+5;
		command_push(line,entry_ptr);
	}else if(strncasecmp(command,"append",6)==0){
		line = &command[0]+7;
		command_append(line,entry_ptr);
	}else if(strncasecmp(command,"pick",4)==0){
		line = &command[0]+5;
		command_pick(line,entry_ptr);
	}else if(strncasecmp(command,"pluck",5)==0){
		line = &command[0]+6;
		command_pluck(line,entry_ptr);
	}else if(strncasecmp(command,"pop",3)==0){
		line = &command[0]+4;
		command_pop(line,entry_ptr);
	}else if(strncasecmp(command,"drop",4)==0){
		line = &command[0]+5;
		command_drop(line,snapshot_ptr);
	}else if(strncasecmp(command,"rollback",8)==0){
		line = &command[0]+9;
		command_rollback(line,entry_ptr,snapshot_ptr);
	}else if(strncasecmp(command,"checkout",8)==0){
		line = &command[0]+9;
		command_checkout(line,entry_ptr,snapshot_ptr);
	}else if(strncasecmp(command,"snapshot",8)==0){
		command_snapshot(entry_ptr,snapshot_ptr);
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
		command_rev(line,entry_ptr);
	}else if(strncasecmp(command,"uniq",4)==0){
		line = &command[0]+4;
		command_uniq(line,entry_ptr);
	}else if(strncasecmp(command,"sort",4)==0){
		line = &command[0]+5;
		command_sort(line,entry_ptr);
	}else if(strncasecmp(command,"forward",7)==0){
		line = &command[0]+8;
		command_forward(line,entry_ptr);
	}else if(strncasecmp(command,"backward",8)==0){
		line = &command[0]+9;
		command_backward(line,entry_ptr);
	}else if(strncasecmp(command,"type",4)==0){
		line = &command[0]+5;
		command_type(line,entry_ptr);
	}else if(strncasecmp(command,"size",4)==0){ 
		line = &command[0]+5;
		command_size(line,entry_ptr);
	}else{
		printf("INVALID COMMAND: TYPE HELP FOR A LIST OF VALID COMMANDS\n\n");
	}

	return 0;
}

int main(void) {

	char line[MAX_LINE];
	entry * ptr = NULL;
	snapshot * snapshots = NULL;

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
		
		if(command_interpreter(line, &ptr, &snapshots) == -1){
			return 0;
		}

  	}
	

	return 0;
}