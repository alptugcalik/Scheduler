#include <ucontext.h>
#include <stdio.h>
#include <stdlib.h>
#include<unistd.h>
#include <signal.h>
#define STACK_SIZE 4096

struct ThreadInfo {		//thread structure
ucontext_t context;		//thread context
int state;				//thread state: 0 for ready, 1 for run, -1 for finish.
int thread_id; 			//thread id is the index in the threadArray for a thread.
int count;				//each thread will be execute 2 times and this is named as count
int desired_count;		//max execution times is desired count.
};

struct ThreadInfo threadArr[5];		//thread array

void exitThread(int index){

	free(threadArr[index].context.uc_stack.ss_sp);		//if count reaches the desired execution times,
	threadArr[index].state = -1;						//release dynamically allocated stack
}														//and specify the state as -1 meaning that it is empty now.

void runThread(int index)		//run function for threads
{
	threadArr[index].state = 1;		//since we are in the run state, corresponding thread's state is assigned as 1.

	switch(index){		//each case corresponds to the one of the threads with its index which is the thread_id
	case 0:				//for thread 0
		threadArr[index].count = threadArr[index].count +1;		//execuiton will be incremented 2 times with 1 seconds delay.
		printf("%d\n", threadArr[index].count);		//count namely execution number is printed into corresponding array column
		sleep(1);		//sleep 1 second
		threadArr[index].count = threadArr[index].count +1;		//another increment
		printf("%d\n", threadArr[index].count);					//another print in the corresponding column
		threadArr[index].state = 0;		//ready thread since we are getting out from the run function
		if(threadArr[index].count >= threadArr[index].desired_count)
		{
			exitThread(index);
		}
		sleep(1);		//to activate alarm again into 2 seconds
	case 1:		//for thread 1
		threadArr[index].count = threadArr[index].count +1;		//increment execuiton number.
		printf("\t %d\n", threadArr[index].count);				//print count in the corresponding thread number
		sleep(1);
		threadArr[index].count = threadArr[index].count +1;		//another increment after 1 second sleep
		printf("\t %d\n", threadArr[index].count);				//another print
		threadArr[index].state = 0;		//ready thread since we are getting out from the run function
		if(threadArr[index].count >= threadArr[index].desired_count)  //if count reaches the desired execution times,
		{
			exitThread(index);
		}
		sleep(1);
	case 2:		//for thread 2
		threadArr[index].count = threadArr[index].count +1;		//increment execuiton number.
		printf("\t \t %d\n", threadArr[index].count);			//print count in the corresponding thread number
		sleep(1);
		threadArr[index].count = threadArr[index].count +1;		//another increment after 1 second sleep
		printf("\t \t %d\n", threadArr[index].count);			//another print
		threadArr[index].state = 0;		//ready thread since we are getting out from the run function
		if(threadArr[index].count >= threadArr[index].desired_count)
		{														//if count reaches the desired execution times,
			exitThread(index);
		}
		sleep(1);
	case 3:		//for thread 3
		threadArr[index].count = threadArr[index].count +1;		//increment execuiton number.
		printf("\t \t \t %d\n", threadArr[index].count);		//print count in the corresponding thread number
		sleep(1);
		threadArr[index].count = threadArr[index].count +1;		//another increment after 1 second sleep
		printf("\t \t \t %d\n", threadArr[index].count);		//another print
		threadArr[index].state = 0;		//ready thread since we are getting out from the run function
		if(threadArr[index].count >= threadArr[index].desired_count)	//if count reaches the desired execution times,
		{
			exitThread(index);
		}
		sleep(1);
	}
}

int createThread(int index){
	int x = getcontext(&(threadArr[index].context));		// get context creat a context for user threads
	if (x == -1)	// if returned vaue is -1, it cannot be created
	{
		printf("Thread cannot be created, Error! ");		//error message
	}

}

void initializeThread(int des_count, int index)		// thread parameters are initialized
{
	threadArr[index].context.uc_link = &threadArr[4].context;		//firstly, linked to the swapped thread which is thread[4]
	threadArr[index].context.uc_stack.ss_sp = malloc(STACK_SIZE);	// dynamically stack is allocated
	threadArr[index].context.uc_stack.ss_size = STACK_SIZE;
	makecontext(&threadArr[index].context, (void (*)(void))runThread, 1, index);	// then context is linked to the function with index of
	threadArr[index].thread_id = index;												//coressponding thread which is the id actually
	threadArr[index].count = 0;		//execution count
	threadArr[index].desired_count = des_count;		//desired execution number
	threadArr[index].state = 0;		//state is initialized as 0 which means ready
};

void scheduler(int zero){		//scheduler function (int zero is used due to the requirement of the signal function properties)
	signal(SIGALRM, scheduler);		//signal is created with schedular function and alarm(2) which is interrupt at each 2 seconds
	alarm(2);
	int x, i;		//randomly x generated (since scheduling is carrying out with respect to lottery scheduling)
	for (i=0;;i++)		//then seach for
	{
		x= rand()%4;      //randomly x generated (since scheduling is carrying out with respect to lottery scheduling)
		if(threadArr[x].count < threadArr[x].desired_count)		//if its corresponding executions ar e completed generate another new one to find
		{													//a thread which is not completed yet.
		break;
		}
	}
	swapcontext(&threadArr[4].context, &threadArr[x].context);		//then call the run function  with swap context.
}																	//all threads are linked to the thread[4]

int main(int argc, char *argv[])
{
	int numbers[4];		//the execution times for the threads will be taken as input from the argument linek in terminal window
	int i;
	for (i=0; i<4; i++)		//the input numbers are stored into numbers array
	{
		numbers[i]=atoi(argv[i+1]);
	}
	for (i=0; i<4; i++)		//then, threads are created and initialized up to 4.
	{
		createThread(i);
		initializeThread(numbers[i], i);
	}
int sum =0;		//to observe shared, total execution for each thread, these are implemented. \t for the space.
	for (i=0; i<4; i++)
	{
		sum = sum + numbers[i];
	}
	printf("share:\n");		//shared part of the pirnt
	printf("%d/%d ",numbers[0], sum);
	printf("\t");
	printf("%d/%d ",numbers[1], sum);
	printf("\t");
	printf("%d/%d ",numbers[2], sum);
	printf("\t");
	printf("%d/%d ",numbers[3], sum);
	printf("\n");
	printf("\n");
	sleep(1);
	printf("Threads: \n" );		//thread id's of the print
	printf("T1 \t T2 \t T3 \t T4 \n");
	sleep(1);
	getcontext(&threadArr[4].context);		//at last, 5th thread is created, which is linked to the each of the other 4 threads to
	scheduler(0);			//switch each of them into swap function

	while (1)		//this is the infinite loop, waiting an interrupt.
	{

	}
	return 0;
}
