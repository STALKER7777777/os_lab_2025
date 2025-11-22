#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main() {
    printf("Creating a zombie process...\n");
    
    pid_t pid = fork();
    
    if (pid == 0) {
        // Дочерний процесс
        printf("Child process (PID: %d) started\n", getpid());
        printf("Child process exiting immediately\n");
        exit(0);
    } else {
        // Родительский процесс
        printf("Parent process (PID: %d) created child (PID: %d)\n", getpid(), pid);
        printf("Parent process sleeping for 30 seconds - check 'ps aux | grep defunct'\n");
        sleep(30); // В этот момент дочерний процесс становится зомби
        
        printf("Parent process now waiting for child to remove zombie\n");
        wait(NULL); // Убираем зомби
        printf("Zombie process cleaned up\n");
        sleep(10); // Даем время убедиться, что зомби исчез
    }
    
    return 0;
}
