#ifndef _TIMER_H_
#define _TIMER_H_

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

static double get_time(void) { 
	struct timespec t;
	if (clock_gettime(CLOCK_MONOTONIC, &t)){
		perror("clock_gettime");
		exit(1);
	}
	return (double)t.tv_sec + (double)t.tv_nsec * 1e-9;
}

#endif
