// CPSC:426 Lab 2 - POSIX Threads and Semaphores
/* Cigarette Smokers Solution */

/* This program executes 12 threads, 3 taking the role of 'agent',
 * 6 taking the roles of the 'smokers' (with two of each type),
 * and 3 'pushers' to handle the interactions of these threads.
 * This program does no actual work, but ouputs update messages
 * to demonstrate its order of execution succesfully resolves
 * the Cigarette Smokers problem. */

#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

// Declare semaphores and shared variables for the threads
sem_t agentMutex;
sem_t tobacco;
sem_t match;
sem_t paper;
sem_t signalTobacco;
sem_t signalMatch;
sem_t signalPaper;
sem_t pusherMutex;
int isTobacco = 0;
int isMatch = 0;
int isPaper = 0;

// Function prototypes
void* agentNoTobacco();
void* agentNoMatch();
void* agentNoPaper();
void* pusherTobacco();
void* pusherMatch();
void* pusherPaper();
void* smokerHasTobacco();
void* smokerHasMatch();
void* smokerHasPaper();

int main()
{
    // Initialize semaphore values
    sem_init(&agentMutex, 0, 1);
    sem_init(&tobacco, 0, 0);
    sem_init(&match, 0, 0);
    sem_init(&paper, 0, 0);
    sem_init(&signalTobacco, 0, 0);
    sem_init(&signalMatch, 0, 0);
    sem_init(&signalPaper, 0, 0);
    sem_init(&pusherMutex, 0, 1);

    // Initialize pthreads
    pthread_t agentTobacco;
    pthread_t agentMatch;
    pthread_t agentPaper;
    pthread_t tobaccoPusher;
    pthread_t matchPusher;
    pthread_t paperPusher;
    pthread_t tobaccoSmoker1;
    pthread_t tobaccoSmoker2;
    pthread_t matchSmoker1;
    pthread_t matchSmoker2;
    pthread_t paperSmoker1;
    pthread_t paperSmoker2;

    // Create pthreads
    // Create agent threads last so they can run immediately
    pthread_create(&tobaccoPusher, NULL, pusherTobacco, NULL);
    pthread_create(&matchPusher, NULL, pusherMatch, NULL);
    pthread_create(&paperPusher, NULL, pusherPaper, NULL);
    pthread_create(&tobaccoSmoker1, NULL, smokerHasTobacco, NULL);
    pthread_create(&tobaccoSmoker2, NULL, smokerHasTobacco, NULL);
    pthread_create(&matchSmoker1, NULL, smokerHasMatch, NULL);
    pthread_create(&matchSmoker2, NULL, smokerHasMatch, NULL);
    pthread_create(&paperSmoker1, NULL, smokerHasPaper, NULL);
    pthread_create(&paperSmoker2, NULL, smokerHasPaper, NULL);
    pthread_create(&agentTobacco, NULL, agentNoTobacco, NULL);
    pthread_create(&agentMatch, NULL, agentNoMatch, NULL);
    pthread_create(&agentPaper, NULL, agentNoPaper, NULL);

    // Wait for all pthreads to exit
    pthread_join(agentTobacco, NULL);
    pthread_join(agentMatch, NULL);
    pthread_join(agentPaper, NULL);
    pthread_join(tobaccoPusher, NULL);
    pthread_join(matchPusher, NULL);
    pthread_join(paperPusher, NULL);
    pthread_join(tobaccoSmoker1, NULL);
    pthread_join(tobaccoSmoker2, NULL);
    pthread_join(matchSmoker1, NULL);
    pthread_join(matchSmoker2, NULL);
    pthread_join(paperSmoker1, NULL);
    pthread_join(paperSmoker2, NULL);

    return 0;
}

/* Thread function for the agent offering match and paper */
void* agentNoTobacco(){
    // Limit thread to make offer 6 times
    int offers = 6;
    while (offers > 0){
        // Wait for the table to be clear
        sem_wait(&agentMutex);
        // Offer products to the smokers
        printf("Agent offers Match and Paper\n");
        sem_post(&match);
        sem_post(&paper);
        --offers;
        // Sleep for 0, 50, 100, 150, or 200ms
        int wait = (rand() % 5) * 0.05;
        sleep(wait);
    }

    // Run out of products and exit
    printf("Agent offering Match and Paper is done and exits\n");
    pthread_exit(0);
}

/* Thread function for the agent offering tobacco and paper */
void* agentNoMatch(){
    // Limit thread to make offer 6 times
    int offers = 6;
    while (offers > 0){
        // Wait for the table to be clear
        sem_wait(&agentMutex);
        // Offer products to the smokers
        printf("Agent offers Tobacco and Paper\n");
        sem_post(&tobacco);
        sem_post(&paper);
        --offers;
        // Sleep for 0, 50, 100, 150, or 200ms
        int wait = (rand() % 5) * 0.05;
        sleep(wait);
    }

    // Run out of products and exit
    printf("Agent offering Tobacco and Paper is done and exits\n");
    pthread_exit(0);
}

/* Thread function for the agent offering tobacco and match */
void* agentNoPaper(){
    // Limit thread to make offer 6 times
    int offers = 6;
    while (offers > 0){
        // Wait for the table to be clear
        sem_wait(&agentMutex);
        // Offer products to the smokers
        printf("Agent offers Tobacco and Match\n");
        sem_post(&tobacco);
        sem_post(&match);
        --offers;
        // Sleep for 0, 50, 100, 150, or 200ms
        int wait = (rand() % 5) * 0.05;
        sleep(wait);
    }

    // Run out of products and exit
    printf("Agent offering Tobacco and Match is done and exits\n");
    pthread_exit(0);
}

/* Thread function for the pusher moving tobacco */
void* pusherTobacco(){
    // Limit pusher to moving 12 of their ingredient
    int ingredient = 12;
    while (ingredient > 0){
        // Wait until tobacco is on the table
        sem_wait(&tobacco);
        // Wait until other pushers are idle
        sem_wait(&pusherMutex);
        // Check if either other pusher is ready
        if (isMatch == 1){
            // Dismiss the other pusher
            isMatch = 0;
            // Signal the correct smoker
            printf("Pusher moving tobacco offers tobacco and a match to a smoker with paper\n");
            sem_post(&signalPaper);
        }
        else if (isPaper == 1){
            // Dismiss the other pusher
            isPaper = 0;
            // Signal the correct smoker
            printf("Pusher moving tobacco offers tobacco and paper to a smoker with matches\n");
            sem_post(&signalMatch);
        }
        else {
            // Show the tobacco is available and allow another pusher to continue
            printf("Pusher moving tobacco shows tobacco to the other pushers\n");
            isTobacco = 1;
        }
        // Release the semaphore to the next pusher
        sem_post(&pusherMutex);
        --ingredient;
    }

    // Pusher finishes moving their ingredient and exits
    printf("Pusher moving tobacco is done and exits\n");
    pthread_exit(0);
}

/* Thread function for the pusher moving match */
void* pusherMatch(){
    // Limit pusher to moving 12 of their ingredient
    int ingredient = 12;
    while (ingredient > 0){
        // Wait until match is on the table
        sem_wait(&match);
        // Wait until other pushers are idle
        sem_wait(&pusherMutex);
        // Check if either other pusher is ready
        if (isTobacco == 1){
            // Dismiss the other pusher
            isTobacco = 0;
            // Signal the correct smoker
            printf("Pusher moving matches offers a match and tobacco to a smoker with paper\n");
            sem_post(&signalPaper);
        }
        else if (isPaper == 1){
            // Dismiss the other pusher
            isPaper = 0;
            // Signal the correct smoker
            printf("Pusher moving matches offers a match and paper to a smoker with tobacco\n");
            sem_post(&signalTobacco);
        }
        else {
            // Show the match is available and allow another pusher to continue
            printf("Pusher moving matches shows match to the other pushers\n");
            isMatch = 1;
        }
        // Release the semaphore to the next pusher
        sem_post(&pusherMutex);
        --ingredient;
    }

    // Pusher finishes moving their ingredient and exits
    printf("Pusher moving matches is done and exits\n");
    pthread_exit(0);
}

/* Thread function for the pusher moving paper */
void* pusherPaper(){
    // Limit pusher to moving 12 of their ingredient
    int ingredient = 12;
    while (ingredient > 0){
        // Wait until paper is on the table
        sem_wait(&paper);
        // Wait until other pushers are idle
        sem_wait(&pusherMutex);
        // Check if either other pusher is ready
        if (isTobacco == 1){
            // Dismiss the other pusher
            isTobacco = 0;
            // Signal the correct smoker
            printf("Pusher moving paper offers paper and tobacco to a smoker with matches\n");
            sem_post(&signalMatch);
        }
        else if (isMatch == 1){
            // Dismiss the other pusher
            isMatch = 0;
            // Signal the correct smoker
            printf("Pusher moving paper offers paper and a match to a smoker with tobacco\n");
            sem_post(&signalTobacco);
        }
        else {
            // Show the paper is available and allow another pusher to continue
            printf("Pusher moving paper shows paper to the other pushers\n");
            isPaper = 1;
        }
        // Release the semaphore to the next pusher
        sem_post(&pusherMutex);
        --ingredient;
    }

    // Pusher finishes moving their ingredient and exits
    printf("Pusher moving paper is done and exits\n");
    pthread_exit(0);
}

/* Thread function for the smoker holding tobacco */
void* smokerHasTobacco(){
    // Limit smoker to 3 cigarettes
    int cigarettes = 3;
    while (cigarettes > 0){
        // Wait for a pusher to offer match and paper
        sem_wait(&signalTobacco);
        // Make and smoke the cigarette
        printf("One smoker with tobacco makes and smokes a cigarette\n");
        sleep(0.05);
        // Signal that the table is empty again
        sem_post(&agentMutex);
        sleep(0.05);
        --cigarettes;
    }

    // Smoker gets hungry and leaves
    printf("One smoker with tobacco gets hungry and leaves\n");
    pthread_exit(0);
}

/* Thread function for the smoker holding matches */
void* smokerHasMatch(){
    // Limit smoker to 3 cigarettes
    int cigarettes = 3;
    while (cigarettes > 0){
        // Wait for a pusher to offer tobacco and paper
        sem_wait(&signalMatch);
        // Make and smoke the cigarette
        printf("One smoker with matches makes and smokes a cigarette\n");
        sleep(0.05);
        // Signal that the table is empty again
        sem_post(&agentMutex);
        sleep(0.05);
        --cigarettes;
    }

    // Smoker gets hungry and leaves
    printf("One smoker with matches gets hungry and leaves\n");
    pthread_exit(0);
}

/* Thread function for the smoker holding paper */
void* smokerHasPaper(){
    // Limit smoker to 3 cigarettes
    int cigarettes = 3;
    while (cigarettes > 0){
        // Wait for a pusher to offer tobacco and match
        sem_wait(&signalPaper);
        // Make and smoke the cigarette
        printf("One smoker with paper makes and smokes a cigarette\n");
        sleep(0.05);
        // Signal that the table is empty again
        sem_post(&agentMutex);
        sleep(0.05);
        --cigarettes;
    }

    // Smoker gets hungry and leaves
    printf("One smoker with paper gets hungry and leaves\n");
    pthread_exit(0);
}
