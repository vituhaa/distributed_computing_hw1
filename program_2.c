#include <stdio.h>
#include <pthread.h>
#include <math.h>
#include <stdlib.h>

int thread_count;
const double g = 6.67430e-11;

struct output{
	double x, y, z;
};

// сила на тело q со стороны тела k
double calc_fqk(double mk, double mq, double r2, double delta){
        if (r2 == 0){
        	printf("Деление на ноль\n");
        }

	double denominator = pow(fabs(r2), 1.5);
	double fqk = g * mq * mk * delta / denominator;
	
       	return fqk;	
}


void* routine(void* rank){
        long my_rank = (long)rank;
        int my_count = N / thread_count; // количество блоков МТ

        int start = my_rank * my_count;
        int end = start + my_count;

	double t = 0;
	double delta_t = 0.5; // задать вручную
	
	while (t <= t_end){
		for (int q = 0; q < N; q++){
			f_x[q] = 0;
                        f_y[q] = 0;
                        f_z[q] = 0;
		}
		for (int q = 0; q < N; q++){
			for (int k = q + 1; q < N; k++){
				delta_x = x[k] - x[q]; 
				delta_y = y[k] - y[q];
				delta_z = z[k] - z[q];

				double r2 = pow(delta_x, 2) + pow(delta_y, 2) + pow(delta_z, 2)

				f_xqk = calc_fqk(m[k], m[q], r2, delta_x);
				f_yqk = calc_fqk(m[k], m[q], r2, delta_y);
				f_zqk = calc_fqk(m[k], m[q], r2, delta_z);

				// третий з-н ньютона
				f_x[q] = f_x[q] + f_xqk;
				f_y[q] = f_y[q] + f_yqk;
				f_z[q] = f_z[q] + f_zqk;

				f_x[k] = f_x[k] - f_xqk;
				f_y[k] = f_y[k] - f_yqk;
				f_z[k] = f_z[k] - f_zqk;
			}

			x[q] = x[q] + vx[q] * delta_t;
			y[q] = y[q] + vy[q] * delta_t;
			z[q] = z[q] + vz[q] * delta_t;

			vx[q] = vx[q] + (f_x[q] / m[q]) * delta_t;
			vy[q] = vy[q] + (f_y[q] / m[q]) * delta_t;
			vz[q] = vz[q] + (f_z[q] / m[q]) * delta_t;

			res.append(t, x[q], y[q], z[q]);
		}

		t += delta_t;
	}
}


int main(int argc, char** argv){
	int n = 0;
	double t_end;
	double* mass_list;
	double** radius_list; // двумерный массив радиус-векторов
	double** speed_list; // двумерный массив скоростей

	if (argc < 3){
		return 1;
    	}
	
	// первый аргемент - t_end
	t_end = atof(argv[1]);

	// второй аргумент - количество потоков
	thread_count = argv[2]

	// третий аргумент - файл
	char* file_name = argv[2];
	
	FILE* descriptor = fopen(file_name, "r");
	char bufer[100];
	int lines_counter = 0;
	int coords_count =  3;

	// считывание данных
	while (fgets(bufer, sizeof(bufer), descriptor)){
		if (lines_counter == 0)
		{
			sscanf(bufer, "%d", &n);
			mass_list = malloc(n * sizeof(double));
			radius_list = malloc(n * sizeof(double*));
			speed_list = malloc(n * sizeof(double*));
			for (int i = 0; i < n; i++)
			{
				radius_list[i] = malloc(coords_count * sizeof(double));
				speed_list[i] = malloc(coords_count * sizeof(double));
			}
		}
		else
		{
			sscanf(bufer, "%lf %lf %lf %lf %lf %lf %lf", &mass_list[lines_counter - 1],
				&radius_list[lines_counter - 1][0], &radius_list[lines_counter - 1][1], &radius_list[lines_counter - 1][2],
				&speed_list[lines_counter - 1][0], &speed_list[lines_counter - 1][1], &speed_list[lines_counter - 1][2]);
		}
		lines_counter += 1;
	}

	fclose(descriptor);

	// итог по введённым данным
	printf("Given values:\nCount values: %d", n);
	for (int i = 0; i < n; i++)
	{
		printf("\nMass: %lf\nCoords: ", mass_list[i]);
		for (int j = 0; j < coords_count; j++)
		{
			printf("%lf ", radius_list[i][j]);
		}
		printf("\nSpeed: ");
		for (int j = 0; j < coords_count; j++)
		{
			printf("%lf ", speed_list[i][j]);
		}
		printf("\n");
	}

	// создание потоков и выполнение routine:
	pthread_t* thread_handles = malloc(thread_count * sizeof(pthread_t));

	for (long i = 0; i < thread_count; ++i){
		pthread_create(&thread_handles[i], NULL, routine, (void*) i);
	}

	for (int i = 0; i < thread_count; ++i){
		pthread_join(thread_handles[i], NULL);
	}

	// освобождение памяти
	free(mass_list);
	for (int i = 0; i < n; i++)
	{
		free(radius_list[i]);
		free(speed_list[i]);
	}
	free(radius_list);
	free(speed_list);
	free(thread_handles);

	// запись в файл (дописать)
	FILE* output_csv = fopen("Trajectories.csv", "w");
	
	while (t <= t_end){
		fprintf(output_csv, "%lf", t);
		for (int i = 0; i < n; i++){
			fprintf(output_csv, "%lf %lf %lf", x, y, z); // дописать откуда берутся точки
		}
	}

	return 0;
}
