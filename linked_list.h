#ifndef LINKED_LIST_H
#define LINKED_LIST_H


/* linked list node definition */

typedef struct struct_of_ints_struct {
	double value ;
	double time ;
	struct struct_of_ints_struct *prev ;  /* pointer to previous node in list */
	struct struct_of_ints_struct *next ;  /* pointer to next node in list */
} struct_of_ints ;


/* linked list helper function declarations */
struct_of_ints* add_to_top    (struct_of_ints* head_node, double value, double time) ;
// struct_of_ints*	delete_node   (struct_of_ints* head_node, double value, time_t time) ;
struct_of_ints* delete_list   (struct_of_ints* head_node ) ;

#endif
