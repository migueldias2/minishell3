
#include "../../inc/minishell.h"


int	here_doc(t_mini *mini, char *str)
{
	char		buffer[BUFFER_SIZE];
	int			fd;
	ssize_t		bytes_read;
	char		*num_str;
	char		filename[50];

	num_str = ft_itoa(mini->num);
	ft_strlcpy(filename, "temp", sizeof(filename));
	ft_strlcat(filename, num_str, sizeof(filename));
	free(num_str);
	mini->filenames[mini->num] = strdup(filename);
	mini->num++;
	fd = open(filename, O_CREAT | O_WRONLY | O_TRUNC, 0777);
	if (fd == -1)
		perror("error failed to open");
	while (1)
	{
		bytes_read = read(STDIN_FILENO, buffer, BUFFER_SIZE - 1);
		if (bytes_read < 0)
			perror("error read");
		if (bytes_read == 0)
			break ;
		buffer[bytes_read] = '\0';
		if (ft_strncmp(buffer, str, ft_strlen(str)) == 0 && \
	(buffer[ft_strlen(str)] == '\n' || buffer[ft_strlen(str)] == '\0'))
			break ;
		write(fd, buffer, bytes_read);
	}
	close(fd);
	fd = open(filename, O_RDONLY);
	/* dup2(fd, STDIN_FILENO);
	unlink("temp.txt"); */
	return (fd);
}
