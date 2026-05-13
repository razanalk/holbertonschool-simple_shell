#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/stat.h>

#define MAX_ARGS 64

extern char **environ;

/**
 * get_path - get PATH from environment
 * Return: PATH value
 */
char *get_path(void)
{
	int i;

	for (i = 0; environ[i]; i++)
	{
		if (strncmp(environ[i], "PATH=", 5) == 0)
			return (environ[i] + 5);
	}

	return (NULL);
}

/**
 * main - simple shell
 * Return: 0
 */
int main(void)
{
	char *line = NULL;
	size_t len = 0;
	ssize_t read;
	char *args[MAX_ARGS];
	char *token;
	int i;
	pid_t pid;
	int status;
	char *path;
	char *path_copy;
	char *dir;
	char full_path[1024];
	int found;

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
		token = strtok(line, " \t");

		while (token != NULL && i < MAX_ARGS - 1)
		{
			args[i] = token;
			token = strtok(NULL, " \t");
			i++;
		}

		args[i] = NULL;

		if (args[0] == NULL)
			continue;

		found = 0;

		if (strchr(args[0], '/'))
		{
			if (access(args[0], X_OK) == 0)
				found = 1;
		}
		else
		{
			path = get_path();

			if (path != NULL && path[0] != '\0')
			{
				path_copy = strdup(path);

				if (path_copy == NULL)
					continue;

				dir = strtok(path_copy, ":");

				while (dir != NULL)
				{
					sprintf(full_path, "%s/%s", dir, args[0]);

					if (access(full_path, X_OK) == 0)
					{
						found = 1;
						args[0] = full_path;
						break;
					}

					dir = strtok(NULL, ":");
				}

				free(path_copy);
			}
		}

		if (!found)
		{
			fprintf(stderr, "./hsh: 1: %s: not found\n", args[0]);
			continue;
		}

		pid = fork();

		if (pid == 0)
		{
			execve(args[0], args, environ);
			perror("./hsh");
			exit(EXIT_FAILURE);
		}
		else
		{
			wait(&status);
		}
	}

	free(line);
	return (0);
}
