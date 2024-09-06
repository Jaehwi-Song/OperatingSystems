#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>

int shared_resource = 0;

#define NUM_ITERS 70
#define NUM_THREADS 70


// flag and turn global variable for peterson algorithm
int flag[NUM_THREADS];
int turn[NUM_THREADS];

int condition = 0;  // if 0 -> peterson algorithm, if 1 -> block&awake algorithm

// struct node to save blocked threads
typedef struct threadNode{
    int tid;
    struct threadNode *prev;
    struct threadNode *next;
} threadNode;


// implemented semaphore
typedef struct {
    int value;   // value for semaphore
    struct threadNode *tail;
    struct threadNode *thread_list;
} Semaphore;

// Global State and LinkedList for blocked threads
Semaphore State;

void peterson_entry(int tid);
void peterson_exit(int tid);
void lock(int tid);
void unlock(int tid);
void condition_check(void);

void condition_check(void) {
    // if # of threads and iters are large, use sleep&wakeup algorithm
    if (NUM_THREADS >= 80 && NUM_ITERS >= 80)
        condition = 1;
    // else use peterson algorithm
    else
        condition = 0;
}

void peterson_entry(int tid)
{
    for (int i = 0; i < NUM_THREADS; i++) {
        flag[tid] = 1;  // thread with tid try to enter critical section
        turn[tid] = i;

        int wait;   // busy waiting if wait == 1, else enter critical section
        do {
            wait = 0;
            for (int j = 0; j < NUM_THREADS; j++) 
            {
                // if there exists a thread running critical section
                if (j != i && flag[j] && (turn[tid] > turn[j] || (turn[tid] == turn[j] && tid > j)))
                {
                    wait = 1;
                    break;
                }
            }
        } while (wait);
    }
}

void peterson_exit(int tid)
{
    flag[tid] = 0;
}

void lock(int tid) 
{
    if(condition) 
    {
        peterson_entry(tid);
            int isSleep = 0;
            State.value--;
            if (State.value < 0) 
            {
                // malloc new thread Node
                threadNode *newNode = (threadNode*)malloc(sizeof(threadNode));
                newNode->tid = tid;
                newNode->next = NULL;
                newNode->prev = NULL;
                if (State.thread_list == NULL)
                {
                    State.thread_list = newNode;
                }
                else
                {
                    newNode->prev = State.tail;
                    State.tail->next = newNode;
                }
                State.tail = newNode;
                isSleep = 1;
                peterson_exit(tid);
                //block();    -> since block() is not supported, used sleep instead
                sleep(100);
            }
        if (isSleep)
            peterson_exit(tid);
    }
    else 
    {
        peterson_entry(tid);
    }
}

void unlock(int tid)
{
    if(condition)
    {
        peterson_entry(tid);
        State.value++;
        if (State.value <= 0)
        {
            threadNode *Node = State.thread_list;
            State.thread_list->next->prev = NULL;
            State.thread_list = State.thread_list->next;
            // wakeup(Node->tid);  // want to wakeup a thread with particular tid, but it is not supported.
            free(Node);
        }
        peterson_exit(tid);
    }
    else 
    {
        peterson_exit(tid);
    }
}


void* thread_func(void* arg) {
    int tid = *(int*)arg;
    
    lock(tid);
    
        for(int i = 0; i < NUM_ITERS; i++)    shared_resource++;
    
    unlock(tid);
    
    pthread_exit(NULL);
}

int main() {
    pthread_t threads[NUM_THREADS];
    int tids[NUM_THREADS];
    // initialize flags and turn
    for (int i = 0; i < NUM_THREADS; i++) {
        flag[i] = 0;
        turn[i] = 0;
    }
    // failed to implement, Block/Wakeup so fixed condition to 0
    // condition_check();
    // initialize LinkedList when using Block/Wakeup algorithm (but failed to implement)
    if (condition)
    {
        State.value = 1;
        State.thread_list = NULL;
        State.tail = NULL;
    }
    for (int i = 0; i < NUM_THREADS; i++) {
        tids[i] = i;
        pthread_create(&threads[i], NULL, thread_func, &tids[i]);
    }
    
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    printf("shared: %d\n", shared_resource);
    
    return 0;
}