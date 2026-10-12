#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>

#define HEIGHT 800
#define WIDTH 800
#define MAX_ITERATIONS 256
#define MAX_RADIUS 4

int thread_count;
int point_count;

int colors[HEIGHT][WIDTH];

struct pairs{
	float x, y;
};

struct pairs *coords = NULL;

int mandelbrot(float c_x, float c_y){
        float z_x = 0.0f;
        float z_y = 0.0f;
        float zx_sq = 0.0f;
        float zy_sq = 0.0f;

        int iterations = 0;

        while (zx_sq + zy_sq < MAX_RADIUS && iterations < MAX_ITERATIONS){
                z_y = 2 * z_x * z_y + c_y;
                z_x = zx_sq - zy_sq + c_x;

                zx_sq = z_x * z_x;
                zy_sq = z_y * z_y;

                iterations++;
        }
        return iterations;
}

void* routine(void* rank){
        long my_rank = (long)rank;
        int my_ids = HEIGHT / thread_count;
        int my_start = my_rank * my_ids;
        int my_end = my_start + my_ids;

	int curr_num = 0;
	int break_f = 0;

        for (int i = my_start; i < my_end; i++){
                float c_y = -1.0f + i * (2.0f / HEIGHT);
                for (int j = 0; j < WIDTH; j++){
                        float c_x = -1.0f + j * (2.0f / WIDTH);

                        colors[i][j] = mandelbrot(c_x, c_y);

			coords[curr_num].x = c_x;
			coords[curr_num].y = c_y;
			curr_num += 1;

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

        thread_count = strtol(argv[1], NULL, 10);
        point_count = strtol(argv[2], NULL, 10);

        pthread_t* thread_handles = malloc(thread_count * sizeof(pthread_t));
	coords = malloc(point_count * sizeof(*coords));

        // создание файла
        FILE* output_csv = fopen("Mandelbrot.csv", "w");
	if (!output_csv){
		printf("файла нет\n");
	}
	fprintf(output_csv, "X, Y\n");

        FILE* output_ppm = fopen("Mandelbrot.ppm", "wb");
        fprintf(output_ppm, "P6\n%d %d\n%d\n", WIDTH, HEIGHT, 255);

        for (long i = 0; i < thread_count; ++i){
                pthread_create(&thread_handles[i], NULL, routine, (void*) i);
        }

        for (int i = 0; i < point_count; i++){
                fprintf(output_csv, "%f, %f\n", coords[i].x, coords[i].y);
	}

	unsigned char color[3];

        //for (int h = 0; h < HEIGHT; h++){
        //        for (int w = 0; w < WIDTH; w++){
        //                if (colors[h][w] == MAX_ITERATIONS){
        //                        color[0] = 0;
        //                        color[1] = 0;
        //                        color[2] = 0;
        //                }
        //                else{
        //                        color[0] = 255;
        //                        color[1] = 255;
        //                        color[2] = 255;
        //                }

        //                fwrite(color, 1, 3, output_ppm);
        //        }
        //}
	
	for (int h = 0; h < HEIGHT; h++){
		for (int w = 0; w < WIDTH; w++){
			if (colors[h][w] == MAX_ITERATIONS){
				color[0] = 0;
				color[1] = 0;
				color[2] = 0;
			}
			else{
				color[0] = 255;
				color[1] = 255;
				color[2] = 255;
			}
			fwrite(color, 1, 3, output_ppm);
		}
	}

        for (long i = 0; i < thread_count; ++i){
                pthread_join(thread_handles[i], NULL);
        }

	free(coords);
        free(thread_handles);

	fclose(output_csv);
	fclose(output_ppm);
        return 0;
}
