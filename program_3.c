#include <stdio.h>

typedef struct {
	pthread_mutex_t mutex; // мьютекс
	pthread_cond_t for_read; // условная переменная для читателей
	pthread_cond_t for_write; // условная переменная для писателей
	int count_readers; // счетчик читателей (сколько потоков в данный момент читают)
	int count_wait_read; // счётчик потоков, ожидающих получения блокировки на чтение
	int count_wait_write; // счётчик потоков, ожидающих получения блокировки на запись
	int f; // флаг получил ли блокировку хотя бы один писатель в данный момент
} My_struct;

int my_rwlock_init(My_struct *my_rwlock){
	pthread_mutex_init(&my_rwlock.mutex, NULL);
	pthread_cond_init(&my_rwlock.for_read, NULL);
	pthread_cond_init(&my_rwlock.for_write, NULL);

	My_struct.count_readers = 0;
	My_struct.count_wait_read = 0;
	My_struct.count_wait_write = 0;
	My_struct.f = 0;

	return 0;
}

int my_rwlock_destroy(My_struct *my_rwlock){
	pthread_mutex_destroy(&my_rwlock.mutex);

	pthread_cond_destroy((&my_rwlock.for_read);
	pthread_cond_destroy(&my_rwlock.for_write);

	return 0;
}

int my_rwlock_rdlock(My_struct *my_rwlock){

}

int my_rwlock_wrlock(My_struct *my_rwlock){

}

int my_rwlock_unlock(My_struct *my_rwlock){

}

int main(){
	My_struct my_rwlock; // экземпляр структуры
	
	my_rwlock_init(&my_rwlock); // вызов функции
}
