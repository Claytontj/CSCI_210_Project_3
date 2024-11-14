#include <stdio.h>
#include <stdlib.h>
#include <spawn.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>

#define MAX_ARGS 20
#define BUFFER_SIZE 256
#define ALLOWED_COMMANDS 12

extern char **environ;

// List of allowed commands
const char *allowed[ALLOWED_COMMANDS] = {
    "cp", "touch", "mkdir", "ls", "pwd", "cat", "grep", "chmod", "diff", "cd", "exit", "help"};

// Function to check if a command is allowed
int isAllowed(const char *cmd)
{
    for (size_t i = 0; i < ALLOWED_COMMANDS; i++)
    {
        if (strcmp(cmd, allowed[i]) == 0)
        {
            return 1;
        }
    }
    return 0;
}

// Function to print the help message
void print_help()
{
    printf("The allowed commands are:\n");
    for (int i = 0; i < ALLOWED_COMMANDS; i++)
    {
        printf("%d: %s\n", i + 1, allowed[i]);
    }
}

int main()
{
    char line[BUFFER_SIZE];
    char *args[MAX_ARGS + 1];

    while (1)
    {
        // Display prompt
        fprintf(stderr, "rsh> ");

        // Read input from user
        if (!fgets(line, sizeof(line), stdin))
        {
            continue;
        }

        // Remove trailing newline
        line[strlen(line) - 1] = '\0';

        // Tokenize input line into command and arguments
        int argc = 0;
        char *token = strtok(line, " ");
        while (token != NULL && argc < MAX_ARGS)
        {
            args[argc++] = token;
            token = strtok(NULL, " ");
        }
        args[argc] = NULL; // Null-terminate argument list

        // If no command is entered, continue
        if (argc == 0)
            continue;

        // Check if command is allowed
        if (!isAllowed(args[0]))
        {
            printf("NOT ALLOWED!\n");
            continue;
        }

        // Handle built-in commands
        if (strcmp(args[0], "exit") == 0)
        {
            break;
        }
        if (strcmp(args[0], "help") == 0)
        {
            print_help();
            continue;
        }
        if (strcmp(args[0], "cd") == 0)
        {
            if (argc > 2)
            {
                printf("-rsh: cd: too many arguments\n");
            }
            else if (argc == 2)
            {
                if (chdir(args[1]) != 0)
                {
                    perror("cd");
                }
            }
            continue;
        }

        // For allowed external commands, use posix_spawnp to execute
        pid_t pid;
        int status;
        if (posix_spawnp(&pid, args[0], NULL, NULL, args, environ) == 0)
        {
            if (waitpid(pid, &status, 0) == -1)
            {
                perror("waitpid");
            }
        }
        else
        {
            printf("Failed to execute %s\n", args[0]);
        }
    }

    return 0;
}