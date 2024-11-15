
#include "../inc/minishell.h"

void check_line(char *line)
{
	int i = 0;
	while (line[i])
	{
		if (line[i] == ';')
		{
			printf("Error: ';' is not allowed\n");
			exit(0);
		}
		i++;
	}
}

void handling_signals(int sig, siginfo_t *info, void *context)
{
	(void)info;
	(void)context;
	if (sig == SIGINT)
	{
		write(1, "\n", 1);
		rl_replace_line("", 0);
        rl_on_new_line();
        rl_redisplay();
	}
}

// int main(int ac, char **av, char **envp)
// {
//     t_mini *main_mini;
//     char *line;
//     struct sigaction sa;

//     line = NULL;
//     ft_bzero(&sa, sizeof(sa));
//     sa.sa_flags = SA_RESTART;
//     sa.sa_sigaction = &handling_signals;
//    	sigaction(SIGINT, &sa, NULL);
//     sigaction(SIGTSTP, &sa, NULL);
//     (void)av, (void)ac;

//     signal(SIGQUIT, SIG_IGN);
// 	main_mini = (t_mini *)malloc(sizeof(t_mini));
//     if (!envp)
//     	init_myown_envp(main_mini);
//     else
//     	init_envp(main_mini, envp);
//     while (1)
//     {
//         line = readline("minishell> ");
//         if (!line)
//         {
//            	printf("Exiting of shell!\n");
//            	break ;
//         }
//         if (*line)
//         {
//             main_mini->line = ft_strdup(line);
//             if (main_mini->line && *main_mini->line != '\n')
//             {
//                 add_history(main_mini->line);
//                 choose_args(main_mini);
//                 free(main_mini->line);
//             }
//         }
//         free(line);
//     }
//     free(main_mini);
//     return 0;
// }


bool	if_stats_input(t_mini *mini)
{	//false para break //true para continue
	if (!mini->line)
	{
		printf("exit\n");
		ft_exit(mini);
		return (false);
	}
	if (*mini->line)
	{
		add_history(mini->line);
		if (ft_strcmp(mini->line, "exit") == 0)
		{
			printf("exit\n");
			ft_exit(mini);
			return (false);
		}
	}
	return (true);
}

void	init_mini(t_mini *mini)
{
	mini->new_tokens = NULL;
	mini->exp_tokens = NULL;
	mini->env = NULL;
	mini->line = NULL;
	mini->args = NULL;
	mini->shlvl = 0;
}

void free_token_list(t_token_node *head)
{
    t_token_node *current = head;
    t_token_node *next;

    while (current != NULL)
	{
		next = current->next;
        for (int i = 0; current->tokens[i] != NULL; i++) {
            free(current->tokens[i]);
			current->tokens[i] = NULL;  // Free each string in the tokens array
        }
        free(current->tokens);
		current->tokens = NULL;  // Finally, free the array of tokens itself

        free(current);
        current = next;  // Move to the next node
    }
}

void print_token_list(t_token_node *head)
{
    t_token_node *current;
	int i;
	int	j = 0;

	i = 0;
	current = head;
    while (current != NULL)
	{
		printf("node[%d]:  \n", j);
        for (i = 0; current->tokens[i] != NULL; i++) {
			printf("token[%d]: %s\n", i, current->tokens[i]);  // Free each string in the tokens array
        }
		current = current->next;
		j++;
		printf("\n");
    }
}

void	shell_looping(t_mini *mini, t_ast_node *ast_root)
{
	t_token_node *sliced_tokens_list;
	while (1)
	{
		mini->line = readline("minishell> ");
		if (!if_stats_input(mini))
			break;
		if (!checker_quotes(mini->line))
			continue;
		mini->new_tokens = tokenize(mini->line);
		if (!mini->new_tokens)
		{
			free(mini->line);
			continue;
		}
		mini->exp_tokens = expand_vars(mini->new_tokens, mini->env);
		if (!mini->exp_tokens)
		{
			free_all(mini->new_tokens, mini->line);
			continue;
		}
		/* int i = 0;
		while (mini->exp_tokens[i])
		{
			printf("%s\n", mini->exp_tokens[i]);
			i++;
		} */
		sliced_tokens_list = NULL;
		ast_root = parse_tokens(mini->exp_tokens, &sliced_tokens_list);
		/* print_token_list(sliced_tokens_list); */
		execute_ast(ast_root, mini);
		free_token_list(sliced_tokens_list);
		free_ast(ast_root);
		free_all(mini->exp_tokens, mini->line);
	}
}


int main(int ac, char **av, char **env)
{
	t_ast_node *ast_root;
	t_mini		*mini;

	mini = malloc(sizeof(t_mini));
	if (!mini)
		return (0);
	(void)ac;
	(void)av;
	init_mini(mini);
	ast_root = NULL;
	if (!env)
   		init_myown_envp(mini);
    else
   		init_envp(mini, env);
	shell_looping(mini, ast_root);
	ft_close_all_fds(mini);
	free_ast(ast_root);
	free(mini);
	return (0);
}
