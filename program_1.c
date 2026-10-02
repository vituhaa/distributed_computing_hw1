#include <stdio.h>
#include <pthread.h>

int thread_count;

void* routine(void* rank){
	long my_rank = (long) rank;

}

int main(int argc, char** argv) {

	thread_count = strtol(argv[1], NULL, 10);

	for (long i = 0; i < thread_count; ++i){
		pthread_create(&thread_handles[i], NULL, routine, (void*) i);
	}
	
	// создание файла
	FILE *output = fopen("Mandelbrot.csv", "w+");
	// запись в файл
	fprintf(output, "Num, X, Y\n");
	fprintf(output, "%d, %d, %d", num, x, y);
	fclose(output);
}
