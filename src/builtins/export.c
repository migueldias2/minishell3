#include "../../inc/minishell.h"

void ft_export(t_mini *mini)
{
	int i = 0;
	while (mini->env[i] != NULL)
		i++;
	mini->env[i] = ft_strjoin("NEW_VAR=", mini->args[1]);
}
