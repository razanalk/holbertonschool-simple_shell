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
 * main - Entry point for the simple shell
 * Return: Always 0
 */
int main(void)
{
	char *line = NULL;
	char *trimmed;
	size_t len = 0;
	ssize_t read;
	char *argv[3];
	pid_t pid;
	int status;
	char *space;

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

		space = strchr(trimmed, ' ');
		if (space)
		{
			*space = '\0';
			argv[0] = trimmed;
			argv[1] = trim(space + 1);
			argv[2] = NULL;
		}
		else
		{
			argv[0] = trimmed;
			argv[1] = NULL;
			argv[2] = NULL;
		}

		pid = fork();
		if (pid == 0)
		{
			if (execve(argv[0], argv, environ) == -1)
			{
				perror("./shell");
				free(line);
				exit(1);
			}
		}
		else
			waitpid(pid, &status, 0);
	}

	free(line);
	return (0);
}
