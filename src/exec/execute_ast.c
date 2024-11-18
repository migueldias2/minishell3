
#include "../../inc/minishell.h"



void	exec(t_token_node *cur, t_mini *mini)
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
	// ft_close_all_fds(mini); 
}

/* void	handle_redirections(t_ast_node *node)
{
	t_redirection *redir;
	int fd;

	redir = node->redirs;
	while (redir)
	{
		// printf("tokens: %s\n", redir->target); 
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
} */


void	do_last_command(t_token_node *cur, t_mini *mini)
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
	{
		wait(NULL);
	}

}

void	do_pipe(t_token_node *cur, t_mini *mini)
{
	pid_t	pid;
	int		pipe_fd[2];

	if (pipe(pipe_fd) == -1)
	{
		perror("error pipe");
    	exit(EXIT_FAILURE);
	}
	pid = fork();
	if (pid == -1)
	{
		perror("error fork");
    	exit(EXIT_FAILURE);
	}
	if (pid == 0)
	{
		close(pipe_fd[0]);
		dup2(pipe_fd[1], STDOUT_FILENO);
		close(pipe_fd[1]);
		if (cur->infile != 0)
		{
			dup2(cur->infile, STDIN_FILENO);
		}
		if (cur->outfile != 1)
		{
			dup2(cur->outfile, STDOUT_FILENO);
		}
		exec(cur, mini);
	}
	else
	{
		close(pipe_fd[1]);
		dup2(pipe_fd[0], STDIN_FILENO);
		close(pipe_fd[0]);
	}
}

void	execute(t_token_node *sliced_tokens_list, t_mini *mini)
{
	t_token_node	*cur;

	cur = sliced_tokens_list;
	while (cur->next != NULL)
	{
		do_pipe(cur, mini);
		cur = cur->next;
	}
	do_last_command(cur, mini);
	while ((wait(NULL)) > 0);
}

void	fill_redirs(t_token_node *node, t_mini *mini)
{
	t_token_node	*cur;
	t_redirection	*redir;
	(void)*mini;

	cur = node;
	while (cur)
	{
		if (cur->redirs)
		{
			redir = cur->redirs;
			while (redir)
			{
				if (redir->type == HEREDOC)
				{
					if (cur->infile != 0)
						close(cur->infile);
					cur->infile = here_doc(mini, redir->target);
					/* dup2(cur->infile, STDIN_FILENO);
					close(cur->infile); */
				}
				redir = redir->next;
			}
		}
		cur = cur->next;
	}
	cur = node;
	while (cur)
	{
		if (cur->redirs)
		{
			redir = cur->redirs;
			while (redir)
			{
				if (redir->type == HEREDOC)
				{
					redir = redir->next;
					continue ;
				}
				else if (redir->type == REDIR_IN)
				{
					if (cur->infile != 0)
						close(cur->infile);
					cur->infile = open(redir->target, O_RDONLY);
					if (cur->infile == -1)
					{
						perror("failed to open input file");
						close(cur->infile);
						exit(EXIT_FAILURE);
					}
					/* dup2(fd, STDIN_FILENO);
					close(fd); */
				}
				else if (redir->type == REDIR_OUT)
				{
					if (cur->outfile != 1)
						close(cur->outfile);
					cur->outfile = open(redir->target, O_WRONLY | O_CREAT | O_TRUNC, 0777);
					if (cur->outfile == -1)
					{
						perror("failed to open output file");
						close(cur->outfile);
						exit(EXIT_FAILURE);
					}
					/* dup2(fd, STDOUT_FILENO);
					close(fd); */
				}
				else if (redir->type == REDIR_APPEND)
				{
					if (cur->outfile != 1)
						close(cur->outfile);
					cur->outfile = open(redir->target, O_WRONLY | O_CREAT | O_APPEND, 0777);
					if (cur->outfile == -1)
					{
						perror("failed to open output file for append");
						close(cur->outfile);
						exit(EXIT_FAILURE);
					}
					/* dup2(fd, STDOUT_FILENO);
					close(fd); */
				}
				redir = redir->next;
			}
		}
		cur = cur->next;
	}
}