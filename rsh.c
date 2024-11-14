#include <stdio.h>
#include <stdlib.h>
#include <spawn.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>

#define N 12

extern char **environ;

char *allowed[N] = {"cp", "touch", "mkdir", "ls", "pwd", "cat", "grep", "chmod", "diff", "cd", "exit", "help"};

int isAllowed(const char *cmd)
{
	// TODO
	// return 1 if cmd is one of the allowed commands
	// return 0 otherwise
	for (size_t i = 0; i < 12; i++)
	{
		if (strcmp(cmd, allowed[i]) == 0)
		{
			return 0;
		}
	}

	return 1;
}

int main()
{

	// TODO
	// Add variables as needed

	char line[256];
	char *argv[21];

	// infinite loop till exit command is run...
	while (1)
	{

		fprintf(stderr, "rsh>");

		if (fgets(line, 256, stdin) == NULL)
			continue;

		if (strcmp(line, "\n") == 0)
			continue;

		line[strlen(line) - 1] = '\0';

		// tokenizing the input line into the respective command and its arguments
		int argc = 0;
		char *token = strtok(line, " ");
		while (token != NULL && argc < 20)
		{
			argv[argc++] = token;
			token = strtok(NULL, " ");
		}
		// add a null termination to the argument list
		argv[argc] = NULL;

		// make sure the command isn't empty
		if (argc == 0)
		{
			continue;
		}

		// check if the command is allowed..
		if (isAllowed(line) == 1)
		{
			printf("NOT ALLOWED!\n");
			continue;
		}
		// if we make it here, we know that the command is an allowed command...now just run the correct command

		// TODO
		// And add code to execute cd, exit, help commands
		if (strcmp(line, "exit") == 0)
		{
			break;
		}

		if (strcmp(line, "help") == 0)
		{
			// print out all the commands...
			printf("The allowed commands are:\n");
			printf("1: cp\n");
			printf("2: touch\n");
			printf("3: mkdir\n");
			printf("4: ls\n");
			printf("5: pwd\n");
			printf("6: cat\n");
			printf("7: grep\n");
			printf("8: chmod\n");
			printf("9: diff\n");
			printf("10: cd\n");
			printf("11: exit\n");
			printf("12: help\n");
			continue;
		}

		if (strcmp(argv[0], "cd") == 0)
		{
			if (argc > 2)
			{
				printf("-rsh: cd: too many arguments\n");
			}
			else if (argc == 2)
			{
				if (chdir(argv[1]) != 0)
				{
					perror("cd");
				}
			}
			continue;
		}

		// Add code to spawn processes for the first 9 commands
		int status;
		pid_t pid;
		status = posix_spawnp(&pid, argv[0], NULL, NULL, argv, environ);
		if (status == 0)
		{
			// if successful, wait for the process to complete
			if (waitpid(pid, &status, 0) == -1)
			{
				perror("waitpid");
			}
		}
		else
		{
			// if failed, just print error message
			printf("Failed to execute %s\n", argv[0]);
		}
	}

	return 0;
}