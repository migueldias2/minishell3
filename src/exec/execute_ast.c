
#include "../../inc/minishell.h"



void	exec(t_node *cur, t_mini *mini)
{
	char	*path;

	if (!cur->cmd)
		exit(EXIT_FAILURE);
	path = get_path(cur->cmd, mini->env); // fazer o get path com environment global variable
	if (!path)
	{
		printf("%s: Command not found\n", cur->cmd);
		exit(EXIT_FAILURE);
	}
	execve(path, cur->args, mini->env);
	perror("execve error");
	free(path);
	exit(EXIT_FAILURE);
}

void	ft_close_all_fds(t_mini *mini)
{
	int	fd;

	(void)mini;
	fd = 3;
	while (fd < 1024)
	{
		close(fd);
		fd++;
	}
}

void	do_command(t_node *cur, t_mini *mini)
{
	pid_t	pid;

	pid = fork();
	if (pid == -1)
	{
		perror("error fork");
    	exit(EXIT_FAILURE);
	}
	if (pid == 0)
	{
		if (cur->infile != 0)
		{
			dup2(cur->infile, STDIN_FILENO);
			close(cur->infile);
		}
		if (cur->outfile != 1)
		{
			dup2(cur->outfile, STDOUT_FILENO);
			close(cur->outfile);
		}
		exec(cur, mini);
	}
	else
		wait(NULL);
}

void	handle_child(t_node *cur, t_mini *mini, int *i, int *infd)
{
	if (*i != 0)
	{
		dup2(*infd, STDIN_FILENO);
		close(*infd);
	}
	if (*i != mini->nr_pipes)
		dup2(mini->pipe_fd[1], STDOUT_FILENO);
	close(mini->pipe_fd[0]);
	close(mini->pipe_fd[1]);
	if (cur->infile != 0)
	{
		dup2(cur->infile, STDIN_FILENO);
		close(cur->infile);
	}
	if (cur->outfile != 1)
	{
		dup2(cur->outfile, STDOUT_FILENO);
		close(cur->outfile);
	}
	ft_close_all_fds(mini);
	exec(cur, mini);
	exit(EXIT_FAILURE);
}

void	do_pipe(t_node *cur, t_mini *mini, int *i, int *infd)
{
	pid_t	pid;

	pid = fork();
	if (pid == -1)
	{
		perror("error fork");
    	exit(EXIT_FAILURE);
	}
	if (pid == 0)
		handle_child(cur, mini, i, infd);
	else
	{
		mini->pids[*i] = pid;
		if (*infd != STDIN_FILENO)
			close(*infd);
		*infd = mini->pipe_fd[0];
		if (cur->infile != 0)
			close(cur->infile);
		if (cur->outfile != 1)
			close(cur->outfile);
		close(mini->pipe_fd[1]);
	}
}

void	execute(t_node *sliced_tokens_list, t_mini *mini)
{
	t_node	*cur;
	int				i;
	int				j;
	int				infd;

	infd = STDIN_FILENO;
	i = 0;
	cur = sliced_tokens_list;
	while (cur)
	{
		if (pipe(mini->pipe_fd) == -1)
			perror("error pipe");
		do_pipe(cur, mini, &i, &infd);
		cur = cur->next;
		i++;
	}
	ft_close_all_fds(mini);
	j = 0;
	while (j <= i)
	{
		if (mini->pids[j] != -1)
			waitpid(mini->pids[j], NULL, 0);
		j++;
	}
}
