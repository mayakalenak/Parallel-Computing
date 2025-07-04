#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "ppmIO.h"
#include "my_timing.h"

#define NUMTHREADS 1 // 2 //4
// pthread_mutex_t lock;
// int r, g, b;

typedef struct
{
	Pixel *src;
	int start;
	int end;
} threadInfo;

void *thread_func(void *threadinfo)
{
	threadInfo *pixel = (threadInfo *)threadinfo;
	// pthread_mutex_lock(&lock);
	for (int i = pixel->start; i < pixel->end; i++)
	{
		pixel->src[i].r = pixel->src[i].r > 128 ? (220 + pixel->src[i].r) / 2 : (30 + pixel->src[i].r) / 2;
		pixel->src[i].b = pixel->src[i].b > 128 ? (220 + pixel->src[i].b) / 2 : (30 + pixel->src[i].b) / 2;
		pixel->src[i].g = pixel->src[i].g > 128 ? (220 + pixel->src[i].g) / 2 : (30 + pixel->src[i].g) / 2;
	}
	// pthread_mutex_unlock(&lock);
}

int main(int argc, char *argv[])
{
	int colors, rows, cols;
	Pixel *src;
	double t1, t2;
	int i;
	pthread_t thread[NUMTHREADS];
	threadInfo data[NUMTHREADS];

	// check usage
	if (argc < 2)
	{
		printf("Usage: %s <image filename>\n", argv[0]);
		exit(-1);
	}

	// read image and check for errors
	src = ppm_read(&rows, &cols, &colors, argv[1]);
	if (!src)
	{
		printf("Unable to read file %s\n", argv[1]);
		exit(-1);
	}

	int N = rows * cols;

	for (int i = 0; i < NUMTHREADS; i++)
	{
		data[i].src = src;
		data[i].start = i * (N / NUMTHREADS);
		data[i].end = (i == NUMTHREADS - 1) ? N : (i + 1) * (N / NUMTHREADS);
	}

	// pthread_mutex_init(&lock, NULL);

	t1 = get_time_sec();

	for (int i = 0; i < NUMTHREADS; i++)
	{
		pthread_create(&(thread[i]), NULL, thread_func, &(data[i]));
	}

	for (int i = 0; i < NUMTHREADS; i++)
	{
		pthread_join(thread[i], NULL);
	}

	t2 = get_time_sec();

	// write out the image
	ppm_write(src, rows, cols, colors, "bold.ppm");

	printf("It took %f seconds for the whole thing to run\n", t2 - t1);

	free(src);

	// pthread_mutex_destroy(&lock);

	return (0);
}