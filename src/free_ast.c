#include "../inc/minishell.h"

void free_redirections(t_redir *redir)
{
	t_redir *temp;

	while (redir)
	{
		temp = redir;
		redir = redir->next;
		free(temp);
		temp = NULL;
	}
}

void free_ast(t_node **node)
{
	t_node	*cur;
	t_node	*temp;
	int i;

	cur = *node;
	if (cur)
	{
		while(cur)
		{
			temp = cur->next;
			if (cur->tokens)
			{
				i = 0;
				while (cur->tokens[i])
				{
					free(cur->tokens[i]);
					cur->tokens[i] = NULL;
					i++;
				}
				free(cur->tokens);
				cur->tokens = NULL;
			}
			if (cur->args)
			{
				i = 0;
				while (cur->args[i] != NULL)
				{
					free(cur->args[i]);
					cur->args[i] = NULL;
					i++;
				}
				free(cur->args);
				cur->args = NULL;
			}
			free_redirections(cur->redirs);
			free(cur);
			cur = temp;
		}
	}
}
