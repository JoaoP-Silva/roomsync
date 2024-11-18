#include "commom.h"

void passa_tempo(int tid, int sala, int decimos)
{
    struct timespec zzz, agora;
    static struct timespec inicio = {0,0};
    int tstamp;

    if ((inicio.tv_sec == 0)&&(inicio.tv_nsec == 0)) {
        clock_gettime(CLOCK_REALTIME,&inicio);
    }

    zzz.tv_sec  = decimos/10;
    zzz.tv_nsec = (decimos%10) * 100L * 1000000L;

    if (sala==0) {
        nanosleep(&zzz,NULL);
        return;
    }

    clock_gettime(CLOCK_REALTIME,&agora);
    tstamp = ( 10 * agora.tv_sec  +  agora.tv_nsec / 100000000L )
            -( 10 * inicio.tv_sec + inicio.tv_nsec / 100000000L );

    printf("%3d [ %2d @%2d z%4d\n",tstamp,tid,sala,decimos);

    nanosleep(&zzz,NULL);

    clock_gettime(CLOCK_REALTIME,&agora);
    tstamp = ( 10 * agora.tv_sec  +  agora.tv_nsec / 100000000L )
            -( 10 * inicio.tv_sec + inicio.tv_nsec / 100000000L );

    printf("%3d ) %2d @%2d\n",tstamp,tid,sala);
}


/** initialize_env - Initializes the problem environment from the stdin
 * Return: A struct env with the problem setup.
 */
struct env* initialize_env()
{
    int num_rooms, num_threads;
    scanf("%d", &num_rooms);
    scanf("%d", &num_threads);
    
    // initialize rooms and threads array
    struct room* _rooms = (struct room*)malloc(num_rooms * sizeof(struct room));
    struct thread* _threads = (struct thread*)malloc(num_threads * sizeof(struct thread));

    // fill the threads array
    for(int i = 0; i < num_threads; i++)
    {   
        _threads[i].curr_room = -1;
        _threads[i].queue_pos = -1;
        _threads[i].idx = i;
        _threads[i].free_to_go = 0;
        scanf("%d", &_threads[i].id);
        scanf("%d", &_threads[i].init);
        scanf("%d", &_threads[i].path_size);

        _threads[i].path = (int*)malloc(_threads[i].path_size * sizeof(int));
        _threads[i].times = (int*)malloc(_threads[i].path_size * sizeof(int));

        // initialize thread path and times
        for(int j = 0; j < _threads[i].path_size; j++)
        { 
            scanf("%d", &_threads[i].path[j]);
            scanf("%d", &_threads[i].times[j]);
        }
    }

    // initialize rooms
    for(int i = 0; i < num_rooms; i++)
    {
        _rooms[i].id = i + 1;
        _rooms[i].active_threads = 0;

        _rooms[i].queue = (struct queue*)malloc(sizeof(struct queue));
        _rooms[i].queue->size = 0;
        _rooms[i].queue->head = NULL;
        _rooms[i].queue->tail = NULL;

        pthread_mutex_init(&_rooms[i].mutex, NULL);
        pthread_cond_init(&_rooms[i].cond, NULL);
    }

    struct env* env = (struct env*)malloc(sizeof(env));
    env->rooms = _rooms;
    env->threads = _threads;
    env->num_rooms = num_rooms;
    env->num_threads = num_threads;
    
    return env;
}

/** wait_room - Moves a thread to a room queue
 * thread_idx - The thread idx in the threads array
 * room_id - The room id
 * struct env* env - Pointer to environment
 * */ 
void wait_room(int thread_idx, int room_id, struct env* env)
{
    struct thread* my_thread = &env->threads[thread_idx];
    struct room* room = &env->rooms[room_id - 1];

    pthread_mutex_lock(&room->mutex);
    // add thread to queue
    queue_append(thread_idx, room->queue);
    // update the thread queue position
    my_thread->queue_pos = room->queue->size;
    //printf("Eu sou a thread %d e estou na %d posicao da fila da sala %d.\n", my_thread->id, my_thread->queue_pos, room_id);
    pthread_mutex_unlock(&room->mutex);
}

/** let_in - Mark the next 3 threads in the room queue as free to go and remove them from
 *          the queue.
 * struct room* r - Pointer to the room 
 * struct env* env - Pointer to env.
 */
void let_in(struct room* r, struct env* env)
{   
    // lock mutex
    pthread_mutex_lock(&r->mutex);
    struct queue* q = r->queue;

    int idx = queue_pop(q);
    env->threads[idx].free_to_go = 1;
    idx = queue_pop(q);
    env->threads[idx].free_to_go = 1;
    idx = queue_pop(q);
    env->threads[idx].free_to_go = 1;

    // update the threads position in the queue
    struct node* it = q->head;
    for(int i = 0; i < q->size; i++)
    {
        struct thread* t = &env->threads[it->val];
        t->queue_pos = i + 1;
        it = it->next;
    }

    // update the active threads var
    r->active_threads = 3;
    // release
    pthread_mutex_unlock(&r->mutex);
}


/** leave_room - The thread leaves the current room.
 * struct thread* t - Pointer to the thread.
 * struct env* env - Pointer to env.
 */
void leave_room(struct thread* t, struct env* env)
{
    if(t->curr_room > 0)
    {
        int curr_room_idx = t->curr_room - 1;
        struct room* curr_room = &env->rooms[curr_room_idx];
        pthread_mutex_lock(&curr_room->mutex);
        curr_room->active_threads -= 1;
        
        // signal to mark the room as free
        if (curr_room->active_threads == 0)
            pthread_cond_signal(&curr_room->cond);

        pthread_mutex_unlock(&curr_room->mutex);
    }
}

/** run_thread - The thread routine
 * _args - Contains the thread_idx and a pointer to the environment.
 */
void* run_thread(void* _args)
{

    struct thread_args* args = (struct thread_args*)_args;
    
    // instantiate the thread and the rooms vector
    struct thread* my_thread = &args->env->threads[args->thread_idx];
    struct room* rooms = args->env->rooms;

    // initial spend time
    passa_tempo(my_thread->id, 0, my_thread->init);


    for(int i = 0; i < my_thread->path_size; i++)
    {
        int room_idx = my_thread->path[i] - 1;
        struct room* next_room = &rooms[room_idx];
        // add the current thread to the next room queue 
        wait_room(my_thread->idx, next_room->id, args->env);
        do
        {
            if(my_thread->queue_pos == 3){
                // wait for the room to be empty
                pthread_mutex_lock(&next_room->mutex);
                while(next_room->active_threads > 0)
                    pthread_cond_wait(&next_room->cond, &next_room->mutex);

                pthread_mutex_unlock(&next_room->mutex);
                
                // give the room access for the first 3 threads 
                let_in(next_room, args->env);
            }

        } while (!my_thread->free_to_go);
        // reset the free to go flag
        my_thread->free_to_go = 0;
        
        // leave the current room
        leave_room(my_thread, args->env);
        
        // update current room
        my_thread->curr_room = next_room->id;
        
        // spend time in the next room
        passa_tempo(my_thread->id, my_thread->curr_room, my_thread->times[i]);
    }
    leave_room(my_thread, args->env);
    pthread_exit(NULL);
}