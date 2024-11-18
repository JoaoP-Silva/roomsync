#ifndef COMMOM_H
#define COMMOM_H
#define _POSIX_C_SOURCE 199309L

#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <time.h>

#include "queue.h"

/** struct thread - Represents a thread in the problem 
 * int id - The thread id.
 * int idx - The thread index in the env thread array.
 * int init - The initial sleep time.
 * int path_size - The number of rooms to visit.
 * int* path - Dynamic array to store the sequence of rooms.
 * int* times - Dynamic array to store the time to stay in each room.
 * int queue_pos - The current position in some room queue.
 * int curr_room - The current room of the thread.
 * int free_to_go - A flag to indicate if the thread can enter the next room.
 */
struct thread 
{
    int id;
    int idx;
    int init;
    int path_size;
    int* path;
    int* times;

    int queue_pos;
    int curr_room;
    int free_to_go;
};




/** struct room - Represents a room in the problem
 * int active_threads - The number of active threads in the room
 * pthread_mutex_t room_mutex - Mutex to control the access of active_threads 
 * struct queue* queue - The threads queue
 * int queue_size - The number of threads in the queue
 * pthread_mutex_t queue_mutex - Mutex to control the access in queue
 * pthread_cond_t cond - Condition variable to avoid busy waiting
 */
struct room 
{
    int id;

    int active_threads;
    struct queue* queue;
    pthread_mutex_t mutex;
    pthread_cond_t cond;
};


/** struct env - The environement of the problem 
 * int num_threads - Number of threads
 * int num_rooms - Number of rooms
 * struct thread* threads - Dynamic array of threads
 * struct room* rooms - Dynamic array of rooms
 */
struct env
{
    int num_threads;
    int num_rooms;
    struct thread* threads;
    struct room* rooms;
};

/** struct thread_args - Arguments for threads
 * int thread_idx - The thread index in the thread array
 * struct env* env - The problem environment
 */
struct thread_args
{
    int thread_idx;
    struct env* env;
};


void passa_tempo(int tid, int sala, int decimos);

/** initialize_env - Initializes the problem environment from the stdin
 * Return: A struct env with the problem setup.
 */
struct env* initialize_env();


/** run_thread - The thread routine
 * _args - Contains the thread_idx and a pointer to the environment.
 */
void* run_thread(void* _args);

#endif
