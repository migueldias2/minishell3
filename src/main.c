
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
		if (ft_strncmp(mini->line, "exit", 4) == 0)
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
	int i;

	mini->new_tokens = NULL;
	mini->exp_tokens = NULL;
	mini->env = NULL;
	mini->line = NULL;
	mini->args = NULL;
	mini->shlvl = 0;
	mini->file_count = 0;
	mini->num = 0;
	mini->nr_pipes = 0;
	i = 0;
    while (i < 100)
	{
        mini->filenames[i] = NULL;
        i++;
    }
	i = 0;
	while (i < 1024)
	{
		mini->pids[i] = -1;
		i++;
	}

}


void print_token_list(t_node *head)
{
    t_node *current;
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

void	print_nodes(t_node *sliced_tokens_list)
{
	t_node	*cur;
	t_redir	*redir;
	int				i;
	int	j;

	i = 0;
	cur = sliced_tokens_list;
	while (cur)
	{

		printf("node[%d]: \n", i);
		if (cur->cmd)
			printf("cmd -> %s\n", cur->cmd);
		j = 0;
		if (cur->args)
		{
			while(cur->args[j])
			{
				printf("arg[%d]->%s\n", j, cur->args[j]);
				j++;
			}
		}
		if (cur->redirs)
		{
			redir = cur->redirs;
			while (redir)
			{
				printf("redir->%d, %s\n", redir->type, redir->target);
				redir = redir->next;
			}
		}
		printf("outfile %d\n", cur->outfile);
		printf("infile %d\n", cur->infile);
		cur = cur->next;
		i++;
		printf("\n");
	}
}

void cleanup_heredoc_files(t_mini *mini)
{
    int i;

	i = 0;
    while (i < 100)
	{
		if (mini->filenames[i])
        {
			unlink(mini->filenames[i]);
        	free(mini->filenames[i]);
        	mini->filenames[i] = NULL;
		}
        i++;
    }
}

void	heredoc_last(t_node **sliced_tokens_list)
{
	t_node	*cur;
	t_node	*temp;

	cur = *sliced_tokens_list;
	if (!cur->next)
		return ;
	while (cur)
	{
		if (cur->redirs && cur->redirs->type == HEREDOC)
		{
			if (cur->previous)
			{
				cur->previous->next = cur->next;
				if (cur->next)
					cur->next->previous = cur->previous;
			}
			else
			{
				*sliced_tokens_list = cur->next;
				if (cur->next)
					cur->next->previous = NULL;
			}
			temp = cur;
			while (cur->next)
				cur = cur->next;
			cur->next = temp;
			temp->previous = cur;
			temp->next = NULL;
			break;
		}
		cur = cur->next;
	}
}

void	shell_looping(t_mini *mini)
{
	t_node *sliced_tokens_list;

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
		/* int i = 0; */
		mini->exp_tokens = expand_vars(mini->new_tokens, mini->env);
		if (!mini->exp_tokens)
		{
			free_all(mini->new_tokens, mini->line);
			continue;
		}
		/* i = 0;
		while (mini->exp_tokens[i])
		{
			printf("%s\n", mini->exp_tokens[i]);
			i++;
		} */
		sliced_tokens_list = NULL;

		if (*mini->line)
		{
			parse_tokens(mini->exp_tokens, &sliced_tokens_list);
			final_sliced_list(&sliced_tokens_list);
			/* print_token_list(sliced_tokens_list); */
			mini->nr_pipes = 0;
			fill_redirs_cmd_args(&sliced_tokens_list, mini);
			mini->nr_pipes--;
			/* printf("%d\n", mini->nr_pipes); */
			open_redirs(sliced_tokens_list, mini);

			heredoc_last(&sliced_tokens_list);
			/* print_nodes(sliced_tokens_list); */
			execute(sliced_tokens_list, mini);

		}




		free_ast(&sliced_tokens_list);
		cleanup_heredoc_files(mini);
		free_all(mini->exp_tokens, mini->line);
	}
}


int main(int ac, char **av, char **env)
{
	t_mini		*mini;

	mini = malloc(sizeof(t_mini));
	if (!mini)
		return (0);
	(void)ac;
	(void)av;
	init_mini(mini);
	if (!env)
   		init_myown_envp(mini);
    else
   		init_envp(mini, env);
	shell_looping(mini);
	/* ft_close_all_fds(mini); */
	free(mini);
	return (0);
}
