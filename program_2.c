#include <stdio.h>
#include <pthread.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include "for_test_3/timer.h"

const double g = 6.67430e-11;

typedef struct
{
	double mass;
	double radius_list[3];
	double speed_list[3];
} Body;

Body *bodies_list;

int thread_count;
int n; // bodies count
double t_end;
double dt = 0.1;

double *forces; // size = thread_count * n * 3

pthread_barrier_t barrier;

FILE *output_csv; 

int iteration_count = 5;

// сила на тело q со стороны тела k
double calc_fqk(double mk, double mq, double r2, double delta){
    if (r2 == 0){
        printf("Деление на ноль\n");
	exit(1);
    }

    double denominator = pow(fabs(r2), 1.5);
    double fqk = g * mq * mk * delta / denominator;

    return fqk;
}


void* routine(void* rank){
	long my_rank = (long)rank;
	int my_count = n / thread_count; // количество блоков МТ

	double t = 0.0;

	while (t <= t_end){
		//printf("CURR RANK = %ld\n", my_rank);
		
		//printf("CURR T = %lf\n", t);

		double* rank_forces = forces + my_rank * n * 3; // адрес первого элемента массива сил для потока
		memset(rank_forces, 0, n * 3 * sizeof(double)); // заполняем нулями массив сил для потока 

		int pairs = n * (n - 1) / 2;
		int start, end;

		int blocks = pairs / thread_count;
		int tail = pairs % thread_count;

		start = my_rank * blocks + (my_rank < tail ? my_rank : tail);
		end = start + blocks + (my_rank < tail ? 1 : 0);

		//printf("CURR START = %d\n", start);
		//printf("CURR END = %d\n", end);

		int id = 0;
		for (int q = 0; q < n; ++q){
			for (int k = q + 1; k < n; ++k, ++id){
				if (id < start || id >= end){
					continue;
				}

				double delta_x = bodies_list[k].radius_list[0] - bodies_list[q].radius_list[0];
				double delta_y = bodies_list[k].radius_list[1] - bodies_list[q].radius_list[1];
				double delta_z = bodies_list[k].radius_list[2] - bodies_list[q].radius_list[2];

				double r2 = pow(delta_x, 2) + pow(delta_y, 2) + pow(delta_z, 2);
				
				// printf("\n");
				// printf("r2 = %lf\n", r2);
				// printf("\n");

				double f_xqk = calc_fqk(bodies_list[k].mass, bodies_list[q].mass, r2, delta_x);
				double f_yqk = calc_fqk(bodies_list[k].mass, bodies_list[q].mass, r2, delta_y);
				double f_zqk = calc_fqk(bodies_list[k].mass, bodies_list[q].mass, r2, delta_z);

				// третий з-н ньютона
				rank_forces[3 * q + 0] += f_xqk * delta_x;
				rank_forces[3 * q + 1] += f_yqk * delta_y;
				rank_forces[3 * q + 2] += f_zqk * delta_z;

				rank_forces[3 * k + 0] -= f_xqk * delta_x;
				rank_forces[3 * k + 1] -= f_yqk * delta_y;
				rank_forces[3 * k + 2] -= f_zqk * delta_z;
			}

		}

		pthread_barrier_wait(&barrier); // ждём когда все потоки посчитают силы у своих пар тел

                int new_q = n / thread_count;
                int new_r = n % thread_count;

                int new_start = my_rank * new_q + (my_rank < new_r ? my_rank : new_r);
                int new_end = new_start + new_q + (my_rank < new_r ? 1 : 0);

                //printf("CURR NEW START = %d\n", new_start);
                //printf("CURR NEW END = %d\n", new_end);

                for (int i = new_start; i < new_end; i++){
                	for (int c = 0; c < 3; c++){
                        	double total_f = 0;
                                for (int j = 0; j < thread_count; j++){
                                	total_f += forces[(j * n + i) * 3 + c];
                                }

                                bodies_list[i].radius_list[c] += bodies_list[i].speed_list[c] * dt;
                                bodies_list[i].speed_list[c] += (total_f / bodies_list[i].mass) * dt;
                        }
                }

		pthread_barrier_wait(&barrier);

		if (my_rank == 0){
			fprintf(output_csv, "\n%lf", t);
			for (int i = 0; i < n; i++){
				fprintf(output_csv, ",%lf,%lf,%lf\n", bodies_list[i].radius_list[0],
					bodies_list[i].radius_list[1], bodies_list[i].radius_list[2]);
			}
		}

		t += dt;
	}
	return NULL;
}


int main(int argc, char** argv){

	if (argc < 4){
		return 1;
    	}

	// первый аргемент - t_end
	t_end = atof(argv[1]);

	// второй аргумент - количество потоков
	thread_count = atoi(argv[2]);

	// третий аргумент - файл
	char* file_name = argv[3];

	FILE* descriptor = fopen(file_name, "r");
	char bufer[100];
	int lines_counter = 0; // счётчик сколько строк прочитали
	int coords_count = 3;

	// считывание данных
	while (fgets(bufer, sizeof(bufer), descriptor)){
		if (lines_counter == 0)
		{
			sscanf(bufer, "%d", &n);
			bodies_list = malloc(n * sizeof(Body));
		}
		else
		{
			sscanf(bufer, "%lf %lf %lf %lf %lf %lf %lf", &bodies_list[lines_counter - 1].mass,
				&bodies_list[lines_counter - 1].radius_list[0], &bodies_list[lines_counter - 1].radius_list[1], &bodies_list[lines_counter - 1].radius_list[2],
				&bodies_list[lines_counter - 1].speed_list[0], &bodies_list[lines_counter - 1].speed_list[1], &bodies_list[lines_counter - 1].speed_list[2]);
		}

		lines_counter += 1;
	}

	fclose(descriptor);

	// итог по введённым данным
	printf("Given values:\nCount values: %d", n);
	for (int i = 0; i < n; i++)
	{
		printf("\nMass: %lf\nCoords: ", bodies_list[i].mass);
		for (int j = 0; j < coords_count; j++)
		{
			printf("%lf ", bodies_list[i].radius_list[j]);
		}
		printf("\nSpeed: ");
		for (int j = 0; j < coords_count; j++)
		{
			printf("%lf ", bodies_list[i].speed_list[j]);
		}
		printf("\n");
	}

	forces = malloc(n * thread_count * 3 * sizeof(double));

	pthread_barrier_init(&barrier, NULL, thread_count);

	double total_time;

	for (int i = 0; i < iteration_count; i++)
	{
		printf("\nIteration %d\n", i + 1);
		double start, end;

		// открытие файла
		output_csv = fopen("Trajectories.csv", "w");
		fprintf(output_csv, "t, ");
		for (int i = 0; i < n; i++){
			fprintf(output_csv, "x%d, y%d, z%d ", i + 1, i + 1, i + 1);
		}

		// сброс forces для новой итерации
		memset(forces, 0, n * thread_count * 3 * sizeof(double));

		// создание потоков и выполнение routine:
		pthread_t* thread_handles = malloc(thread_count * sizeof(pthread_t));
	
		GET_TIME(start);

		for (long i = 0; i < thread_count; ++i){
			pthread_create(&thread_handles[i], NULL, routine, (void*) i);
		}

		for (int i = 0; i < thread_count; ++i){
			pthread_join(thread_handles[i], NULL);
		}

		GET_TIME(end);
		total_time += (end - start);
		printf("Elapsed time = %.5f (sec)\n", end - start);

		fclose(output_csv);
		free(thread_handles);
	}

	printf("\nAverage time per %d iterations: %lf (sec)\n", iteration_count, total_time / iteration_count);

	// освобождение памяти
	pthread_barrier_destroy(&barrier);

	free(forces);
	free(bodies_list);

	return 0;
}


