#include "date.h"
#include "stdlib.h"
#include "stdio.h"

#define INVALID_MONTH 0
#define MIN_DAY 1 
#define MAX_DAY 30
#define MIN_MONTH 1
#define MAX_MONTH 12
#define DAYS_IN_YEAR 365

typedef struct Date_t
{
	int day;
	int month;
	int year;
};

static bool dayIsValid(int day)
{
	return day >= MIN_DAY && day <= MAX_DAY;
}

static bool monthIsValid(int month)
{
	return month >= MIN_MONTH && month <= MAX_MONTH;
}

Date dateCreate(int day, int month, int year)
{
	if (!dayIsValid(day) || !monthIsValid(month))
	{
		return NULL;
	}

	Date date = malloc(sizeof(*date));
	if (date == NULL)
	{
		return NULL;
	}

	date->day = day;
	date->month = month;
	date->year = year;

	return date;
}

void dateDestroy(Date date)
{
	free(date);
}

Date dateCopy(Date date)
{
	if (date == NULL)
	{
		return NULL;
	}

	return dateCreate(date->day, date->month, date->year);
}

bool dateGet(Date date, int* day, int* month, int* year)
{
	if (date == NULL || day == NULL || month == NULL || year == NULL)
	{
		return false;
	}

	*day = date->day;
	*month = date->month;
	*year = date->year;

	return true;
}

int dateCompare(Date date1, Date date2)
{
	if (date1 == NULL || date2 == NULL || (date1->day == date2->day && date1->month == date2->month && date1->year == date2->year))
	{
		return 0;
	}

	if (date1->year < date2->year)
	{
		return -1;
	}
	else if (date1->year == date2->year)
	{
		if (date1->month < date2->month)
		{
			return -1;
		}
		else if (date1->month == date2->month)
		{
			if (date1->day < date2->day)
			{
				return -1;
			}
		}
	}

	return 1;
}

void dateTick(Date date)
{
	if (date == NULL)
	{
		return NULL;
	}

	if (date->day < MAX_DAY)
	{
		date->day++;
		return;
	}

	date->day = MIN_DAY;
	date->month++;

	if (date->month > MAX_MONTH)
	{
		date->month = MIN_MONTH;
		date->year++;
		return;
	}
}