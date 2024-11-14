#include <stdio.h>
#include <stdlib.h>
#include <spawn.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>

#define ALLOWED_COMMANDS 12 // Number of allowed commands

extern char **environ; // Environment variable for posix_spawnp

// List of allowed commands for the restricted shell
char *allowed_commands[ALLOWED_COMMANDS] = {
	"cp", "touch", "mkdir", "ls", "pwd", "cat",
	"grep", "chmod", "diff", "cd", "exit", "help"};

// Function to check if a command is allowed
// Returns 1 if the command is allowed, otherwise 0
int is_command_allowed(const char *command)
{
	for (size_t i = 0; i < ALLOWED_COMMANDS; i++)
	{
		if (strcmp(command, allowed_commands[i]) == 0)
		{
			return 1;
		}
	}
	return 0;
}

int main()
{
	char input_line[256]; // Buffer to hold input line from the user
	char *arguments[21];  // Array to store command and arguments (20 args max + NULL terminator)

	// Infinite loop for the shell prompt until "exit" command is run
	while (1)
	{
		// Display prompt
		fprintf(stderr, "rsh> ");

		// Read input line from user
		if (fgets(input_line, sizeof(input_line), stdin) == NULL)
			continue;

		// Ignore empty lines
		if (strcmp(input_line, "\n") == 0)
			continue;

		// Remove newline character at the end of the input line
		input_line[strlen(input_line) - 1] = '\0';

		// Tokenize the input line to separate command and arguments
		int arg_count = 0;
		char *token = strtok(input_line, " ");
		while (token != NULL && arg_count < 20)
		{
			arguments[arg_count++] = token;
			token = strtok(NULL, " ");
		}
		arguments[arg_count] = NULL; // Null-terminate the arguments list

		// Ensure command is not empty
		if (arg_count == 0)
			continue;

		// Check if command is allowed
		if (!is_command_allowed(arguments[0]))
		{
			printf("NOT ALLOWED!\n");
			continue;
		}

		// Handle built-in commands
		if (strcmp(arguments[0], "exit") == 0)
		{
			break; // Exit the shell
		}

		if (strcmp(arguments[0], "help") == 0)
		{
			// Display the list of allowed commands
			printf("The allowed commands are:\n");
			for (int i = 0; i < ALLOWED_COMMANDS; i++)
			{
				printf("%d: %s\n", i + 1, allowed_commands[i]);
			}
			continue;
		}

		if (strcmp(arguments[0], "cd") == 0)
		{
			// Handle the "cd" command with argument check
			if (arg_count > 2)
			{
				printf("-rsh: cd: too many arguments\n");
			}
			else if (arg_count == 2)
			{
				if (chdir(arguments[1]) != 0)
				{
					perror("cd");
				}
			}
			continue;
		}

		// Spawn a process for external commands (first 9 commands)
		pid_t child_pid;
		int spawn_status = posix_spawnp(&child_pid, arguments[0], NULL, NULL, arguments, environ);
		if (spawn_status == 0)
		{
			// Wait for child process to complete if spawn was successful
			if (waitpid(child_pid, NULL, 0) == -1)
			{
				perror("waitpid");
			}
		}
		else
		{
			// Error message if spawning the process failed
			printf("Failed to execute %s\n", arguments[0]);
		}
	}

	return 0;
}