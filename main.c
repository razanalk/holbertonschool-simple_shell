#include "shell.h"

/**
 * trim - Removes leading and trailing spaces
 * @str: String to trim
 * Return: Trimmed string
 */
char *trim(char *str)
{
	char *end;

	while (*str == ' ' || *str == '\t')
		str++;
	if (*str == '\0')
		return (str);
	end = str + strlen(str) - 1;
	while (end > str && (*end == ' ' || *end == '\t'))
		end--;
	*(end + 1) = '\0';
	return (str);
}

/**
 * split - Splits a string into an array of words
 * @str: String to split
 * Return: Array of strings
 */
char **split(char *str)
{
	char **argv;
	char *token;
	int i = 0;

	argv = malloc(sizeof(char *) * 64);
	if (!argv)
		return (NULL);

	token = strtok(str, " \t");
	while (token)
	{
		argv[i++] = token;
		token = strtok(NULL, " \t");
	}
	argv[i] = NULL;
	return (argv);
}

/**
 * main - Entry point for the simple shell
 * Return: Always 0
 */
int main(void)
{
	char *line = NULL;
	char *trimmed;
	char **argv;
	size_t len = 0;
	ssize_t read;
	pid_t pid;
	int status;

	while (1)
	{
		if (isatty(STDIN_FILENO))
			write(STDOUT_FILENO, "#cisfun$ ", 9);

		read = getline(&line, &len, stdin);
		if (read == -1)
		{
			free(line);
			exit(0);
		}

		line[read - 1] = '\0';
		trimmed = trim(line);

		if (trimmed[0] == '\0')
			continue;

		argv = split(trimmed);
		if (!argv)
			continue;

		pid = fork();
		if (pid == 0)
		{
			if (execve(argv[0], argv, environ) == -1)
			{
				perror("./shell");
				free(argv);
				free(line);
				exit(1);
			}
		}
		else
			waitpid(pid, &status, 0);

		free(argv);
	}

	free(line);
	return (0);
}
