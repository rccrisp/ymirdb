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

element * remove_value_from_index(element * these_values, int index, int size_before_remove){
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
	if(i<number){
		for(; i < number-1; i++){
			printf("%d ", print_values[i].value);
		}
		printf("%d", print_values[i].value);
	}
	
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
	free(n->item.values);
	free(n);
	
}

node * list_next(node * n){
	return n->next;
}

void list_free(node * head){
	node * iter = head->next;
	node * current;
	while(iter->next){
		current = iter;
		iter = iter->next;
		list_delete(head,current);
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
int snapshot_list_add(snapshot * head, node * head_entries){
  	struct snapshot * last_snapshot = head;
    while (last_snapshot->next) {
        last_snapshot = last_snapshot->next;
    }

    struct snapshot * new_snapshot = malloc(sizeof(struct snapshot));
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
	free(n);
}

snapshot * snapshot_list_next(snapshot * n){
	return n->next;
}

void snapshot_list_free(snapshot * head){
	snapshot * iter = head->next;
	snapshot * current;
	while(iter->next){
		current = iter;
		iter = iter->next;
		snapshot_list_delete(head,current);
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
		list_delete(head, this);
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

void command_rollback(char * line, node * head, snapshot * snapshots){
	snapshot * this_snapshot = find_snapshot(line,snapshots);

	if(this_snapshot!=NULL){
		// delete the current state as we are going to replace it
		list_free(head);

		// set current state to snapshot state
		node * snapshot_entries = this_snapshot->entries->next;
		entry * this_entry;
		while(snapshot_entries){
			this_entry = malloc(sizeof(element)*(snapshot_entries->item.length));
			*this_entry = snapshot_entries->item;
			list_add(head,*this_entry);
			snapshot_entries = snapshot_entries->next;
		}

		// delete all newer snapshots
		this_snapshot = this_snapshot->next;
		snapshot * holder;
		while(this_snapshot){
			holder = this_snapshot->next;
			list_free(this_snapshot->entries);
			free(this_snapshot);
			this_snapshot = holder;
		}
		printf("ok\n");
	}else{
		printf("no such snapshot\n");
	}
	printf("\n");

	return;
}

void command_checkout(char * line, node * head, snapshot * snapshots){
	snapshot * this_snapshot = find_snapshot(line,snapshots);

	if(this_snapshot!=NULL){
		// delete the current state as we are going to replace it
		list_free(head);

		node * snapshot_entries = this_snapshot->entries->next;
		entry * this_entry;
		while(snapshot_entries){
			this_entry = malloc(sizeof(element)*(snapshot_entries->item.length));
			*this_entry = snapshot_entries->item;
			list_add(head,*this_entry);
			snapshot_entries = snapshot_entries->next;
		}
		printf("ok\n");
	}else{
		printf("no such snapshot\n");
	}
	printf("\n");

	return;
}

void command_snapshot(node * head, snapshot * snapshots){
	node * iter = head->next;

	node * head_snapshots = list_init();
	entry * current_state_entry;
	entry this_entry;
	while(iter){
		current_state_entry = &(iter->item);
		strcpy(this_entry.key,current_state_entry->key);
		this_entry.values = malloc(sizeof(element)*current_state_entry->length);
		for(int i = 0; i < current_state_entry->length; i++){
			// will have to change for complex entry
			this_entry.values[i].value = current_state_entry->values[i].value;
		}
		this_entry.length = current_state_entry->length;
		list_add(head_snapshots,this_entry);
		iter = iter->next;
	}

	int id = snapshot_list_add(snapshots,head_snapshots);
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

void command_rev(char * line, node * head){
	// find the key to sort from from the linked list
	node * rev_node = find_key(line,head);

	if(rev_node!=NULL){
		printf("ok\n");
		entry entry_to_rev = rev_node->item;
		element * values_to_rev = entry_to_rev.values;
		values_to_rev = reverse(values_to_rev, entry_to_rev.length);

	}else{
		printf("no such key\n");
	}

	printf("\n");
	return;
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

void command_uniq(char * line, node * head){
	// find the key to use from from the linked list
	node * uniq_node = find_key(line,head);

	if(uniq_node!=NULL){
		printf("ok\n");
		entry * entry_to_uniq = &(uniq_node->item);
		element * values_to_uniq = entry_to_uniq->values;
		 entry_to_uniq->values = uniq(values_to_uniq, &(entry_to_uniq->length));
	}else{
		printf("no such key\n");
	}

	printf("\n");
	return;
}

void command_sort(char * line, node * head){
	// find the key to sort from from the linked list
	node * sort_node = find_key(line,head);

	if(sort_node!=NULL){
		printf("ok\n");
		entry entry_to_sort = sort_node->item;
		element * values_to_sort = entry_to_sort.values;
		qsort(values_to_sort,entry_to_sort.length,sizeof(element),cmpfunc);
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

int command_interpreter(char command[], node * head, snapshot * snapshots){
	char * line;
	if(strncasecmp(command,"bye",3)==0){
		list_free(head);
		snapshot_list_free(snapshots);
		free(head);
		free(snapshots);
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
		line = &command[0]+9;
		command_rollback(line,head,snapshots);
	}else if(strncasecmp(command,"checkout",8)==0){
		line = &command[0]+9;
		command_checkout(line,head,snapshots);
	}else if(strncasecmp(command,"snapshot",8)==0){
		command_snapshot(head,snapshots);
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
		command_rev(line,head);
	}else if(strncasecmp(command,"uniq",4)==0){
		line = &command[0]+4;
		command_uniq(line,head);
	}else if(strncasecmp(command,"sort",4)==0){
		line = &command[0]+5;
		command_sort(line,head);
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

		if(command_interpreter(line, head, snapshots) == -1){
			return 0;
		}

  	}
	

	return 0;
}
