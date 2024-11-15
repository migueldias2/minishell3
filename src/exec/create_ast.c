
#include "../../inc/minishell.h"

t_ast_node	*new_ast_node(int type, char *command, char **args)
{
	t_ast_node	*node;

	node = malloc(sizeof(t_ast_node));
	if (!node)
		return (NULL);
	node->type = type;
	node->cmd = command;
	node->args = args;
	node->left = NULL;
	node->right = NULL;
	node->redirs = NULL;
	return (node);
}

t_redirection	*new_redirection(int type ,char *target)
{
	t_redirection *redir;

	redir = malloc(sizeof(t_redirection));
	if (!redir)
		return (NULL);
	redir->type = type;
	redir->target = target;
	redir->next = NULL;
	return (redir);
}

void	add_redirection(t_ast_node *node, int type, char *target)
{
	t_redirection	*redir;
	t_redirection	*temp;

	temp = NULL;
	redir = new_redirection(type, target);
	if (!redir)
		return ;
	if (!node->redirs)
		node->redirs = redir;
	else
	{
		temp = node->redirs;
		while (temp->next)
			temp = temp->next;
		temp->next = redir;
	}
}

int	get_redir_type(char *token)
{
	if (!ft_strcmp(token, ">"))
		return (REDIR_OUT);
	else if (!ft_strcmp(token, ">>"))
		return (REDIR_APPEND);
	else if (!ft_strcmp(token, "<"))
		return (REDIR_IN);
	else if (!ft_strcmp(token, "<<"))
		return (HEREDOC);
	return (-1);
}


char **filter(char **tokens)
{
	int i;
	int count;
	int num;

	i = 0;
	count = 0;
	while (tokens[i])
	{
		num = get_redir_type(tokens[i]);
		if (num != -1)
		{
			free(tokens[i]);
			free(tokens[i + 1]);
			i += 2;
		}
		else
		{
			tokens[count] = tokens[i];
			count++;
			i++;
		}
	}
	tokens[count] = NULL;
	return tokens;
}

t_token_node *add_token_to_list(t_token_node *head, char **tokens)
{
	t_token_node	*new_node;
	t_token_node	*current;

	new_node = malloc(sizeof(t_token_node));
	if (!new_node)
	{
		perror("Memory allocation-LIST error");
		return (NULL);
	}
	new_node->tokens = tokens;
	new_node->next = NULL;
	if (head == NULL)
		return new_node;
	current = head;
	while (current->next != NULL)
		current = current->next;
	current->next = new_node;
	return head;
}


t_ast_node	*parse_tokens(char **tokens, t_token_node **sliced_tokens_list)
{
	int			pipe_index;
	t_ast_node	*node;
	int			cmd_index;
	int			i;
	int			redir_type;
	char		**left_slice;
	char		**right_slice;

	i = 0;
	node = NULL;
	cmd_index = -1;
	pipe_index = find_pipe(tokens);
	if (pipe_index != -1)
	{
		node = new_ast_node(PIPE, NULL, NULL);
		left_slice = slice_tokens(tokens, 0, pipe_index);
		right_slice = slice_tokens(tokens, pipe_index + 1, -1);

		*sliced_tokens_list = add_token_to_list(*sliced_tokens_list, left_slice);
		*sliced_tokens_list = add_token_to_list(*sliced_tokens_list, right_slice);

		node->left = parse_tokens(left_slice, sliced_tokens_list);
		node->right = parse_tokens(right_slice, sliced_tokens_list);
		return (node);
	}

	while (tokens[i] != NULL)
	{
    	redir_type = get_redir_type(tokens[i]);
    	if (redir_type == HEREDOC)
		{
        	if (tokens[i + 1] == NULL)
            	perror("Syntax error: missing file for redirection");
        	if (!node)
				node = new_ast_node(CMD, NULL, NULL);
            add_redirection(node, HEREDOC, tokens[i + 1]);
            i += 2;
        }
		else
            i++;
	}
	i = 0;
	while (tokens[i] != NULL)
	{
    	redir_type = get_redir_type(tokens[i]);
    	if (redir_type != -1 && redir_type != HEREDOC)
		{
        	if (tokens[i + 1] == NULL)
            	perror("Syntax error: missing file for redirection");
			if (!node)
				node = new_ast_node(CMD, NULL, NULL);
            add_redirection(node, redir_type, tokens[i + 1]);
            i += 2;
    	}
		else
			i++;
	}
	i = 0;
	while (tokens[i] != NULL)
	{
		redir_type = get_redir_type(tokens[i]);
		if (redir_type != -1)
			i += 2;
		else
		{
			cmd_index = i;
			break;
		}
	}

	if (cmd_index != -1)
	{
    	if (!node)
			node = new_ast_node(CMD, tokens[cmd_index], slice_tokens(tokens, cmd_index, -1));
		else
		{
			node->cmd = tokens[cmd_index];
			node->args = filter(slice_tokens(tokens, cmd_index, -1));
		}
	}
	return node;
}
