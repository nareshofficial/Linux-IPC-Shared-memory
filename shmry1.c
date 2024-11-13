#include <unistd.h> 
#include <stdlib.h> 
#include <stdio.h> 
#include <string.h> 
#include <sys/shm.h>
#include <stdint.h>  // For uintptr_t

#define TEXT_SZ 2048 

struct shared_use_st {
    int written_by_you;
    char some_text[TEXT_SZ];
};

int main() {
    int running = 1;
    void *shared_memory = (void *)0; 
    struct shared_use_st *shared_stuff; 
    int shmid;
    
    srand((unsigned int)getpid()); 

    // Create shared memory
    shmid = shmget((key_t)1234, sizeof(struct shared_use_st), 0666 | IPC_CREAT);
    printf("Shared memory id is %d \n", shmid);

    if (shmid == -1) {
        fprintf(stderr, "shmget failed\n");
        exit(EXIT_FAILURE);
    }

    // Attach the shared memory
    shared_memory = shmat(shmid, (void *)0, 0);
    if (shared_memory == (void *)-1) {
        fprintf(stderr, "shmat failed\n");
        exit(EXIT_FAILURE);
    }

    // Print address of the shared memory safely using %p or uintptr_t
    printf("Memory Attached at %p\n", shared_memory);
    // Alternatively, use uintptr_t for printing as a number:
    // printf("Memory Attached at %lx\n", (unsigned long)(uintptr_t)shared_memory);

    shared_stuff = (struct shared_use_st *)shared_memory;
    shared_stuff->written_by_you = 0;

    while (running) {
        if (shared_stuff->written_by_you) {
            printf("You Wrote: %s", shared_stuff->some_text);
            sleep(rand() % 4);  // Random sleep between 0 to 3 seconds
            shared_stuff->written_by_you = 0;

            if (strncmp(shared_stuff->some_text, "end", 3) == 0) {
                running = 0;
            }
        }
    }

    // Detach from shared memory
    if (shmdt(shared_memory) == -1) {
        fprintf(stderr, "shmdt failed\n");
        exit(EXIT_FAILURE);
    }

    // Remove shared memory
    if (shmctl(shmid, IPC_RMID, 0) == -1) {
        fprintf(stderr, "failed to delete shared memory\n");
        exit(EXIT_FAILURE);
    }

    exit(EXIT_SUCCESS);
}

