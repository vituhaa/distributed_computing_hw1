#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include "for_test_3/timer.h"

#define HEIGHT 800
#define WIDTH 800
#define MAX_ITERATIONS 256
#define MAX_RADIUS 4

int thread_count;
int point_count;

int colors[HEIGHT][WIDTH]; // для записи в ppm файл

struct pairs{
	float x, y;
};

struct pairs *coords = NULL; // для записи в csv файл

int iteration_count = 1;

int mandelbrot(float c_x, float c_y){
        float z_x = 0.0f;
        float z_y = 0.0f;
        float zx_sq = 0.0f;
        float zy_sq = 0.0f;

        int iterations = 0;

        while (zx_sq + zy_sq < MAX_RADIUS && iterations < MAX_ITERATIONS){
                z_y = 2 * z_x * z_y + c_y; // мнимая часть 
                z_x = zx_sq - zy_sq + c_x; // действительная

                zx_sq = z_x * z_x;
                zy_sq = z_y * z_y;

                iterations++;
        }
        return iterations;
}

void* routine(void* rank){
        long my_rank = (long)rank;
        int my_ids = HEIGHT / thread_count; // делим на блоки
        int my_start = my_rank * my_ids; // определяем старт для каждого потока
        int my_end = my_start + my_ids; // определяем конец для каждого потока

	int curr_num = 0;
	int break_f = 0; // условие выхода - достигли указанного количества точек

        for (int i = my_start; i < my_end; i++){
                float c_y = -2.0f + i * (4.0f / HEIGHT); // ищем в области [-2, 2]
                for (int j = 0; j < WIDTH; j++){
                        float c_x = -2.5f + j * (4.0f / WIDTH); // ищем в области [-2.5, 1.5]

                        colors[i][j] = mandelbrot(c_x, c_y); // записываем итерации

			if (colors[i][j] == MAX_ITERATIONS){
				// записываем координаты
                        	coords[curr_num].x = c_x;
                        	coords[curr_num].y = c_y;
                        	curr_num += 1;
			}

			// достигли point_count, выходим
			if (curr_num >= point_count){
				break_f = 1;
				break;
                        }
                }
		if (break_f == 1){
			break;
		}
        }
        return NULL;
}

int main(int argc, char** argv) {
	
	double start, end;

        thread_count = strtol(argv[1], NULL, 10);
        point_count = strtol(argv[2], NULL, 10);

        coords = malloc(point_count * sizeof(*coords));

        double total_time = 0.0;
        for (int i = 0; i < iteration_count; i++)
        {
                printf("\nIteration %d\n", i + 1);
		double start, end;

                // создание файла
                FILE* output_csv = fopen("Mandelbrot.csv", "w");   
	        fprintf(output_csv, "X, Y\n");

                // сброс cords и colors для новой итерации
                memset(coords, 0, point_count * sizeof(*coords));
                memset(colors, 0, sizeof colors);

                // создание потоков и выполнение routine
                pthread_t* thread_handles = malloc(thread_count * sizeof(pthread_t));

	        GET_TIME(start);

                for (long i = 0; i < thread_count; ++i){
                        pthread_create(&thread_handles[i], NULL, routine, (void*) i);
                }

                for (long i = 0; i < thread_count; ++i){
                        pthread_join(thread_handles[i], NULL);
                }

	        GET_TIME(end);
                total_time += (end - start);
	        printf("Elapsed time = %.5f (sec)\n", end - start);

	        for (int i = 0; i < point_count; i++){
                        fprintf(output_csv, "%f, %f\n", coords[i].x, coords[i].y);
                }

                fclose(output_csv);

                FILE* output_ppm = fopen("Mandelbrot.ppm", "wb");
                fprintf(output_ppm, "P6\n%d %d\n%d\n", WIDTH, HEIGHT, 255);

                unsigned char color[3];
                for (int h = 0; h < HEIGHT; h++){
                        for (int w = 0; w < WIDTH; w++){
                                if (colors[h][w] == MAX_ITERATIONS){
                                        color[0] = 0; // чёрный - в множестве
                                        color[1] = 0;
                                        color[2] = 0;
                                }
                                else{
                                        color[0] = 255; // белый - не в множестве
                                        color[1] = 255;
                                        color[2] = 255;
                                }
                                fwrite(color, 1, 3, output_ppm);
                        }
                }

                fclose(output_ppm);

                free(thread_handles);
        }

        printf("\nAverage time per %d iterations: %lf (sec)\n", iteration_count, total_time / iteration_count);

	free(coords);

        return 0;
}
