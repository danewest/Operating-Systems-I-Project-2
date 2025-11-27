/*
 * project2_sample.c
 */ 

#include<pthread.h>
#include<stdio.h>
#include<semaphore.h>

int n;     
sem_t psem, csem; 

void *producer(void *param); 
void *consumer(void *param); 

int main(int argc, char *argv[])
{
  pthread_t tid_producer, tid_consumer;    /* thread identifier */
  pthread_attr_t attr_producer, attr_consumer;  /* set of thread attributes */

  sem_init(&psem, 0, 1); 
  sem_init(&csem, 0, 0); 

  /* get default attribute */
  pthread_attr_init(&attr_producer);
  pthread_attr_init(&attr_consumer);

  /*create threads */
//  pthread_create(&tid_producer, &attr_producer, producer, NULL);
  pthread_create(&tid_consumer, &attr_consumer, consumer, NULL);
  pthread_create(&tid_producer, &attr_producer, producer, NULL);

  /* wait for producer and consumer threads to exit */
  pthread_join(tid_producer, NULL);
  pthread_join(tid_consumer, NULL);

}

void *producer(void *param)
{
  int i;
  for (i=0; i<10; i++)
  { 
     sem_wait(&psem);
     n++;
     sem_post(&csem);
  }

  pthread_exit(0);
}

/* ''consume'' and print value n */
void *consumer(void *param)
{
  int i;
  for (i=0; i<10; i++)
  { 
     sem_wait(&csem);
     printf("%d ", n);
     sem_post(&psem);
  }

  pthread_exit(0);
}
