#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <pthread.h>

struct SumArgs {
  int *array;
  int begin;
  int end;
};

// Функция для генерации массива
void GenerateArray(int *array, unsigned int array_size, unsigned int seed) {
    srand(seed);
    for (unsigned int i = 0; i < array_size; i++) {
        array[i] = rand() % 100; // Числа от 0 до 99
    }
}

int Sum(const struct SumArgs *args) {
  int sum = 0;
  for (int i = args->begin; i < args->end; i++) {
    sum += args->array[i];
  }
  return sum;
}

void *ThreadSum(void *args) {
  struct SumArgs *sum_args = (struct SumArgs *)args;
  return (void *)(size_t)Sum(sum_args);
}

int main(int argc, char **argv) {
  uint32_t threads_num = 4;
  uint32_t array_size = 1000000;
  uint32_t seed = 42;

  // Обработка аргументов командной строки
  for (int i = 1; i < argc; i++) {
    if (strcmp(argv[i], "--threads_num") == 0 && i + 1 < argc) {
      threads_num = atoi(argv[i + 1]);
      i++;
    } else if (strcmp(argv[i], "--array_size") == 0 && i + 1 < argc) {
      array_size = atoi(argv[i + 1]);
      i++;
    } else if (strcmp(argv[i], "--seed") == 0 && i + 1 < argc) {
      seed = atoi(argv[i + 1]);
      i++;
    }
  }

  printf("Threads: %u, Array size: %u, Seed: %u\n", threads_num, array_size, seed);

  // Выделение памяти для массива
  int *array = malloc(sizeof(int) * array_size);
  if (!array) {
    printf("Memory allocation failed!\n");
    return 1;
  }

  // Генерация массива
  GenerateArray(array, array_size, seed);

  // Замер времени начала
  struct timespec start_time, end_time;
  clock_gettime(CLOCK_MONOTONIC, &start_time);

  // Создание потоков
  pthread_t threads[threads_num];
  struct SumArgs args[threads_num];

  // Распределение работы между потоками
  int chunk_size = array_size / threads_num;
  for (uint32_t i = 0; i < threads_num; i++) {
    args[i].array = array;
    args[i].begin = i * chunk_size;
    args[i].end = (i == threads_num - 1) ? array_size : (i + 1) * chunk_size;
    
    if (pthread_create(&threads[i], NULL, ThreadSum, (void *)&args[i])) {
      printf("Error: pthread_create failed!\n");
      free(array);
      return 1;
    }
  }

  // Сбор результатов
  int total_sum = 0;
  for (uint32_t i = 0; i < threads_num; i++) {
    int *thread_sum = NULL;
    pthread_join(threads[i], (void **)&thread_sum);
    if (thread_sum) {
      total_sum += (int)(size_t)thread_sum;
    }
  }

  // Замер времени окончания
  clock_gettime(CLOCK_MONOTONIC, &end_time);
  
  double execution_time = (end_time.tv_sec - start_time.tv_sec) + 
                         (end_time.tv_nsec - start_time.tv_nsec) / 1000000000.0;

  // Проверка результата (последовательное вычисление для проверки)
  int sequential_sum = 0;
  for (uint32_t i = 0; i < array_size; i++) {
    sequential_sum += array[i];
  }

  free(array);
  
  printf("Parallel sum: %d\n", total_sum);
  printf("Sequential sum: %d\n", sequential_sum);
  printf("Execution time: %.6f seconds\n", execution_time);
  printf("Results match: %s\n", total_sum == sequential_sum ? "YES" : "NO");
  
  return 0;
}
