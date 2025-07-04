#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>
#include <math.h>
#include "my_timing.h"

// Global variables
int N = 0;
double *data;
int arr[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
pthread_mutex_t lock;

typedef struct
{
    int start;
    int end;
} threadInfo;

int loadData(char *filename)
{
    FILE *fp;

    if (filename != NULL && strlen(filename))
        fp = fopen(filename, "r");
    else
        return -1;

    if (!fp)
        return -1;

    fread(&N, sizeof(int), 1, fp);
    data = (double *)malloc(sizeof(double) * N);
    fread(data, sizeof(double), N, fp);
    fclose(fp);
    // Uncomment this to verify the right data are being read in.
    // For super_short.bin, it should print out
    // data[0] = 97.137926
    // data[1] = 24.639612
    // data[2] = 55.692572
    //   int i;
    //   for (i = 0; i < N; i++) {
    //         printf( "data[%d] = %f\n", i, data[i] );
    //   }
    return 1;
}

int leadingDigit(double n)
{
    if (fabs(n) == 1.0)
        return 1;
    else if (fabs(n) == 0.0)
        return 0;
    else if (fabs(n) < 1.0)
    {
        double tmp = fabs(n);
        while (tmp < 1.0)
        {
            tmp *= 10.0;
        }
        return (int)floor(tmp);
    }
    else
    {
        long long unsigned in = (long long unsigned)floor(fabs(n));
        while (in > 9)
        {
            in /= 10;
        }
        return in;
    }
}
void *thread_func(void *threadinfo)
{
    threadInfo *d = (threadInfo *)threadinfo;
    int count[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

    for (int i = d->start; i <  d->end; i++)
    {
        int x = leadingDigit(data[i]);
        count[x]++;
    }

    pthread_mutex_lock(&lock);
    for (int i = 0; i < 10; i++)
    {
        arr[i] += count[i];
    }
    pthread_mutex_unlock(&lock);

    pthread_exit(NULL);
}


int main()
{
    double t1, t2;
    pthread_t thread[8];
    threadInfo data[8];

    int succ;
    succ = loadData("medium.bin");
    if (!succ)
    {
        return -1;
    }

    data[0].start = 0;
    data[0].end = N / 8;

    data[1].start = N / 8;
    data[1].end = 2 * (N / 8);

    data[2].start = 2 * (N / 8);
    data[2].end = 3 * (N / 8);

    data[3].start = 3 * (N / 8);
    data[3].end = 4 * (N / 8);

    data[4].start = 4 * (N / 8);
    data[4].end = 5 * (N / 8);

    data[5].start = 5 * (N / 8);
    data[5].end = 6 * (N / 8);

    data[6].start = 6 * (N / 8);
    data[6].end = 7 * (N / 8);

    data[7].start = 7 * (N / 8);
    data[7].end = N;


    pthread_mutex_init(&lock, NULL);

    t1 = get_time_sec();

    for (int i = 0; i < 8; i++)
    {
        pthread_create(&(thread[i]), NULL, thread_func, &(data[i]));
    }

    for (int i = 0; i < 8; i++)
    {
        pthread_join(thread[i], NULL);
    }

    t2 = get_time_sec();

    printf("It took %f seconds\n", t2 - t1);

    // for (int i = 1; i < 10; i++) {
    // printf( "There are %d %d's\n", arr[i], i );
    // }

    pthread_mutex_destroy(&lock);

    return 0;
}