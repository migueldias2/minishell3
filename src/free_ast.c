#include "../inc/minishell.h"

void free_redirections(t_redirection *redir)
{
	t_redirection *temp;

	while (redir)
	{
		temp = redir;
		redir = redir->next;
		free(temp);
		temp = NULL;
	}
}

void free_ast(t_ast_node *node)
{
	if (!node)
		return;
	if (node->left)
	{
		free_ast(node->left);
	}
	if (node->right)
	{
		free_ast(node->right);
	}
	if (node->args)
	{
		int i = 0;
		while (node->args[i] != NULL)
		{
			free(node->args[i]);
			node->args[i] = NULL;
			i++;
		}
		free(node->args);
		node->args = NULL;
	}
	free_redirections(node->redirs);
	free(node);
	node = NULL;
}
