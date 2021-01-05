#include "linked_list.h"
#include "stdlib.h"

struct Node_t
{
	NodeData data;
	Node next;
	Node prev;
};

struct LinkedList_t
{
	Node head;
	int size;
};

LinkedList listCreate()
{
	LinkedList linkedList = malloc(sizeof(*linkedList));
	if (linkedList == NULL)
	{
		return NULL;
	}

	linkedList->head = NULL;
	linkedList->size = 0;

	return linkedList;
}

int listGetSize(LinkedList list)
{
	if (list == NULL)
	{
		return -1;
	}

	return list->size;
}

Node listGetFirstNode(LinkedList list)
{
	if (list == NULL)
	{
		return NULL;
	}
	
	return list->head;
}

NodeData listNodeGetData(Node node)
{
	if (node == NULL)
	{
		return NULL;
	}

	return node->data;
}

void listRemoveNode(LinkedList list, Node node)
{
	if (list == NULL || node == NULL)
	{
		return;
	}

	Node current = node;
	if (list->head == node)
	{
		list->head = node->next;
	}

	if (node->prev != NULL)
	{
		node->prev->next = node->next;
	}

	if (node->next != NULL)
	{
		node->next->prev = node->prev;
	}
	
	free(current);
	list->size--;
}

Node listCreateNewNode(NodeData data)
{
	Node node = malloc(sizeof(*node));
	if (node == NULL)
	{
		return NULL;
	}
	node->data = data;
	node->next = NULL;
	node->prev = NULL;

	return node;
}

Node listGetNextNode(Node node)
{
	if (node == NULL)
	{
		return NULL;
	}

	return node->next;
}

void listInsertStart(LinkedList list, Node node)
{
	if (list == NULL || node == NULL)
	{
		return;
	}

	if (list->head != NULL)
	{
		list->head->prev = node;
	}

	node->next = list->head;
	list->head = node;
	list->size++;
}

void listInsertAfter(LinkedList list, Node target, Node newNode)
{
	if (list == NULL || target == NULL || newNode == NULL)
	{
		return;
	}

	if (target->next != NULL)
	{
		target->next->prev = newNode;
	}

	newNode->next = target->next;
	newNode->prev = target;
	target->next = newNode;
	list->size++;
}