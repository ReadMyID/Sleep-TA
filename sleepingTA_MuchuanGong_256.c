/*
 ============================================================================
 Name        : HW4.c
 Author      : Muchuan Gong
 Version     :
 Copyright   : Your copyright notice
 Description : sleepingTA, Ansi-style
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <time.h>
/*mutex declarations, a global variable*/

pthread_mutex_t mutex_lock; /*protect the gloable variable waiting_student*/

/*semaphore declarations, global variables*/

sem_t students_sem; 	/*ta waits for a student to show up, student notifies ta his/her arrival*/
sem_t ta_sem;    		/*students waits for ta to help,ta notifies students he/she is ready to help*/

/*the number of waiting students, a global variable*/

int waiting_students;

/*the maximum time (in seconds) to sleep*/

#define MAX_SLEEP_TIME  3
/*number of potential students*/

#define NUM_OF_STUDENTS 4
#define NUM_OF_HELPS    2

/*number of available seats*/
#define NUM_OF_SEATS    2




void rand_sleep(void);
void* student_programming(void* stu_id);
void* ta_teaching();

int main(int argc, char **argv){
	printf("CS149 SleepingTA from Muchuan Gong\n");

		pthread_t students[NUM_OF_STUDENTS];
		pthread_t ta;


		int studentid[NUM_OF_STUDENTS]={0};

	sem_init(&students_sem,0,0);
	sem_init(&ta_sem,0,1);
	//set random
	srand(time(NULL));

	pthread_mutex_init(&mutex_lock,NULL);

	pthread_create(&ta,NULL,ta_teaching,NULL);
	//create threads
	for(int i=0; i<NUM_OF_STUDENTS; i++)
	{
		studentid[i] = i+1;
		pthread_create(&students[i], NULL, student_programming, (void*) &studentid[i]);
	}

	pthread_join(ta, NULL);

	//for(i=0; i<NUM_OF_STUDENTS;i++)
	//{
	//	pthread_join(students[i],NULL);
	//}

	return 0;
}

void* student_programming(void* stu_id)
{
	int id = *(int*)stu_id;

	int helpcount = 0;

	while(1)
	{
		printf("        Student %d programming for 3 seconds\n",
				id);
		rand_sleep();
		//sem_wait(&mutex);
		pthread_mutex_lock(&mutex_lock);



		if(waiting_students<NUM_OF_SEATS)
		{

			waiting_students++;

			printf("        student %d takes a seat, # of waiting students = %d\n",
					id,waiting_students);

			sem_post(&students_sem);
			pthread_mutex_unlock(&mutex_lock);
			sem_wait(&ta_sem);

			printf("Student %d revceiving help\n",id);
			helpcount++;
		}
		else //no more chairs
		{
			printf("                student %d will try later\n"
						,id);
			//sem_post(&mutex);
			pthread_mutex_unlock(&mutex_lock);

		}
		if(helpcount >= NUM_OF_HELPS){
					pthread_mutex_unlock(&mutex_lock);
					pthread_exit(0);
				}
	}
}

void* ta_teaching()
{
	int helpcount = 0;
	while(1)
	{

		sem_wait(&students_sem);

		//sem_wait(&mutex);
		pthread_mutex_lock(&mutex_lock);

		printf("Helping a student for 3 second,");

					helpcount++;
					waiting_students--;

		printf(" # of waiting students = %d\n"
				,waiting_students);


		sem_post(&ta_sem);
		//sem_post(&mutex);
		pthread_mutex_unlock(&mutex_lock);
		if(helpcount==NUM_OF_HELPS*NUM_OF_STUDENTS){
			pthread_exit(0);
		}
		rand_sleep();
	}
}

void rand_sleep(void){
	time_t seed;
	time(&seed);
	int time = rand_r(&seed) % MAX_SLEEP_TIME + 1;
	sleep(time);
}
