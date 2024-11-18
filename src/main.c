#include "commom.h"
#include "queue.h"

int main()
{
    // get the problem setup
    struct env* env = initialize_env();
    // thread array
    pthread_t* threads = (pthread_t*)malloc(env->num_threads * sizeof(pthread_t));
    // create and execute threads
    for(int i = 0; i < env->num_threads; i++)
    {
        struct thread_args* args = (struct thread_args*)malloc(sizeof(struct thread_args));
        args->env = env;
        args->thread_idx = i;
        int rc = pthread_create(&threads[i], NULL, run_thread, (void *)args);
        if (rc) {
            printf("ERROR; return code from pthread_create() is %d\n", rc);
            exit(-1);
        }
    }

    pthread_exit(NULL);
}