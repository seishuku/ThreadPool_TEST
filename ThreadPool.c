#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <assert.h>
#include "threads.h"

#define DBGPRINTF(...) { fprintf(stderr, __VA_ARGS__); }

bool Done=false;

typedef struct
{
	uint32_t Index, Eye;
	uint32_t Seed;
	uint8_t Done;
} ThreadData_t;

ThreadData_t ThreadData[4];
ThreadWorker_t Thread[4];

inline uint32_t random(uint32_t *Seed)
{
	return (((*Seed=*Seed*214013L+2531011L)>>16)&0x7fff);
}

void Job(void *Arg)
{
	ThreadData_t *Data=(ThreadData_t *)Arg;

	Data->Index=random(&Data->Seed);
	Data->Eye=random(&Data->Seed);

	Data->Done=1;
	assert(Data->Done);
}

void ThreadConstructor(void *Arg)
{
	ThreadData_t *Data=(ThreadData_t *)Arg;

	Data->Seed=(uint32_t)Arg;
	Data->Done=0;
	Data->Index=0;
	Data->Eye=0;

	DBGPRINTF("Worker thread %lld starting...\r\n", pthread_self());
}

void ThreadDestructor(void *Arg)
{
	DBGPRINTF("Worker thread %lld stopping...\r\n", pthread_self());
}

int main(int argc, char **argv)
{
	Thread_Init(&Thread[0]);
	Thread_AddConstructor(&Thread[0], ThreadConstructor, (void *)&ThreadData[0]);
	Thread_AddDestructor(&Thread[0], ThreadDestructor, NULL);
	Thread_Start(&Thread[0]);

	Thread_Init(&Thread[1]);
	Thread_AddConstructor(&Thread[1], ThreadConstructor, (void *)&ThreadData[1]);
	Thread_AddDestructor(&Thread[1], ThreadDestructor, NULL);
	Thread_Start(&Thread[1]);

	Thread_Init(&Thread[2]);
	Thread_AddConstructor(&Thread[2], ThreadConstructor, (void *)&ThreadData[2]);
	Thread_AddDestructor(&Thread[2], ThreadDestructor, NULL);
	Thread_Start(&Thread[2]);

	Thread_Init(&Thread[3]);
	Thread_AddConstructor(&Thread[3], ThreadConstructor, (void *)&ThreadData[3]);
	Thread_AddDestructor(&Thread[3], ThreadDestructor, NULL);
	Thread_Start(&Thread[3]);

	while(1)
	{
		Thread_AddJob(&Thread[0], Job, (void *)&ThreadData[0]);
		Thread_AddJob(&Thread[1], Job, (void *)&ThreadData[1]);
		Thread_AddJob(&Thread[2], Job, (void *)&ThreadData[2]);
		Thread_AddJob(&Thread[3], Job, (void *)&ThreadData[3]);

		volatile int32_t Timeout=INT32_MAX;
		while(!(ThreadData[0].Done&&ThreadData[1].Done&&ThreadData[2].Done&&ThreadData[3].Done))
		{
			if(Timeout<0)
			{
				DBGPRINTF("Timed out...\a\n");
				break;
			}

			Timeout--;
		}

		ThreadData[0].Done=0;
		ThreadData[1].Done=0;
		ThreadData[2].Done=0;
		ThreadData[3].Done=0;

		DBGPRINTF("0: %d %d\n1: %d %d\n2: %d %d\n3: %d %d\n",
				  ThreadData[0].Eye, ThreadData[0].Index,
				  ThreadData[1].Eye, ThreadData[1].Index,
				  ThreadData[2].Eye, ThreadData[2].Index,
				  ThreadData[3].Eye, ThreadData[3].Index);
	}

	Thread_Destroy(&Thread[0]);
	Thread_Destroy(&Thread[1]);
	Thread_Destroy(&Thread[2]);
	Thread_Destroy(&Thread[3]);
}
