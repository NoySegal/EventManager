#include "priority_queue.h"
#include "stdio.h"
#include "stdlib.h"
#include "linked_list.h"

struct PriorityQueue_t
{
	LinkedList combinedElementList;
	Node iterator;
	CopyPQElement copyElement;
	CopyPQElementPriority copyElementPriority;
	FreePQElement freeElement;
	FreePQElementPriority freeElementPriority;
	EqualPQElements equalElements;
	ComparePQElementPriorities comparePriorities;
};

typedef struct CombinedElement_t
{
	PQElement element;
	PQElementPriority priority;
} *CombinedElement;

static int compareNodesByPriority(PriorityQueue queue, Node n1, Node n2) {
	PQElementPriority p1 = ((CombinedElement)listNodeGetData(n1))->priority;
	PQElementPriority p2 = ((CombinedElement)listNodeGetData(n2))->priority;
	return queue->comparePriorities(p1, p2);
}

static void destroyCombinedElement(PriorityQueue queue, CombinedElement combinedElement)
{
	queue->freeElement(combinedElement->element);
	queue->freeElementPriority(combinedElement->priority);
	free(combinedElement);
}

static CombinedElement createCombinedElement(PriorityQueue queue, PQElement element, PQElementPriority priority)
{
	CombinedElement combinedElement = malloc(sizeof(*combinedElement));
	if (combinedElement == NULL)
	{
		return NULL;
	}

	combinedElement->element = queue->copyElement(element);
	combinedElement->priority = queue->copyElementPriority(priority);
	if (combinedElement->element == NULL || combinedElement->priority == NULL)
	{
		destroyCombinedElement(queue, combinedElement);
		return NULL;
	}
	return combinedElement;
}

static PriorityQueueResult pqRemoveByNode(PriorityQueue queue, Node node)
{
	if (queue == NULL || node == NULL)
	{
		return PQ_NULL_ARGUMENT;
	}

	destroyCombinedElement(queue, listNodeGetData(node));
	listRemoveNode(queue->combinedElementList, node);

	return PQ_SUCCESS;
}

static Node getLastBiggerNode(PriorityQueue queue, Node node)
{
	if (queue == NULL || node == NULL) {
		return NULL;
	}

	Node currentNode = listGetFirstNode(queue->combinedElementList);
	Node previousNode = NULL;

	while (currentNode != NULL && compareNodesByPriority(queue, currentNode, node) >= 0)
	{
		previousNode = currentNode;
		currentNode = listGetNextNode(currentNode);
	}

	return previousNode;
}

static Node getFirstEqualNodeByElement(PriorityQueue queue, PQElement element)
{
	if (queue == NULL || element == NULL) {
		return NULL;
	}

	LIST_FOREACH(Node, node, queue->combinedElementList) {
		if (queue->equalElements(((CombinedElement)listNodeGetData(node))->element, element))
		{
			return node;
		}
	}

	return NULL;
}

static Node getFirstIdenticalNodeByElementAndPriority(PriorityQueue queue, PQElement element, PQElementPriority priority)
{
	if (queue == NULL || element == NULL || priority == NULL) {
		return NULL;
	}

	LIST_FOREACH(Node, node, queue->combinedElementList) {
		if (queue->equalElements(((CombinedElement)listNodeGetData(node))->element, element) 
			&& queue->comparePriorities(((CombinedElement)listNodeGetData(node))->priority, priority) == 0)
		{
			return node;
		}
	}

	return NULL;
}

PriorityQueue pqCreate(CopyPQElement copy_element,
	FreePQElement free_element,
	EqualPQElements equal_elements,
	CopyPQElementPriority copy_priority,
	FreePQElementPriority free_priority,
	ComparePQElementPriorities compare_priorities)
{
	if (!copy_element || !free_element || !equal_elements || !copy_priority || !free_priority || !compare_priorities)
	{
		return NULL;
	}

	PriorityQueue queue = malloc(sizeof(*queue));
	LinkedList linkedList = listCreate();
	if (queue == NULL || linkedList == NULL)
	{
		free(queue);
		free(linkedList);
		return NULL;
	}

	queue->combinedElementList = linkedList;
	queue->iterator = NULL;
	queue->copyElement = copy_element;
	queue->copyElementPriority = copy_priority;
	queue->freeElement = free_element;
	queue->freeElementPriority = free_priority;
	queue->equalElements = equal_elements;
	queue->comparePriorities = compare_priorities;

	return queue;
}

void pqDestroy(PriorityQueue queue)
{
	if (queue == NULL)
	{
		return;
	}

	pqClear(queue);
	free(queue->combinedElementList);
	free(queue);
}

PriorityQueueResult pqClear(PriorityQueue queue)
{
	if (queue == NULL)
	{
		return PQ_NULL_ARGUMENT;
	}

	while (pqGetSize(queue) > 0)
	{
		pqRemove(queue);
	}

	return PQ_SUCCESS;
}

int pqGetSize(PriorityQueue queue)
{
	if (queue == NULL)
	{
		return -1;
	}

	return listGetSize(queue->combinedElementList);
}

PriorityQueueResult pqRemove(PriorityQueue queue)
{
	if (queue == NULL)
	{
		return PQ_NULL_ARGUMENT;
	}

	pqRemoveByNode(queue, listGetFirstNode(queue->combinedElementList));

	queue->iterator = NULL;
	return PQ_SUCCESS;
}

PQElement pqGetFirst(PriorityQueue queue)
{
	if (queue == NULL || pqGetSize(queue) == 0) {
		return NULL;
	}

	queue->iterator = listGetFirstNode(queue->combinedElementList);

	return ((CombinedElement)listNodeGetData(queue->iterator))->element;
}

PriorityQueueResult pqInsert(PriorityQueue queue, PQElement element, PQElementPriority priority)
{
	if (queue == NULL || element == NULL || priority == NULL) {
		return PQ_NULL_ARGUMENT;
	}

	CombinedElement combinedElement = createCombinedElement(queue, element, priority);
	if (combinedElement == NULL)
	{
		return PQ_OUT_OF_MEMORY;
	}
	
	Node newNode = listCreateNewNode(combinedElement);
	if (newNode == NULL)
	{
		destroyCombinedElement(queue, combinedElement);
		return PQ_OUT_OF_MEMORY;
	}

	Node lastBiggerNode = getLastBiggerNode(queue, newNode);
	if (lastBiggerNode == NULL)
	{
		listInsertStart(queue->combinedElementList, newNode);
	}
	else
	{
		listInsertAfter(queue->combinedElementList, lastBiggerNode, newNode);
	}

	queue->iterator = NULL;
	return PQ_SUCCESS;
}

PQElement pqGetNext(PriorityQueue queue)
{
	if (queue == NULL || queue->iterator == NULL)
	{
		return NULL;
	}

	queue->iterator = listGetNextNode(queue->iterator);
	
	if (queue->iterator == NULL)
	{
		return NULL;
	}
	
	return ((CombinedElement)listNodeGetData(queue->iterator))->element;
}

bool pqContains(PriorityQueue queue, PQElement element)
{
	if (queue == NULL || element == NULL)
	{
		return false;
	}

	Node matchedNode = getFirstEqualNodeByElement(queue, element);
	if (matchedNode == NULL)
	{
		return false;
	}

	return true;
}

PriorityQueueResult pqRemoveElement(PriorityQueue queue, PQElement element)
{
	if (queue == NULL || element == NULL)
	{
		return PQ_NULL_ARGUMENT;
	}

	Node toDeleteNode = getFirstEqualNodeByElement(queue, element);
	if (toDeleteNode == NULL)
	{
		return PQ_ELEMENT_DOES_NOT_EXISTS;
	}

	pqRemoveByNode(queue, toDeleteNode);

	queue->iterator = NULL;

	return PQ_SUCCESS;
}

PriorityQueue pqCopy(PriorityQueue queue)
{
	if (queue == NULL)
	{
		return NULL;
	}

	PriorityQueue copy = pqCreate(queue->copyElement, queue->freeElement, queue->equalElements, 
		queue->copyElementPriority, queue->freeElementPriority, queue->comparePriorities);
	if (copy == NULL)
	{
		return NULL;
	}

	CombinedElement combinedElement = NULL;
	LIST_FOREACH(Node, node, queue->combinedElementList) {
		combinedElement = listNodeGetData(node);
		if (pqInsert(copy, combinedElement->element, combinedElement->priority) != PQ_SUCCESS)
		{
			pqDestroy(copy);
			return NULL;
		}
	}

	queue->iterator = NULL;
	copy->iterator = NULL;

	return copy;
}

PriorityQueueResult pqChangePriority(PriorityQueue queue, PQElement element,
	PQElementPriority old_priority, PQElementPriority new_priority)
{
	if (queue == NULL || element == NULL || old_priority == NULL || new_priority == NULL)
	{
		return PQ_NULL_ARGUMENT;
	}

	Node targetOld = getFirstIdenticalNodeByElementAndPriority(queue, element, old_priority);
	if (targetOld == NULL)
	{
		return PQ_ELEMENT_DOES_NOT_EXISTS;
	}

	pqRemoveByNode(queue, targetOld);
	queue->iterator = NULL;
	return pqInsert(queue, element, new_priority);
}