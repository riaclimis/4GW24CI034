#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

sem_t r1, r2;
int deadlock_mode;

void *p1(void *arg)
{
    printf("P1: Acquired R1 (Mutual Exclusion)\n");
    sem_wait(&r1);
    sleep(1);
    if (deadlock_mode)
    {
        printf("P1: Hold&Wait - Trying R2 (Circular Wait)\n");
        sem_wait(&r2); // Deadlock!
    }
    else
    {
        printf("P1: Safe - Acquired R2 (Resource Ordering)\n");
        sem_wait(&r2);
    }
    printf("P1: Critical section\n");
    sem_post(&r2);
    sem_post(&r1);
    return NULL;
}

void *p2(void *arg)
{
    printf("P2: Acquired R2 (Mutual Exclusion)\n");
    sem_wait(&r2);
    sleep(1);
    if (deadlock_mode)
    {
        printf("P2: Hold&Wait - Trying R1 (Circular Wait)\n");
        sem_wait(&r1); // Deadlock!
    }
    else
    {
        printf("P2: Safe - Release R2, Acquire R1 then R2\n");
        sem_post(&r2); // Release higher ID first
        sem_wait(&r1);
        sem_wait(&r2); // Ascending order
    }
    printf("P2: Critical section\n");
    sem_post(&r2);
    sem_post(&r1);
    return NULL;
}

int main()
{
    printf("Deadlock Study Program\n");
    printf("Enter 1 for DEADLOCK demo, 0 for SAFE prevention: ");
    scanf("%d", &deadlock_mode);

    printf("Mode: %s\n", deadlock_mode ? "DEADLOCK (Ctrl+C to stop)" : "SAFE PREVENTION");

    pthread_t t1, t2;
    sem_init(&r1, 0, 1);
    sem_init(&r2, 0, 1);

    pthread_create(&t1, NULL, p1, NULL);
    pthread_create(&t2, NULL, p2, NULL);

    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    if (!deadlock_mode)
    {
        printf("Program completed successfully!\n");
    }
    sem_destroy(&r1);
    sem_destroy(&r2);
    return 0;
}