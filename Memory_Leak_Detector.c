#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
*  SIZE_OF_PATH 				- Length of the string containg path to the log file_name
*  MAX_LENGTH_OF_FUNCTION_NAME 	- Maximum length of function name in the file_name to be checked 
*  OUTPUT_FILE_PATH 			- Specifies the path to the log file_name
*/

#define SIZE_OF_PATH 					256
#define MAX_LENGTH_OF_FUNCTION_NAME  	100
#define OUTPUT_FILE_PATH 				"Memeory_Leak_Info.txt"

typedef struct  _ALLOCATED_MEM_INFO
{
	void* address;
	long int size;
	unsigned int line_number;
	char function_name[MAX_LENGTH_OF_FUNCTION_NAME];
	char file_name[SIZE_OF_PATH];
} _ALLOCATED_MEM_INFO;

typedef struct _LEAK_INFO
{
	struct _LEAK_INFO* next;
	_ALLOCATED_MEM_INFO data;
} _LEAK_INFO;

_LEAK_INFO* start  = NULL;

void addToLeakList(_LEAK_INFO info)
{
	_LEAK_INFO* new_entry = (_LEAK_INFO*)malloc(sizeof(_LEAK_INFO));

	new_entry->data.size = info.data.size;
	new_entry->data.line_number = info.data.line_number;
	new_entry->data.address = info.data.address;
	strcpy(new_entry->data.file_name, info.data.file_name);
	strcpy(new_entry->data.function_name, info.data.function_name);
	new_entry->next=NULL;

	if(start==NULL)
	{
		start = new_entry;
	}
	else
	{
		new_entry->next = start;
		start = new_entry;
	}
}

void logInformation(void* address, unsigned long int size, const char* file_name, const char* function_name, unsigned int line_number)
{
	_LEAK_INFO new_node;

	new_node.data.address = address;
	new_node.data.line_number = line_number;
	memset(new_node.data.file_name,0,SIZE_OF_PATH);
	memset(new_node.data.function_name,0,MAX_LENGTH_OF_FUNCTION_NAME);
	strcpy(new_node.data.file_name, file_name );
	strcpy(new_node.data.function_name, function_name );
	new_node.data.size = size;
	addToLeakList(new_node);
}


void deleteLog(_LEAK_INFO* address)
{
	if(start==NULL)
		return;
	_LEAK_INFO* iterator, *temp;
	if(start->data.address==address)
	{
		temp = start;
		start = temp->next;
		free(temp);
	}
	else
	{
		for(iterator = start; iterator->next!=NULL && (iterator->next)->data.address!=address; iterator=iterator->next); 
		if(iterator!=NULL && (iterator->next)->data.address==address)
		{
			temp = iterator->next;
			iterator->next = temp->next;
			free(temp); 
		}
	}
}

void* modifiedMalloc(long int size, const char* file_name, const char* function_name, unsigned int line_number)
{
	if(size<=0)
		return NULL;
	void* address = malloc(size);
	if(address!=NULL)
	{
		logInformation(address, size, file_name, function_name, line_number);
	}
	return address;
}

void* modifiedCalloc(long int size, long int elements, const char* file_name, const char* function_name, unsigned int line_number)
{
	if(elements<=0)
		return NULL;
	unsigned long int total_size = size*elements;
	void* address = calloc(size, elements);
	if(address!=NULL)
	{
		logInformation(address, total_size, file_name, function_name, line_number);
	}
	return address;
}

void modifiedFree(void* address)
{
	if(address!=NULL)
	{
		deleteLog(address);
		free(address);
	}
}


void clearLeakList()
{
	_LEAK_INFO* iterator = start;
	_LEAK_INFO* temp = iterator;
	while(iterator!=NULL)
	{
		iterator = iterator->next;
		free(temp);
		temp = iterator;
	}
}

void reportLeak()
{
	FILE* fileObject = fopen(OUTPUT_FILE_PATH, "w");
	char info[2000];
	if(fileObject!=NULL)
	{
		sprintf(info, "%s\n", "\t\tSummary");
		fwrite(info, strlen(info)+1, 1, fileObject);
		sprintf(info, "%s\n", "-----------------------------------");
		fwrite(info, strlen(info)+1, 1, fileObject);
		if(start==NULL)
		{
			sprintf(info, "No memory leak found!\n");
			fwrite(info, (strlen(info) + 1) , 1, fileObject);
		}
		else
		{
			for(_LEAK_INFO* iterator = start; iterator!=NULL; iterator = iterator->next)
			{
				sprintf(info, "In file     		: %s\n", iterator->data.file_name);
				fwrite(info, (strlen(info) + 1) , 1, fileObject);
				
				sprintf(info, "In function 		: %s\n", iterator->data.function_name);
				fwrite(info, (strlen(info) + 1) , 1, fileObject);

				sprintf(info, "At line number 	: %d\n", iterator->data.line_number);
				fwrite(info, (strlen(info) + 1) , 1, fileObject);

				sprintf(info, "Leak of size 		: %ld Bytes\n", iterator->data.size);			
				fwrite(info, (strlen(info) + 1) , 1, fileObject);

				sprintf(info, "%s\n", "-----------------------------------");	
				fwrite(info, (strlen(info) + 1) , 1, fileObject);
			}
		}
	}
	clearLeakList();
}

#define calloc(size, elements) 	modifiedCalloc(size, elements, __FILE__, __FUNCTION__, __LINE__)
#define malloc(size) 			modifiedMalloc(size, __FILE__, __FUNCTION__, __LINE__)
#define free(address) 			modifiedFree(address)