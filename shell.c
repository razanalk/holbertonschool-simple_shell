#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

#define MAX_ARGS 64

extern char **environ;

/**
 * get_path - gets PATH variable
 * Return: PATH value or NULL
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
 * find_command - finds command in PATH
 * @command: command name
 * Return: full command path or NULL
 */
char *find_command(char *command)
{
	char *path, *path_copy, *dir;
	char *full_path;
	int len;

	if (strchr(command, '/'))
	{
		if (access(command, X_OK) == 0)
			return (strdup(command));

		return (NULL);
	}

	path = get_path();

	if (path == NULL || path[0] == '\0')
		return (NULL);

	path_copy = strdup(path);

	if (path_copy == NULL)
		return (NULL);

	dir = strtok(path_copy, ":");

	while (dir)
	{
		len = strlen(dir) + strlen(command) + 2;

		full_path = malloc(len);

		if (full_path == NULL)
		{
			free(path_copy);
			return (NULL);
		}

		sprintf(full_path, "%s/%s", dir, command);

		if (access(full_path, X_OK) == 0)
		{
			free(path_copy);
			return (full_path);
		}

		free(full_path);
		dir = strtok(NULL, ":");
	}

	free(path_copy);
	return (NULL);
}

/**
 * main - simple shell
 * Return: status
 */
int main(void)
{
	char *line = NULL;
	size_t len = 0;
	ssize_t read;
	char *args[MAX_ARGS];
	char *token;
	char *cmd_path;
	pid_t pid;
	int status;
	int i;

	status = 0;

	while (1)
	{
		if (isatty(STDIN_FILENO))
			printf(":) ");

		read = getline(&line, &len, stdin);

		if (read == -1)
		{
			free(line);
			exit(status);
		}

		if (line[read - 1] == '\n')
			line[read - 1] = '\0';

		i = 0;

		token = strtok(line, " \t");

		while (token && i < MAX_ARGS - 1)
		{
			args[i] = token;
			token = strtok(NULL, " \t");
			i++;
		}

		args[i] = NULL;

		if (args[0] == NULL)
			continue;

		cmd_path = find_command(args[0]);

		if (cmd_path == NULL)
		{
			fprintf(stderr, "./hsh: 1: %s: not found\n", args[0]);
			status = 127;
			continue;
		}

		pid = fork();

		if (pid == 0)
		{
			execve(cmd_path, args, environ);
			perror("./hsh");
			free(cmd_path);
			exit(EXIT_FAILURE);
		}
		else
		{
			wait(&status);

			if (WIFEXITED(status))
				status = WEXITSTATUS(status);
		}

		free(cmd_path);
	}

	free(line);

	return (status);
}
