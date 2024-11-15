
#include "../../inc/minishell.h"


extern char **environ;

void	exec(t_ast_node *node, t_mini *mini)
{
	(void)mini;
	char	*path;

	/* if (ft_strncmp(node->cmd, "cd", 2) == 0 && !node->cmd[2])
		ft_cd(node, mini); */
	path = get_path(node->cmd); // fazer o get path com environment global variable
	if (!path)
	{
		/* perror("Command not found"); */
		exit(EXIT_FAILURE);
	}
	execve(path, node->args, environ);
	perror("execve error");
	// free(path);
	exit(EXIT_FAILURE);
	/* ft_close_all_fds(mini); */
}

void	handle_redirections(t_ast_node *node)
{
	t_redirection *redir;
	int fd;

	redir = node->redirs;
	while (redir)
	{
		/* printf("tokens: %s\n", redir->target); */
		if (redir->type == REDIR_IN)
		{
			fd = open(redir->target, O_RDONLY);
			if (fd == -1)
			{
				perror("failed to open input file");
				close(fd);
				exit(EXIT_FAILURE);
			}
			dup2(fd, STDIN_FILENO);
			close(fd);
		}
		else if (redir->type == REDIR_OUT)
		{
			fd = open(redir->target, O_WRONLY | O_CREAT | O_TRUNC, 0777);
			if (fd == -1)
			{
				perror("failed to open output file");
				close(fd);
				exit(EXIT_FAILURE);
			}
			dup2(fd, STDOUT_FILENO);
			close(fd);
		}
		else if (redir->type == REDIR_APPEND)
		{
			fd = open(redir->target, O_WRONLY | O_CREAT | O_APPEND, 0777);
			if (fd == -1)
			{
				perror("failed to open output file for append");
				close(fd);
				exit(EXIT_FAILURE);
			}
			dup2(fd, STDOUT_FILENO);
			close(fd);
		}
		else if (redir->type == HEREDOC)
		{
			fd = here_doc(redir->target);
			dup2(fd, STDIN_FILENO);
			close(fd);
		}
		redir = redir->next;
	}
}

int	noredirs_orheredoc_singlestdin(t_ast_node *node)
{
	t_redirection *temp;
	int	count_redirs;
	int	count_inredir;

	temp = node->redirs;
	count_redirs = 0;
	count_inredir = 0;
	if (!temp)
		return (0);
	while (temp)
	{
		if (temp->type == HEREDOC)
			return (0);
		else if (temp->type == REDIR_IN)
			count_inredir++;
		else
			count_redirs++;
		temp = temp->next;
	}
	if (count_inredir && !count_redirs)
		return (0);
	return (1);
}

void	ft_close(int fd)
{
	if (fd > 2)
		close(fd);
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

int	check(t_ast_node *node)
{
	if (node->redirs && node->redirs->target && node->redirs->type == HEREDOC)
		return (2);
	if (node->redirs && node->redirs->target && node->redirs->type == REDIR_IN)
		return (3);
	else if (node->redirs && node->redirs->target)
		return (1);
	return (0);
}

void	execute_ast(t_ast_node *node, t_mini *mini)
{
	int pipe_fd[2];
	pid_t	left_pid;
	pid_t	right_pid;
	/* (void)mini; */
/* 	int	left_has_redirection;
	int	right_has_redirection; */

	if (!node)
		return ;

	if (node->type == PIPE)
	{
/* 		left_has_redirection = check(node->left);
		right_has_redirection = check(node->right);
		if (left_has_redirection != 2 && right_has_redirection != 2)
        {
            execute_ast(node->left, mini);
            execute_ast(node->right, mini);
            return;
        } */
		if (pipe(pipe_fd) == -1)
		{
			perror("Error creating pipe");
			return;
		}
		//ft_close_all_fds(mini);
		left_pid = fork();
		if (left_pid == -1)
		{
			perror("Error forking left child");
			close(pipe_fd[0]);
			close(pipe_fd[1]);
			return;
		}
		else if (left_pid == 0)
		{
			/* handle_redirections(node->left); */
			dup2(pipe_fd[1], STDOUT_FILENO);
			close(pipe_fd[0]);
			close(pipe_fd[1]);
			execute_ast(node->left, mini);
		//	ft_close_all_fds();
			exit(EXIT_FAILURE);
		}
		right_pid = fork();
		if (right_pid == -1)
		{
			perror("Error forking right child");
			close(pipe_fd[0]);
			close(pipe_fd[1]);
			return;
		}
		else if (right_pid == 0)
		{
			dup2(pipe_fd[0], STDIN_FILENO);
			close(pipe_fd[1]);
			close(pipe_fd[0]);
			execute_ast(node->right, mini);
			exit(EXIT_FAILURE);
		}
		close(pipe_fd[0]);
		close(pipe_fd[1]);
			/* ft_close_all_fds(mini); */
		waitpid(left_pid, NULL, 0);
		waitpid(right_pid, NULL, 0);
		return;
	}
	if (node->type == CMD)
	{
		if (fork() == 0)
		{
			handle_redirections(node);
			exec(node, mini);
			exit(EXIT_FAILURE);
		}
		else
		{
		//	ft_close_all_fds();
			ft_close_all_fds(mini);
			wait(NULL);
		}
	}
}
