#include "event_manager.h"
#include "priority_queue.h"
#include "stdlib.h"
#include "stdio.h"
#include "string.h"

typedef struct EventManager_t
{
	PriorityQueue events;
	PriorityQueue members;
	Date createdDate;
	Date currentDate;
};

typedef struct Event_t
{
	int id;
	char* name;
	Date date;
	PriorityQueue members;
} *Event;

typedef struct Member_t
{
	int id;
	char* name;
	int countEvents;
} *Member;

static char* copyString(char* target, const char* source)
{
	if (target == NULL || source == NULL)
	{
		return NULL;
	}
	int i;
	for (i = 0; source[i] != '\0'; i++)
	{
		target[i] = source[i];
	}

	target[i] = '\0';

	return target;
}

static PQElement copyEventGeneric(PQElement n) {
	if (!n) {
		return NULL;
	}
	Event copy = malloc(sizeof(*copy));
	if (!copy) {
		return NULL;
	}
	copy->name = malloc(strlen(((Event)n)->name) + 1);
	if (copy->name == NULL) {
		free(copy);
		return NULL;
	}
	Date copyDate = dateCopy(((Event)n)->date);
	if (!copyDate) {
		free(copy->name);
		free(copy);
		return NULL;
	}
	PriorityQueue copyMembers = pqCopy(((Event)n)->members);
	if (!copyMembers) {
		free(copy->name);
		dateDestroy(copyDate);
		free(copy);
		return NULL;
	}

	copyString(copy->name, ((Event)n)->name);
	copy->date = copyDate;
	copy->id = ((Event)n)->id;
	copy->members = copyMembers;

	return copy;
}

static PQElementPriority copyDateGeneric(PQElementPriority n) {
	return dateCopy(n);
}

static void freeEventGeneric(PQElement n) {
	
	dateDestroy(((Event)n)->date);
	free(((Event)n)->name);
	pqDestroy(((Event)n)->members);
	free(n);
}

static void freeDateGeneric(PQElementPriority n) {
	dateDestroy(n);
}

static int compareDatesGeneric(PQElementPriority n1, PQElementPriority n2) {
	return dateCompare((Date)n1, (Date)n2) * (-1);
}

static bool equalEventsGeneric(PQElement n1, PQElement n2) {
	return ((Event)n1)->id == ((Event)n2)->id;
}

static PQElement copyMemberGeneric(PQElement n) {
	if (!n) {
		return NULL;
	}
	Member copy = malloc(sizeof(*copy));
	if (!copy) {
		return NULL;
	}
	copy->name = malloc(strlen(((Member)n)->name) + 1);
	if (copy->name == NULL) {
		free(copy);
		return NULL;
	}

	copyString(copy->name, ((Member)n)->name);
	copy->id = ((Member)n)->id;
	copy->countEvents = ((Member)n)->countEvents;

	return copy;
}

static PQElementPriority copyIntGeneric(PQElementPriority n) {
	if (!n) {
		return NULL;
	}
	int* copy = malloc(sizeof(*copy));
	if (!copy) {
		return NULL;
	}
	*copy = *(int*)n;
	return copy;
}

static void freeMemberGeneric(PQElement n) {
	free(((Member)n)->name);
	free(n);
}

static void freeIntGeneric(PQElementPriority n) {
	free(n);
}

static int compareIntsGeneric(PQElementPriority n1, PQElementPriority n2) {
	return (*(int*)n2 - *(int*)n1);
}

static int compareIntsLargerHasHigherPriorityGeneric(PQElementPriority n1, PQElementPriority n2) {
	return (*(int*)n1 - *(int*)n2);
}

static bool equalMembersGeneric(PQElement n1, PQElement n2) {
	return ((Member)n1)->id == ((Member)n2)->id;
}

static Event emCreateEvent(char* name, int id, Date date)
{
	if (name == NULL || id < 0)
	{
		return NULL;
	}

	Event event = malloc(sizeof(*event));
	if (!event)
	{
		return NULL;
	}

	Date newDate = dateCopy(date);
	if (!newDate)
	{
		free(event);
		return NULL;
	}

	event->name = malloc(strlen(name) + 1);
	if (event->name == NULL)
	{
		dateDestroy(newDate);
		free(event->name);
		return NULL;
	}

	PriorityQueue memberQueue = pqCreate(copyMemberGeneric, freeMemberGeneric, equalMembersGeneric, copyIntGeneric, freeIntGeneric, compareIntsGeneric);
	if (!memberQueue)
	{
		dateDestroy(newDate);
		free(event->name);
		free(event);
		return NULL;
	}

	copyString(event->name, name);
	event->id = id;
	event->members = memberQueue;
	event->date = newDate;

	return event;
}

static bool emEventWithNameAndDateExists(EventManager em, char* name, Date date)
{
	PQ_FOREACH(Event, event, em->events)
	{
		if (strcmp(event->name, name) == 0 && dateCompare(event->date, date) == 0)
		{
			return true;
		}
	}
	return false;
}

static void emRemoveAllMembersFromEvent(EventManager em, Event event)
{
	if (em == NULL || event == NULL)
	{
		return;
	}
	while (pqGetSize(event->members) != 0)
	{
		emRemoveMemberFromEvent(em, ((Member)pqGetFirst(event->members))->id, event->id);
	}
}

static EventManagerResult emDeleteEventById(EventManager em, int event_id)
{
	if (em == NULL || event_id < 0)
	{
		return EM_ERROR;
	}

	PQ_FOREACH(Event, event, em->events)
	{
		if (event->id == event_id)
		{
			emRemoveAllMembersFromEvent(em, event);
			pqRemoveElement(em->events, event);
			return EM_SUCCESS;
		}
	}

	return EM_EVENT_NOT_EXISTS;
}

static Event emGetEventById(EventManager em, int event_id)
{
	if (em == NULL || event_id < 0)
	{
		return NULL;
	}

	PQ_FOREACH(Event, event, em->events)
	{
		if (event->id == event_id)
		{
			return event;
		}
	}

	return NULL;
}

static Member emGetMemberById(PriorityQueue queue, int member_id)
{
	if (queue == NULL)
	{
		return NULL;
	}

	PQ_FOREACH(Member, member, queue)
	{
		if (member->id == member_id)
		{
			return member;
		}
	}

	return NULL;
}

static Member emCreateMember(char* name, int id)
{
	if (name == NULL || id < 0)
	{
		return NULL;
	}

	Member member = malloc(sizeof(*member));
	if (member == NULL)
	{
		return NULL;
	}

	member->name = malloc(sizeof(char) * strlen(name) + 1);
	if (member->name == NULL)
	{
		free(member);
		return NULL;
	}

	member->id = id;
	copyString(member->name, name);
	member->countEvents = 0;

	return member;
}

static void emRemoveTodayEvents(EventManager em)
{
	if (em == NULL)
	{
		return;
	}

	Event event = pqGetFirst(em->events);
	while (event != NULL && dateCompare(event->date, em->currentDate) == 0)
	{
		emRemoveAllMembersFromEvent(em, event);
		pqRemoveElement(em->events, event);
		event = pqGetFirst(em->events);
	}
}

EventManager createEventManager(Date date)
{
	if (date == NULL)
	{
		return NULL;
	}

	EventManager eventManager = malloc(sizeof(*eventManager));
	Date createdDate = dateCopy(date);
	Date currentDate = dateCopy(date);
	PriorityQueue eventQueue = pqCreate(copyEventGeneric, freeEventGeneric, equalEventsGeneric, copyDateGeneric, freeDateGeneric, compareDatesGeneric);
	PriorityQueue memberQueue = pqCreate(copyMemberGeneric, freeMemberGeneric, equalMembersGeneric, copyIntGeneric, freeIntGeneric, compareIntsGeneric);
	if (eventManager == NULL || createdDate == NULL || currentDate == NULL || eventQueue == NULL || memberQueue == NULL)
	{
		dateDestroy(createdDate);
		dateDestroy(currentDate);
		pqDestroy(eventQueue);
		pqDestroy(memberQueue);
		destroyEventManager(eventManager);
		return NULL;
	}

	eventManager->currentDate = currentDate;
	eventManager->createdDate = createdDate;
	eventManager->events = eventQueue;
	eventManager->members = memberQueue;

	return eventManager;
}

void destroyEventManager(EventManager em)
{
	if (em == NULL)
	{
		return;
	}
	
	dateDestroy(em->createdDate);
	dateDestroy(em->currentDate);
	pqDestroy(em->events);
	pqDestroy(em->members);
	free(em);
}

EventManagerResult emAddEventByDate(EventManager em, char* event_name, Date date, int event_id)
{
	if (em == NULL || event_name == NULL || date == NULL)
	{
		return EM_NULL_ARGUMENT;
	}

	if (dateCompare(em->currentDate, date) > 0)
	{
		return EM_INVALID_DATE;
	}

	if (event_id < 0)
	{
		return EM_INVALID_EVENT_ID;
	}

	if (emEventWithNameAndDateExists(em, event_name, date) == true)
	{
		return EM_EVENT_ALREADY_EXISTS;
	}

	Event newEvent = emCreateEvent(event_name, event_id, date);
	if (newEvent == NULL)
	{
		destroyEventManager(em);
		return EM_OUT_OF_MEMORY;
	}

	if (pqContains(em->events, newEvent))
	{
		freeEventGeneric(newEvent);
		return EM_EVENT_ID_ALREADY_EXISTS;
	}

	if (pqInsert(em->events, newEvent, newEvent->date) != PQ_SUCCESS)
	{
		destroyEventManager(em);
		return EM_OUT_OF_MEMORY;
	}

	freeEventGeneric(newEvent);

	return EM_SUCCESS;
}

EventManagerResult emAddEventByDiff(EventManager em, char* event_name, int days, int event_id)
{
	if (em == NULL || event_name == NULL)
	{
		return EM_NULL_ARGUMENT;
	}

	if (days < 0)
	{
		return EM_INVALID_DATE;
	}

	Date calculatedDate = dateCopy(em->currentDate);
	while (days > 0)
	{
		dateTick(calculatedDate);
		days--;
	}

	EventManagerResult result = emAddEventByDate(em, event_name, calculatedDate, event_id);
	dateDestroy(calculatedDate);

	return result;
}

EventManagerResult emRemoveEvent(EventManager em, int event_id)
{
	if (em == NULL)
	{
		return EM_NULL_ARGUMENT;
	}

	if (event_id < 0)
	{
		return EM_INVALID_EVENT_ID;
	}

	return emDeleteEventById(em, event_id);
}

EventManagerResult emChangeEventDate(EventManager em, int event_id, Date new_date)
{
	if (em == NULL || new_date == NULL)
	{
		return EM_NULL_ARGUMENT;
	}

	if (dateCompare(em->currentDate, new_date) > 0)
	{
		return EM_INVALID_DATE;
	}

	if (event_id < 0)
	{
		return EM_INVALID_EVENT_ID;
	}

	Event target = emGetEventById(em, event_id);
	if (target == NULL)
	{
		return EM_EVENT_ID_NOT_EXISTS;
	}

	if (emEventWithNameAndDateExists(em, target->name, new_date) == true)
	{
		return EM_EVENT_ALREADY_EXISTS;
	}

	Event targetCopy = copyEventGeneric(target);

	if (pqChangePriority(em->events, targetCopy, targetCopy->date, new_date) == PQ_OUT_OF_MEMORY)
	{
		destroyEventManager(em);
		return EM_OUT_OF_MEMORY;
	}

	freeEventGeneric(targetCopy);
	target = emGetEventById(em, event_id);
	dateDestroy(target->date);
	target->date = dateCopy(new_date);
	
	return EM_SUCCESS;
}

EventManagerResult emAddMember(EventManager em, char* member_name, int member_id)
{
	if (em == NULL || member_name == NULL)
	{
		return EM_NULL_ARGUMENT;
	}

	if (member_id < 0)
	{
		return EM_INVALID_MEMBER_ID;
	}

	Member target = emGetMemberById(em->members, member_id);
	if (target != NULL)
	{
		return EM_MEMBER_ID_ALREADY_EXISTS;
	}

	Member member = emCreateMember(member_name, member_id);
	if (member == NULL)
	{
		destroyEventManager(em);
		return EM_OUT_OF_MEMORY;
	}

	if (pqInsert(em->members, member, &member_id) != PQ_SUCCESS)
	{
		destroyEventManager(em);
		return EM_OUT_OF_MEMORY;
	}

	freeMemberGeneric(member);

	return EM_SUCCESS;
}

EventManagerResult emAddMemberToEvent(EventManager em, int member_id, int event_id)
{
	if (em == NULL)
	{
		return EM_NULL_ARGUMENT;
	}

	if (member_id < 0)
	{
		return EM_INVALID_MEMBER_ID;
	}

	if (event_id < 0)
	{
		return EM_INVALID_EVENT_ID;
	}

	Event event = emGetEventById(em, event_id);
	if (event == NULL)
	{
		return EM_EVENT_ID_NOT_EXISTS;
	}

	Member member = emGetMemberById(em->members, member_id);
	if (member == NULL)
	{
		return EM_MEMBER_ID_NOT_EXISTS;
	}

	if (pqContains(event->members, member) == true)
	{
		return EM_EVENT_AND_MEMBER_ALREADY_LINKED;
	}

	if (pqInsert(event->members, member, &member_id) != PQ_SUCCESS)
	{
		destroyEventManager(em);
		return EM_OUT_OF_MEMORY;
	}

	member->countEvents++;

	return EM_SUCCESS;
}

EventManagerResult emRemoveMemberFromEvent(EventManager em, int member_id, int event_id)
{
	if (em == NULL)
	{
		return EM_NULL_ARGUMENT;
	}

	if (member_id < 0)
	{
		return EM_INVALID_MEMBER_ID;
	}

	if (event_id < 0)
	{
		return EM_INVALID_EVENT_ID;
	}

	Member memberEventManager = emGetMemberById(em->members, member_id);
	if (memberEventManager == NULL)
	{
		return EM_MEMBER_ID_NOT_EXISTS;
	}

	Event event = emGetEventById(em, event_id);
	if (event == NULL)
	{
		return EM_EVENT_ID_NOT_EXISTS;
	}

	Member memberEvent = emGetMemberById(event->members, member_id);
	if (memberEvent == NULL)
	{
		return EM_EVENT_AND_MEMBER_NOT_LINKED;
	}

	pqRemoveElement(event->members, memberEvent);
	memberEventManager->countEvents--;

	return EM_SUCCESS;
}

EventManagerResult emTick(EventManager em, int days)
{
	if (em == NULL)
	{
		return EM_NULL_ARGUMENT;
	}

	if (days <= 0)
	{
		return EM_INVALID_DATE;
	}

	while (days > 0)
	{
		emRemoveTodayEvents(em);
		dateTick(em->currentDate);
		days--;
	}

	return EM_SUCCESS;
}

int emGetEventsAmount(EventManager em)
{
	if (em == NULL)
	{
		return -1;
	}

	return pqGetSize(em->events);
}

char* emGetNextEvent(EventManager em)
{
	if (em == NULL)
	{
		return NULL;
	}

	Event nextEvent = pqGetFirst(em->events);
	if (nextEvent == NULL)
	{
		return NULL;
	}

	return nextEvent->name;
}

static int getDigitsLength(int number)
{
	if (number == 0)
	{
		return 1;
	}

	int count = 0;
	int negativeSign = 0;
	if (number < 0)
	{
		negativeSign = 1;
	}

	while (number != 0)
	{
		number = number / 10;
		count++;
	}
	return count + negativeSign;
}

static char* concatStrings(char* target, char* source)
{
	if (target == NULL || source == NULL)
	{
		return NULL;
	}

	int i = 0;
	while (target[i] != '\0') {
		i++;
	}

	for (int j = 0; source[j] != '\0'; ++j, ++i) {
		target[i] = source[j];
	}

	target[i] = '\0';
	return target;
}

static char* dateToString(Date date)
{
	int day, month, year;
	dateGet(date, &day, &month, &year);
	// length of: day.month.year
	int length = getDigitsLength(day) + getDigitsLength(month) + getDigitsLength(year) + 2;
	char* dayStr = malloc(getDigitsLength(day) + 1);
	char* monthStr = malloc(getDigitsLength(month) + 1);
	char* yearStr = malloc(getDigitsLength(year) + 1);
	char* dateStr = malloc(length + 1);
	*dateStr = '\0';
	if (dateStr == NULL || dayStr == NULL || monthStr == NULL || yearStr == NULL)
	{
		free(dateStr);
		free(dayStr);
		free(monthStr);
		free(yearStr);
		return;
	}
	_itoa_s(day, dayStr, getDigitsLength(day) + 1, 10);
	_itoa_s(month, monthStr, getDigitsLength(month) + 1, 10);
	_itoa_s(year, yearStr, getDigitsLength(year) + 1, 10);

	concatStrings(dateStr, dayStr);
	concatStrings(dateStr, ".");
	concatStrings(dateStr, monthStr);
	concatStrings(dateStr, ".");
	concatStrings(dateStr, yearStr);

	free(dayStr);
	free(monthStr);
	free(yearStr);

	return dateStr;
}

static char* emCreatePrintableLine(Event event, char* date)
{
	if (event == NULL || date == NULL)
	{
		return;
	}
	int countMembers = pqGetSize(event->members);
	int memberNamesLength = 0;
	PQ_FOREACH(Member, member, event->members)
	{
		memberNamesLength += strlen(member->name);
	}

	int lineLength = strlen(event->name) + 1 + strlen(date) + memberNamesLength + countMembers;

	char* out = malloc(lineLength + 1);
	*out = '\0';
	if (out == NULL)
	{
		return;
	}

	concatStrings(out, event->name);
	concatStrings(out, ",");
	concatStrings(out, date);
	PQ_FOREACH(Member, member, event->members)
	{
		concatStrings(out, ",");
		concatStrings(out, member->name);
	}

	return out;
}

static void emPrintEvent(FILE* stream, Event event)
{
	if (stream == NULL || event == NULL)
	{
		return;
	}

	char* date = dateToString(event->date);
	if (date == NULL)
	{
		return;
	}

	char* line = emCreatePrintableLine(event, date);
	if (line == NULL)
	{
		free(date);
		return;
	}

	fprintf(stream, line);
	fprintf(stream, "\n");

	free(date);
	free(line);
}

void emPrintAllEvents(EventManager em, const char* file_name)
{
	if (em == NULL || file_name == NULL)
	{
		return;
	}

	FILE* stream;
	errno_t err = fopen_s(&stream, file_name, "w");
	if (err != 0)
	{
		return;
	}

	PQ_FOREACH(Event, event, em->events)
	{
		emPrintEvent(stream, event);
	}

	fclose(stream);
}

static char* intToString(int num)
{
	int length = getDigitsLength(num);
	char* result = malloc(length + 1);
	*result = '\0';
	if (result == NULL)
	{
		return;
	}
	_itoa_s(num, result, getDigitsLength(num) + 1, 10);

	return result;
}

void emPrintAllResponsibleMembers(EventManager em, const char* file_name)
{
	if (em == NULL || file_name == NULL)
	{
		return;
	}

	FILE* stream;
	errno_t err = fopen_s(&stream, file_name, "w");
	if (err != 0)
	{
		return;
	}

	PriorityQueue newMemberQueue = pqCreate(copyMemberGeneric, freeMemberGeneric, equalMembersGeneric, copyIntGeneric, freeIntGeneric, compareIntsLargerHasHigherPriorityGeneric);

	PQ_FOREACH(Member, member, em->members)
	{
		pqInsert(newMemberQueue, member, &member->countEvents);
	}

	char* countEventsString;
	PQ_FOREACH(Member, member, newMemberQueue)
	{
		if (member->countEvents != 0)
		{
			countEventsString = intToString(member->countEvents);

			fprintf(stream, member->name);
			fprintf(stream, ",");
			fprintf(stream, countEventsString);
			fprintf(stream, "\n");

			free(countEventsString);
		}
	}

	pqDestroy(newMemberQueue);
	fclose(stream);
}