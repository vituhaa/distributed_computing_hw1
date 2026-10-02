#include <stdio.h>
#include <pthread.h>
#include <math.h>

int thread_count;
const double g = 6.67430e-11;

double calc_fqk(double mk, double mq, double rk, double rq, int index){
	double fqk = g * mq * mk * (rk - rq) / pow(fabs(rk - rq), 3);
       	return fqk;	
}

double sum_fq(int count, double* mass_list, double* radius_list){
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
	int n;
	double t_end;
	double mass_list[];
	double radius_list[];
	double coords[][];
	double speed[][];

	if (argc < 3){
                return 1;
        }
	
	t_end = atof(agrv[1]);
	char* file_name = argv[2];
	FILE* descriptor = fopen(file_name, "r");
	char bufer[100];
	while (fgets(bufer, sizeof(bufer), descriptor)){
		sscanf(bufer, "%lf %lf %lf %lf %lf", );
	}
	fclose(descriptor);
}
