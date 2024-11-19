#include "../../inc/minishell.h"

void	add_redir_app(t_mini *mini)
{
	if (mini->cur->outfile != 1)
		close(mini->cur->outfile);
	mini->cur->outfile = open(mini->redir->target, \
		O_WRONLY | O_CREAT | O_APPEND, 0777);
	if (mini->cur->outfile == -1)
	{
		perror("failed to open output file for append");
		close(mini->cur->outfile);
	}
}

void	add_redir_out(t_mini *mini)
{
	if (mini->cur->outfile != 1)
		close(mini->cur->outfile);
	mini->cur->outfile = open(mini->redir->target, \
		O_WRONLY | O_CREAT | O_TRUNC, 0777);
	if (mini->cur->outfile == -1)
	{
		perror("failed to open output file");
		close(mini->cur->outfile);
	}
}

void	add_redir_in(t_mini *mini)
{
	if (mini->cur->infile != 0)
		close(mini->cur->infile);
	mini->cur->infile = open(mini->redir->target, O_RDONLY);
	if (mini->cur->infile == -1)
	{
		perror("failed to open input file");
		close(mini->cur->infile);
	}
}

void	add_heredoc(t_node *node, t_mini *mini)
{
	t_node	*cur;
	t_redir	*redir;

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
				}
				redir = redir->next;
			}
		}
		cur = cur->next;
	}
}

void	open_redirs(t_node *node, t_mini *mini)
{
	add_heredoc(node, mini);
	mini->cur = node;
	while (mini->cur)
	{
		if (mini->cur->redirs)
		{
			mini->redir = mini->cur->redirs;
			while (mini->redir)
			{
				if (mini->redir->type == HEREDOC)
				{
					mini->redir = mini->redir->next;
					continue ;
				}
				else if (mini->redir->type == REDIR_IN)
					add_redir_in(mini);
				else if (mini->redir->type == REDIR_OUT)
					add_redir_out(mini);
				else if (mini->redir->type == REDIR_APPEND)
					add_redir_app(mini);
				mini->redir = mini->redir->next;
			}
		}
		mini->cur = mini->cur->next;
	}
}
