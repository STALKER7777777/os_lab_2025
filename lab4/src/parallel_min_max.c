#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <signal.h>
#include <errno.h>

volatile sig_atomic_t timeout_occurred = 0;

void timeout_handler(int sig) {
    timeout_occurred = 1;
}

int main(int argc, char *argv[]) {
    int timeout = 0; // 0 означает отсутствие таймаута
    
    // Обработка аргументов командной строки
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--timeout") == 0 && i + 1 < argc) {
            timeout = atoi(argv[i + 1]);
            i++;
        }
    }
    
    // Установка обработчика сигнала SIGALRM
    if (timeout > 0) {
        signal(SIGALRM, timeout_handler);
        alarm(timeout);
    }
    
    // Создание дочерних процессов (ваш существующий код)
    pid_t pid1 = fork();
    if (pid1 == 0) {
        // Дочерний процесс 1 - поиск минимума
        // Ваш существующий код для поиска минимума
        sleep(15); // Имитация долгой работы для теста
        exit(0);
    }
    
    pid_t pid2 = fork();
    if (pid2 == 0) {
        // Дочерний процесс 2 - поиск максимума
        // Ваш существующий код для поиска максимума
        sleep(15); // Имитация долгой работы для теста
        exit(0);
    }
    
    // Родительский процесс
    int status;
    pid_t finished_pid;
    
    while (1) {
        // Неблокирующее ожидание
        finished_pid = waitpid(-1, &status, WNOHANG);
        
        if (finished_pid == -1) {
            // Все дочерние процессы завершились
            if (errno == ECHILD) {
                break;
            }
        } else if (finished_pid == 0) {
            // Дочерние процессы еще работают
            if (timeout_occurred) {
                printf("Timeout occurred! Killing child processes...\n");
                kill(pid1, SIGKILL);
                kill(pid2, SIGKILL);
                break;
            }
            usleep(100000); // Пауза 100ms перед следующей проверкой
        } else {
            // Один из дочерних процессов завершился
            printf("Child process %d finished\n", finished_pid);
        }
    }
    
    // Ожидание завершения всех дочерних процессов
    while (waitpid(-1, NULL, WNOHANG) > 0) {
        // Очищаем оставшиеся процессы
    }
    
    return 0;
}
