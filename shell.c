#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

#define MAX_ARGS 64

/**
 * main - simple shell
 * Return: 0
 */
int main(void)
{
	char *line = NULL;
	size_t len = 0;
	ssize_t read;
	pid_t pid;
	int status;
	char *args[MAX_ARGS];
	char *token;
	int i;

	while (1)
	{
		if (isatty(STDIN_FILENO))
			printf(":) ");

		read = getline(&line, &len, stdin);

		if (read == -1)
		{
			free(line);
			exit(0);
		}

		if (line[read - 1] == '\n')
			line[read - 1] = '\0';

		i = 0;
		token = strtok(line, " ");

		while (token != NULL && i < MAX_ARGS - 1)
		{
			args[i] = token;
			token = strtok(NULL, " ");
			i++;
		}

		args[i] = NULL;

		if (args[0] == NULL)
			continue;

		pid = fork();

		if (pid == 0)
		{
			if (execvp(args[0], args) == -1)
			{
				perror("./hsh");
			}
			exit(EXIT_FAILURE);
		}
		else
		{
			wait(&status);
		}
	}

	return (0);
}
