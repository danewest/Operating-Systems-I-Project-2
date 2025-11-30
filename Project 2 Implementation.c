/*
 * Project 2 4.1 Implementation ~ Dane West & Coby West
 */ 

#include<pthread.h>
#include<stdio.h>
#include<stdlib.h>
#include<semaphore.h>
#include<unistd.h>
#include <time.h>

int *buffer;
int buffer_size;

int write_index = 0;
int read_index = 0;

int next_val = 0;
int upper_limit;

int num_prod;
int num_cons;

sem_t empty, full, mutex; 

struct thread_struct {
  int id;
};

struct timespec start, end;
double elapsed;

void *producer(void *param); 
void *consumer(void *param); 


int main(int argc, char *argv[])
{
  if (argc != 5) {
    printf("Usage: %s <buffer_size> <num_producers> <num_consumers> <upper_limit>\n", argv[0]);
    exit(1);
  }

  buffer_size = atoi(argv[1]);
  num_prod = atoi(argv[2]);
  num_cons = atoi(argv[3]);
  upper_limit = atoi(argv[4]);

  buffer = (int *)malloc(sizeof(int) * buffer_size);

  if (buffer == NULL) {
    exit(1);
  }

  //semaphore initialization
  sem_init(&empty, 0, buffer_size);
  sem_init(&full, 0, 0);
  sem_init(&mutex, 0, 1);

  // Start tracking time
  clock_gettime(CLOCK_MONOTONIC, &start);

  //creating producer threads
  pthread_t prod_tids[num_prod];
  struct thread_struct prod_args[num_prod];
  pthread_attr_t prod_attr;
  pthread_attr_init(&prod_attr);

  for (int i = 0; i < num_prod; i++) {
    prod_args[i].id = i + 1;
    pthread_create(&prod_tids[i], &prod_attr, producer, &prod_args[i]);
  }

  //creating consumer threads
  pthread_t cons_tids[num_cons];
  struct thread_struct cons_args[num_cons];
  pthread_attr_t cons_attr;
  pthread_attr_init(&cons_attr);

  for (int i = 0; i < num_cons; i++) {
    cons_args[i].id = i + 1;
    pthread_create(&cons_tids[i], &cons_attr, consumer, &cons_args[i]);
  }

  //wait for all threads
  for (int i = 0; i < num_prod; i++) {
    pthread_join(prod_tids[i], NULL);
  }

  //insert -1 to signal consumers to exit
  for (int i = 0; i < num_cons; i++) {
    sem_wait(&empty);
    sem_wait(&mutex);

    buffer[write_index] = -1;
    write_index = (write_index + 1) % buffer_size;

    sem_post(&mutex);
    sem_post(&full);
  }

  //wait for all consumers to finish
  for (int i = 0; i < num_cons; i++) {
    pthread_join(cons_tids[i], NULL);
  }

  // End of tracking time
  clock_gettime(CLOCK_MONOTONIC, &end);

  // Compute elapsed time following in seconds and print
  elapsed = (end.tv_sec - start.tv_sec)
          + (end.tv_nsec - start.tv_nsec) / 1e9;
  printf("Elapsed time: %f seconds\n", elapsed);

  free(buffer);
  return 0;
}

void *producer(void *param)
{
  struct thread_struct *args = (struct thread_struct *) param;

  while (1) {
        sem_wait(&empty);
        //enter critical section
        sem_wait(&mutex);

        // Uncomment int j and for loop for testing purposes
        //int j;
        //for (j = 0; j < 1000000; j++);

        if (next_val > upper_limit) {
            sem_post(&mutex);
            sem_post(&empty);
            break;
        }

        //produce and insert in same critical section
        int item = next_val;
        next_val++;

        buffer[write_index] = item;
        write_index = (write_index + 1) % buffer_size;

        sem_post(&mutex);
        sem_post(&full);
    }

    pthread_exit(0);
}

/* ''consume'' and print value n */
void *consumer(void *param)
{
  struct thread_struct *args = (struct thread_struct *) param;

  while (1) {
    sem_wait(&full);
    sem_wait(&mutex);

    int j;
    for (j = 0; j < 1000000; j++);

    int item = buffer[read_index];
    read_index = (read_index + 1) % buffer_size;

    sem_post(&mutex);
    sem_post(&empty);

    if (item == -1) {
      pthread_exit(0);
    }

    // Comment out printf() for testing
    if (item <= upper_limit) {
      printf("%d, %d\n", item, args->id);
    }
  }
}
