#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>
#include <sys/types.h>
#include "stack.h"

pthread_mutex_t lock;

static int message = 0;

struct worker {
    int* pTc;
    int* cTp;
};

struct cleaner {
    int* pTc;
    int* cTp;
};

void handler(int signo) {
    
    if (SIGUSR1 == signo) {
        message = 1;
    }
}

// function for cleaner processes
void cleaner(struct cleaner* cleaner) {

    // busy wait
    int active = 1;
    while(active) {
        if (is_empty()) {
            pthread_mutex_lock(&lock);
            if (-1 == write(cleaner->cTp[1], "PUSH 1;", 7)) {
                perror(NULL); 
                return;
            }
            kill(getppid(), SIGUSR1);
            pthread_mutex_unlock(&lock);

        } else if (is_full()) {
            pthread_mutex_lock(&lock);
            if (-1 == write(cleaner->cTp[1], "POP;", 4)) {
                perror(NULL); 
                return;
            }
            kill(getppid(), SIGUSR1);
            pthread_mutex_unlock(&lock);
        }
    }

}

// function for worker processes
void worker(struct worker* worker) {

    // busy wait, need a signal handler for when signals have been sent
    int active = 1;
    while(active) {
        if (0 == is_empty()) {
            pthread_mutex_lock(&lock);

            int decision = rand() % 2;
            if (1 == decision) {
                // push
                if (-1 == write(worker->cTp[1], "PUSH 1;", 7)) {
                    perror(NULL); 
                    return;
                }
            } else {
                // pop
                if (-1 == write(worker->cTp[1], "POP;", 4)) {
                    perror(NULL); 
                    return;
                }
            }
            kill(getppid(), SIGUSR1);
            pthread_mutex_unlock(&lock);
        }
    }
}

// function for main process
void p_zero(int* pTc, int* cTP) {
    // busy wait for commands
    int active = 1;
    while (active) {
        if (message) {
            message = 0;
            printf("parenting...\n");
            /*
            In here is where I would read from pipes to see whats up 
            and change the stack accordingly
            */
            sleep(1);
        }
    }
}

void destroy(struct worker** workers, struct cleaner** cleaners, int a, int b) {

    for (int i = 0; i < a; i++) {
        close(workers[i]->cTp);
        close(workers[i]->pTc);
        free(workers[i]);
    }
    for (int i = 0; i < b; i++) {
        close(cleaners[i]->cTp);
        close(cleaners[i]->pTc);
        free(cleaners[i]);
    }

    free(workers);
    free(cleaners);
}

int main(int argc, char** argv) {

    if (4 != argc) {
        return -1;
    }

    int amount_agents = strtol(argv[1], NULL, 10);
    int amount_workers = strtol(argv[2], NULL, 10);
    int amount_cleaners = strtol(argv[3], NULL, 10);

    if (amount_agents != (amount_workers+amount_cleaners)) {
        return -1;
    }

    signal(SIGUSR1, handler);

    struct worker** workers = malloc(amount_workers*sizeof(worker));
    struct cleaner** cleaners = malloc(amount_cleaners*sizeof(cleaner));

    // fork parent process into 'amount_workers' child processes
    for (int i = 0; i < amount_workers; i++) {
        
        // create pipes then fork
        int pid = fork();

        int pTc[2] = {-1, -1};
        int cTp[2] = {-1, -1};
        if (-1 == pipe(pTc)) {perror(NULL);return -1;}
        if (-1 == pipe(cTp)) {perror(NULL);return -1;}

        struct worker* curr = malloc(sizeof(worker));
        curr->pTc = pTc;
        curr->cTp = cTp;
        workers[i] = curr;

        if (-1 == pid) {
            perror(NULL);
            return -1;
        } else if (0 == pid) {
            worker(curr);
        } else {
            p_zero(pTc, cTp);
        }
    }

    for (int i = 0; i < amount_cleaners; i++) {
        
        // create pipes then fork
        int pid = fork();

        int pTc[2] = {-1, -1};
        int cTp[2] = {-1, -1};
        if (-1 == pipe(pTc)) {perror(NULL);return -1;}
        if (-1 == pipe(cTp)) {perror(NULL);return -1;}

        struct cleaner* curr = malloc(sizeof(cleaner));
        curr->pTc = pTc;
        curr->cTp = cTp;
        cleaners[i] = curr;

        if (-1 == pid) {
            perror(NULL);
            return -1;
        } else if (0 == pid) {
            cleaner(curr);
        } else {
            p_zero(pTc, cTp);
        }
    }

    destroy(workers, cleaners, amount_workers, amount_cleaners);

    return 1;
}