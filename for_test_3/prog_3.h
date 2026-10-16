#include <stdio.h>
#include <pthread.h>

typedef struct {
        pthread_mutex_t mutex; // мьютекс
        pthread_cond_t for_read; // условная переменная для читателей
        pthread_cond_t for_write; // условная переменная для писателей
        int count_readers; // счетчик читателей (сколько потоков в данный момент читают)
        int waiting_readers; // счётчик потоков, ожидающих получения блокировки на чтение
        int waiting_writers; // счётчик потоков, ожидающих получения блокировки на запись
        int f; // флаг получил ли блокировку хотя бы один писатель в данный момент
} My_struct;


int my_rwlock_init(My_struct *my_rwlock);
int my_rwlock_destroy(My_struct *my_rwlock);
int my_rwlock_rdlock(My_struct *my_rwlock);
int my_rwlock_wrlock(My_struct *my_rwlock);
int my_rwlock_unlock(My_struct *my_rwlock);
