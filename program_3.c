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

// инициализация
int my_rwlock_init(My_struct* my_rwlock) {
    pthread_mutex_init(&my_rwlock->mutex, NULL);
    pthread_cond_init(&my_rwlock->for_read, NULL);
    pthread_cond_init(&my_rwlock->for_write, NULL);

    my_rwlock->count_readers = 0;
    my_rwlock->waiting_readers = 0;
    my_rwlock->waiting_writers = 0;
    my_rwlock->f = 0;

    return 0;
}

int my_rwlock_destroy(My_struct* my_rwlock) {
    pthread_mutex_destroy(&my_rwlock->mutex);
    pthread_cond_destroy(&my_rwlock->for_read);
    pthread_cond_destroy(&my_rwlock->for_write);

    return 0;
}

// несколько потоков могут читать, но только один - писать
int my_rwlock_rdlock(My_struct* my_rwlock) {
    // printf("thread %lu: in rdlock\n", (unsigned long)pthread_self()); // идентификатор для отладки

    pthread_mutex_lock(&my_rwlock->mutex);

    my_rwlock->waiting_readers++; // добавляется ожидающий читатель
    while (my_rwlock->f || my_rwlock->waiting_writers) {
	// printf("thread %lu: waiting for read\n", (unsigned long)pthread_self());

	pthread_cond_wait(&my_rwlock->for_read, &my_rwlock->mutex); // блокируем поток, освобождаем мьютекс

	// printf("thread %lu: woke up\n", (unsigned long)pthread_self());
    }
    my_rwlock->waiting_readers--; // из ожидания переводим в чтение
    my_rwlock->count_readers++; // добавляем читателя в активного

    pthread_mutex_unlock(&my_rwlock->mutex);

    return 0;
}

int my_rwlock_wrlock(My_struct* my_rwlock) {
    pthread_mutex_lock(&my_rwlock->mutex);

    my_rwlock->waiting_writers++; // добавляем ожидающего писателя

    while (my_rwlock->count_readers || my_rwlock->f) { // если есть активные читатели или какой-то писатель получил блокировку

        pthread_cond_wait(&my_rwlock->for_write, &my_rwlock->mutex); // новому писателю закрываем доступ, пока другой писатель/читатель не уйдёт
    }

    my_rwlock->waiting_writers--;
    my_rwlock->f = 1;

    pthread_mutex_unlock(&my_rwlock->mutex);

    return 0;
}

int my_rwlock_unlock(My_struct* my_rwlock) {
    pthread_mutex_lock(&my_rwlock->mutex);

    if (my_rwlock->f) {
        my_rwlock->f = 0; // если писатель держал доступ - отпускаем
    }
    else {
        my_rwlock->count_readers--; // если читатель - отпускаем
    }


    if (my_rwlock->count_readers == 0 && my_rwlock->waiting_writers > 0) { // если читателей больше нет, а ожидающие писатели есть
        pthread_cond_signal(&my_rwlock->for_write); // будим одного ожидающего писателя забирать данные
    }
    else if (my_rwlock->f == 0 && my_rwlock->waiting_writers == 0) { // если активных и ожидающих писателей нет
        pthread_cond_broadcast(&my_rwlock->for_read); // будим всех читателей
    }

    pthread_mutex_unlock(&my_rwlock->mutex);

    return 0;
}

