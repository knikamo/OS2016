/**
 * Rendezvous
 *
 * Two threads executing chunks of work in a lock step - skeleton
 * code.
 *
 * Author: Nikos Nikoleris <nikos.nikoleris@it.uu.se>
 *
 */

#include <stdio.h>     /* printf() */
#include <stdlib.h>    /* abort(), [s]rand() */
#include <unistd.h>    /* sleep() */
#include <semaphore.h> /* sem_...() */
#include <pthread.h>   /* pthread_...() */
#include <errno.h>     /* errno */

#define LOOPS 5
#define NTHREADS 3
#define MAX_SLEEP_TIME 3

sem_t sem1;
sem_t sem2;

/* TODO: Make the two threads perform their iterations in a
 * predictable way. Both should perform iteration 1 before iteration 2
 * and then 2 before 3 etc. */

void *
threadA(void *param __attribute__((unused)))
{
    int i;

    for (i = 0; i < LOOPS; i++) {
        sem_wait(&sem1);
            //int value;
            //sem_getvalue(&sem, &value);
        
	printf("threadA --> %d iteration\n", i);
	sleep(rand() % MAX_SLEEP_TIME);
        sem_post(&sem2);
    }

    pthread_exit(0);
}


void *
threadB(void *param  __attribute__((unused)))
{
    int i;
 
    for (i = 0; i < LOOPS; i++) {
        sem_wait(&sem2);

	printf("threadB --> %d iteration\n", i);
	sleep(rand() % MAX_SLEEP_TIME);
        sem_post(&sem1);
    }

    pthread_exit(0);
}

int
main()
{
    sem_init(&sem1, 0, 1);
    sem_init(&sem2, 0, 1);

    
    
    pthread_t tidA, tidB;
    
    srand(time(NULL));
    pthread_setconcurrency(3);




    if (pthread_create(&tidA, NULL, threadA, NULL) ||
	pthread_create(&tidB, NULL, threadB, NULL)) {
	perror("pthread_create");
	abort();
    }
    if (pthread_join(tidA, NULL) != 0 ||
        pthread_join(tidB, NULL) != 0) {
	perror("pthread_join");
	abort();
    }

    return 0;
}

/*
 * Local Variables:
 * mode: c
 * c-basic-offset: 4
 * indent-tabs-mode: nil
 * c-file-style: "stroustrup"
 * End:
 */
