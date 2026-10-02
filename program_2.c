#include <stdio.h>
#include <pthread.h>
#include <math.h>
#include <stdlib.h>

int thread_count;
const double g = 6.67430e-11;

double calc_fqk(double mk, double mq, double rk, double rq){
	double fqk = g * mq * mk * (rk - rq) / pow(fabs(rk - rq), 3);
       	return fqk;	
}

double sum_fq(int count, int index, double* mass_list, double* radius_list){
	double sum = 0;
	for(int i = 0; i < count; i++){
		if (i != index){
			sum += calc_fqk(mass_list[index], mass_list[i], radius_list[index], radius_list[i]);	
		}
	}
	return sum;
}

double euler(double t_end){
	double t = 0;
	while (t < t_end){
		
	}
}

void* routine(void* rank){
	long my_rank = (long)rank;

}

int main(int argc, char** argv){
	int n = 0;
	double t_end;
	double* mass_list;
	double** radius_list;
	double** speed_list;

	if (argc < 3){
		return 1;
    }
	
	t_end = atof(argv[1]);
	char* file_name = argv[2];
	FILE* descriptor = fopen(file_name, "r");
	char bufer[100];
	int lines_counter = 0;
	int coords_count =  2;

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
			sscanf(bufer, "%lf %lf %lf %lf %lf", &mass_list[lines_counter - 1]
				, &radius_list[lines_counter - 1][0], &radius_list[lines_counter - 1][1]
				, &speed_list[lines_counter - 1][0], &speed_list[lines_counter - 1][1]);
		}
		lines_counter += 1;
	}
	fclose(descriptor);

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

	free(mass_list);
	for (int i = 0; i < n; i++)
	{
		free(radius_list[i]);
		free(speed_list[i]);
	}
	free(radius_list);
	free(speed_list);
}
