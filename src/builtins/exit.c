#include "../../inc/minishell.h"


void ft_exit(t_mini *mini)
{
	if (mini->env)
	   ft_free_env(mini->env);
	if (mini->args)
	   free_args(mini->args);
	if (mini->line)
	   free(mini->line);
	if (mini)
	   free(mini);
	exit(0);
}
