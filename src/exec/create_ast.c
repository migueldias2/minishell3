
#include "../../inc/minishell.h"

/* t_ast_node	*new_ast_node(int type, char *command, char **args)
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
} */

t_redir	*new_redirection(int type ,char *target)
{
	t_redir *redir;

	redir = malloc(sizeof(t_redir));
	if (!redir)
		return (NULL);
	redir->type = type;
	redir->target = target;
	redir->next = NULL;
	return (redir);
}

void	add_redirection(t_node *node, int type, char *target)
{
	t_redir	*redir;
	t_redir	*temp;

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

t_node *add_token_to_list(t_node *head, char **tokens)
{
	t_node	*new_node;
	t_node	*current;

	new_node = malloc(sizeof(t_node));
	if (!new_node)
	{
		perror("Memory allocation-LIST error");
		return (NULL);
	}
	new_node->tokens = tokens;
	new_node->next = NULL;
	new_node->cmd = NULL;
	new_node->previous = NULL;
	new_node->redirs = NULL;
	new_node->args = NULL;
	new_node->infile = STDIN_FILENO;
	new_node->outfile = STDOUT_FILENO;
	if (head == NULL)
		return new_node;
	current = head;
	while (current->next != NULL)
		current = current->next;
	current->next = new_node;
	current->next->previous = current;
	return head;
}

void	expand_args(char **args, char **env_copy)
{
	int		i;
	int		len;
	char	*str;

	i = 0;
	while (args[i])
	{
		len = ft_strlen(args[i]);
		if (!strcmp(args[i], "\"|\"") || !strcmp(args[i], "\">\"") || \
			!strcmp(args[i], "\"<\"") || !strcmp(args[i], "\">>\"") || \
			!strcmp(args[i], "\"<<\""))
		{
			str = ft_strdup(args[i]);
			free(args[i]);
			args[i] = expand_var(ft_strndup(str, len), env_copy);
			free(str);
			i++;
		}
		else
			i++;
	}
}

void	final_sliced_list(t_node **sliced_tokens_list)
{
	int				i;
	t_node	*current;
	t_node	*temp_next;

	current = *sliced_tokens_list;
	i = 0;
	while(current)
	{
		temp_next = current->next;
		if (i % 2 != 0)
		{
			if (current->previous) // If there is a previous node
				current->previous->next = current->next;
			else // If there is no previous node, we're removing the head
				*sliced_tokens_list = current->next;
			if (current->next)
				current->next->previous = current->previous;
			free_args(current->tokens);
			free(current);
		}
		current = temp_next;
		i++;
	}
}

void	parse_tokens(char **tokens, t_node **sliced_tokens_list)
{
	pt_data data;

	data.pipe_index = find_pipe(tokens);
	if (data.pipe_index != -1)
	{
		data.left_slice = slice_tokens(tokens, 0, data.pipe_index);
		data.right_slice = slice_tokens(tokens, data.pipe_index + 1, -1);

		*sliced_tokens_list = add_token_to_list(*sliced_tokens_list, data.left_slice);
		*sliced_tokens_list = add_token_to_list(*sliced_tokens_list, data.right_slice);

		parse_tokens(data.right_slice, sliced_tokens_list);
	}
	else
		*sliced_tokens_list = add_token_to_list(*sliced_tokens_list, slice_tokens(tokens, 0, -1));
}

void	fill_redirs_cmd_args(t_node **sliced_tokens_list, t_mini *mini)
{
	t_node	*cur;
	int				i;
	int				redir_type;
	int				cmd_index;

	cur = *sliced_tokens_list;

	while (cur)
	{
		i = 0;
		cmd_index = -1;
		while (cur->tokens[i] != NULL)
		{
			redir_type = get_redir_type(cur->tokens[i]);
			if (redir_type == HEREDOC)
			{
				if (cur->tokens[i + 1] == NULL)
					perror("Syntax error: missing file for redirection");
				add_redirection(cur, HEREDOC, cur->tokens[i + 1]);
				i += 2;
			}
			else
				i++;
		}
		i = 0;
		while (cur->tokens[i] != NULL)
		{
			redir_type = get_redir_type(cur->tokens[i]);
			if (redir_type != -1 && redir_type != HEREDOC)
			{
				if (cur->tokens[i + 1] == NULL)
					perror("Syntax error: missing file for redirection");
				add_redirection(cur, redir_type, cur->tokens[i + 1]);
				i += 2;
			}
			else
				i++;
		}
		i = 0;
		while (cur->tokens[i] != NULL)
		{
			redir_type = get_redir_type(cur->tokens[i]);
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
			cur->cmd = cur->tokens[cmd_index];
			cur->args = filter(slice_tokens(cur->tokens, cmd_index, -1));
			expand_args(cur->args, mini->env);
		}
		cur = cur->next;
		mini->nr_pipes++;
	}
}
