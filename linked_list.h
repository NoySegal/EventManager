#ifndef LINKED_LIST_H
#define LINKED_LIST_H

/** Type for defining the linked list */
typedef struct LinkedList_t* LinkedList;

/** Type for defining the linked list */
typedef struct Node_t* Node;

/** Data element data type for linked list node container */
typedef void* NodeData;

/**
* Type of function used by the linked list to compare nodes.
* This function should return:
* 		A positive integer if the first element is greater;
* 		0 if they're equal;
*		A negative integer if the second element is greater.
*/
typedef int(*CompareNodes)(Node, Node);

/**
* listCreate: Allocates a new empty linked list.
* 
* @return
*	NULL - if allocations failed.
*	A new linked list in case of success.
*/
LinkedList listCreate();

/**
* listGetSize: Gets the size of the list.
*
* @return
*	-1 - if parameter is NULL.
*	The size of the list in case of success.
*/
int listGetSize(LinkedList list);

/**
* listGetFirstNode: Gets the first node in the list.
*
* @return
*	NULL - if parameter is NULL.
*	The first node in the list in case of success.
*/
Node listGetFirstNode(LinkedList list);

/**
* listNodeGetData: Gets the data in the node.
*
* @return
*	NULL - if parameter is NULL.
*	The data in the node in case of success.
*/
NodeData listNodeGetData(Node node);

/**
* listRemoveNode: Remove the node and frees memory.
*/
void listRemoveNode(LinkedList list, Node node);

/**
* listCreateNewNode: Instantiates a new node.
*
* @return
*	NULL - if memory allocation failed.
*	The new node in case of success.
*/
Node listCreateNewNode(NodeData data);

/**
* listGetNextNode: Gets the next node in the list.
*
* @return
*	NULL - if parameter is NULL.
*	The next node in the list in case of success.
*/
Node listGetNextNode(Node node);

/**
* listInsertStart: Inserts a node in the beginning of list.
*/
void listInsertStart(LinkedList list, Node node);

/**
* listInsertAfter: Inserts a new node after a target node.
*/
void listInsertAfter(LinkedList list, Node target, Node newNode);


/*!
* Macro for iterating over a priority queue.
* Declares a new iterator for the loop.
*/
#define LIST_FOREACH(type, iterator, list) \
    for(type iterator = (type) listGetFirstNode(list) ; \
        iterator ;\
        iterator = listGetNextNode(iterator))
#endif /* LINKED_LIST_H */