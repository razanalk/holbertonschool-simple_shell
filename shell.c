#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

/**
 * get_path - gets PATH environment variable
 * Return: PATH value
 */
char *get_path(void)
{
	extern char **environ;
	int i = 0;

	while (environ[i])
	{
		if (strncmp(environ[i], "PATH=", 5) == 0)
			return (environ[i] + 5);
		i++;
	}

	return (NULL);
}

/**
 * execute_command - executes command
 * @command: command input
 * Return: nothing
 */
void execute_command(char *command)
{
	char *path, *path_copy, *dir;
	char full_path[1024];
	char *argv[] = {command, NULL};
	pid_t pid;

	if (access(command, X_OK) == 0)
	{
		pid = fork();

		if (pid == 0)
		{
			execve(command, argv, NULL);
			perror("./hsh");
			exit(1);
		}
		else
		{
			wait(NULL);
		}

		return;
	}

	path = get_path();

	if (!path)
		return;

	path_copy = strdup(path);

	dir = strtok(path_copy, ":");

	while (dir)
	{
		snprintf(full_path, sizeof(full_path), "%s/%s", dir, command);

		if (access(full_path, X_OK) == 0)
		{
			pid = fork();

			if (pid == 0)
			{
				execve(full_path, argv, NULL);
				perror("./hsh");
				exit(1);
			}
			else
			{
				wait(NULL);
			}

			free(path_copy);
			return;
		}

		dir = strtok(NULL, ":");
	}

	perror("./hsh");
	free(path_copy);
}

/**
 * main - simple shell
 * Return: 0
 */
int main(void)
{
	char *line = NULL;
	size_t len = 0;

	while (1)
	{
		printf(":) ");
		fflush(stdout);

		if (getline(&line, &len, stdin) == -1)
		{
			free(line);
			exit(0);
		}

		line[strcspn(line, "\n")] = '\0';

		if (strlen(line) == 0)
			continue;

		execute_command(line);
	}

	free(line);

	return (0);
}
