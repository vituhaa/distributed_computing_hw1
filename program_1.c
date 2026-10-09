#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>

#define HEIGHT 100
#define WIDTH 100

int thread_count;
int point_count;

FILE* output;

void* routine(void* rank){
	long my_rank = (long)rank;
	for (int h = 0; h < HEIGHT; h++){
		for (int w = 0; w < WIDTH; w++){
			float c_y = -1.0f + h * (2.0f / HEIGHT);
			float c_x = -1.0f + w * (2.0f / WIDTH);

			float z_x = 0.0f;
			float z_y = 0.0f;
			float z_x1 = 0.0f;
			float z_y1 = 0.0f;

			int iterations = 0;

			while (z_x1 + z_y1 <= 4 && iterations < 100){
				z_y = 2 * z_x * z_y + c_y;
				z_x = z_x1 - z_y1 + c_x;

				z_x1 = z_x * z_x;
				z_y1 = z_y * z_y;

				iterations++;
			}
			// запись в файл
        		fprintf(output, "%f, %f\n", c_x, c_y);
		}
	}
	return NULL;
}

int main(int argc, char** argv) {

	thread_count = strtol(argv[1], NULL, 10);
	point_count = strtol(argv[2], NULL, 10);

	pthread_t* thread_handles = malloc(thread_count * sizeof(pthread_t));

	// создание файла
        output = fopen("Mandelbrot.csv", "w+");
	fprintf(output, "X, Y\n");

	for (long i = 0; i < thread_count; ++i){
		pthread_create(&thread_handles[i], NULL, routine, (void*) i);
	}

	for (long i = 0; i < thread_count; ++i){
		pthread_join(thread_handles[i], NULL);
	}

	fclose(output);

	free(thread_handles);
	return 0;
}
