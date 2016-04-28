#include <stdio.h>
#include <stdlib.h>
#include "linked_list.h"

/* definition of linked list helper functions */


/*
 * this function adds a node to the beginning of the linked list
 * -returns head_node - or NULL if error
 */
struct_of_ints* add_to_top (struct_of_ints* head_node, double value, double time ) {

	/* allocate memory for the new node */
	struct_of_ints* new_node = NULL ;
	new_node = malloc (sizeof(struct_of_ints)) ;

	/* check if malloc worked, otherwise, populate new node */
	if (new_node == NULL) return NULL ;
	new_node->value = value ;
	new_node->time = time ;
	new_node->prev  = NULL  ;
	new_node->next  = head_node ;

	/* update former head pointer's links */
	if (head_node) head_node->prev = new_node ;

	/* return new head node */
	return new_node ;
}

// /*
//  * this function adds a node to the end of the linked list 
//  * -returns head_node - or NULL if error
//  */
// struct_of_ints* add_to_end (struct_of_ints* head_node, double value ) {
// 	// use another pointer to cycle through so head_node is not affected
// 	struct_of_ints* current_node = NULL;

// 	/* allocate memory for the new node */
// 	struct_of_ints* new_node = malloc (sizeof(struct_of_ints)) ;

// 	/* check if malloc worked, otherwise, populate new node */
// 	if (new_node == NULL) return NULL ;
// 	new_node->value = value ;
// 	new_node->next  = NULL  ;
// 	new_node->prev  = NULL ;

// 	// if NULL list, this is the head node
// 	if(head_node == NULL) {
// 		head_node = new_node;
// 		return head_node;
// 	}

// 	current_node = head_node;
// 	// if not the head, cycle through till end
// 	while(current_node->next) {
// 		current_node = current_node->next;

// 	}
// 	// update links
// 	current_node->next = new_node;
// 	new_node->prev = current_node;
// 	return head_node;
// }

/*
 * deletes the first instance of the "value" from the list
 * -returns head_node - or NULL if node isn't in list
 */
// struct_of_ints*	delete_node (struct_of_ints* head_node, double value ) {
// 	// use another pointer to cycle through so head_node is not affected
// 	struct_of_ints* current_node = head_node;

// 	// if empty cannot delete
// 	if (head_node == NULL) return NULL;

// 	// if delete head
// 	if (head_node->value == value) {
// 		// next node is new head
// 		head_node = head_node->next;
// 		// free!
// 		free(current_node);
// 		return head_node;
// 	}

// 	// else cycle until found which node to delete
// 	while (current_node) {
// 		if (current_node->value == value) {
// 			// if found, update the links around the node
// 			current_node->prev->next = current_node->next;
// 			if (current_node->next) {
// 				// if not end, set prev link
// 				current_node->next->prev = current_node->prev;
// 			}
// 			// free!
// 			free(current_node);
// 			return head_node;
// 		}
// 		current_node = current_node->next;
// 	}
// 	// if not found, return NULL
// 	return NULL;
// }

// /*
//  * searches list for first instance of the "value" passed in
//  * -returns node if found - or a NULL if node isn't in list
//  */
// struct_of_ints* search_list (struct_of_ints* head_node, int value ) {
// 	// use another pointer to cycle through
// 	struct_of_ints* current_node = head_node;
// 	// cycle until found
// 	while (current_node) {
// 		if (current_node->value == value) {
// 			return current_node ;
// 		}
// 		current_node = current_node->next;
// 	}
// 	// if not found, return NULL
// 	return NULL ;
// }


// /*
//  * sorts linked list in acending order: 1, 2, 3...
//  * -returns new head of linked list after sort, or NULL if list is empty
//  */
// struct_of_ints* sort_list (struct_of_ints* head_node ) {
// 	// logic: put smallest value in front, then sort rest of list
// 	// return NULL if list is empty
// 	if(head_node == NULL) {
// 		return head_node;
// 	}

// 	// use another pointer to cycle through so head_node is not affected
// 	struct_of_ints* current_node = head_node->next;
// 	// temp holds value to be switched
// 	int temp = 0;
// 	while (current_node) {
// 		// if this value is smaller than 1st value then switch the two
// 		if (current_node->value < head_node->value) {
// 			temp = head_node->value;
// 			head_node->value = current_node->value;
// 			current_node->value = temp;
// 		}
// 		current_node = current_node->next;
// 	}
// 	// recursion to find next smallest value
// 	sort_list (head_node->next);
// 	return head_node ;
// }


// /*
//  * sorts linked list in decending order: 3, 2, 1...
//  * -returns new head of linked list after sort, or NULL if list is empty
//  */
// struct_of_ints* sort_list_rev (struct_of_ints* head_node ) {
// 	// put largest sample in front, then sort rest of list
// 	// return NULL if list is empty
// 	if(head_node == NULL) {
// 		return head_node;
// 	}
// 	// use another pointer to cycle through so head_node is not affected
// 	struct_of_ints* current_node = head_node->next;
// 	// temp holds value to be switched
// 	int temp = 0;
// 	while (current_node) {
// 		// if this value is larger than 1st value then switch the two
// 		if (current_node->value > head_node->value) {
// 			temp = head_node->value;
// 			head_node->value = current_node->value;
// 			current_node->value = temp;
// 		}
// 		current_node = current_node->next;
// 	}
// 	// recursion to find next largest value
// 	sort_list_rev (head_node->next);
// 	return head_node ;
// }


// /*
//  * prints entire linked list to look like an array
//  */
// void print_list (struct_of_ints* head_node ) {

// 	// i labels which node is printed
// 	int i = 0 ;
// 	// use another pointer to cycle through so head_node is not affected
// 	struct_of_ints* current_node = head_node;
// 	// cycles through and prints entire list while current node is not NULL
// 	while (current_node) {
// 		printf ("\tlist[%d]=%d \n", i, current_node->value ) ;
// 		current_node = current_node->next;
// 		i++;
// 	}

// }


/*
 * delete every node in the linked list 
 * returns NULL if successful, otherwise head node is returned
 */
struct_of_ints* delete_list (struct_of_ints* head_node ) {
	// temp holds node to be freed
   	struct_of_ints* temp;
   	// loop while not not at end
   	while (head_node) {
    	temp = head_node;
    	// update head_node as the previous one will be deleted
    	head_node = head_node->next;
    	// free!
    	free(temp);
	}
	// once at end, return NULL head_node
	return head_node;

}
