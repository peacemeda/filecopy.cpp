#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <iostream>
#include <fstream>

#define BUFFER_SIZE 25

int main(int argc, char *argv[]) {
    
    char buffer[BUFFER_SIZE];
    ssize_t bytes_read;

    // Check if the correct number of arguments is provided
    if (argc != 3) {
        fprintf(stderr, "program: %s <sourceFile> <destinationFile>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // Open source file for If not created creat 
    int sourceFile = open(argv[1], O_RDONLY);
    if (sourceFile == -1) {
        perror("open");
        exit(EXIT_FAILURE);
    }

    // Open destination file for writing, 
    //create file if doesn't exist, trunc the size to 0 permission 
    int destinationFile = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (destinationFile == -1) {
        perror("open");
        exit(EXIT_FAILURE);
    }
    
    int pipe_fd[2];
    pid_t pid;
    // To Create pipe we call pipe() method
    if (pipe(pipe_fd) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    // To creat a child process
    pid = fork();
    if (pid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    }
    // Child process and close the child proces
    if (pid == 0) { 
        close(pipe_fd[1]);
        // Read from the pipe and write to destination file
        while ((bytes_read = read(pipe_fd[0], buffer, BUFFER_SIZE)) > 0) {
            if (write(destinationFile, buffer, bytes_read) != bytes_read) {
                perror("write");
                exit(EXIT_FAILURE);
            }
        }

        // Close read end of the pipe
        close(pipe_fd[0]);
        close(destinationFile);
        exit(EXIT_SUCCESS);
    } else { 
        close(pipe_fd[0]);

        // Read from source file and write to the pipe
        while ((bytes_read = read(sourceFile, buffer, BUFFER_SIZE)) > 0) {
            if (write(pipe_fd[1], buffer, bytes_read) != bytes_read) {
                perror("write");
                exit(EXIT_FAILURE);
            }
        }

        // Close write end of the pipe
        close(pipe_fd[1]);
        close(sourceFile);

        // Wait for child process to finish
        wait(NULL);
        exit(EXIT_SUCCESS);
    }

    return 0;
}
