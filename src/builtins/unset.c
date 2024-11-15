#include "../../inc/minishell.h"

void ft_unset(t_mini *mini)
{
	int i;
	int j;
	char **new_env;

	i = 0;
	j = 0;
	while (mini->env[i])
		i++;
	new_env = (char **)malloc(sizeof(char *) * i);
	i = 0;
	while (mini->env[i])
	{
		if (ft_strncmp(mini->env[i], mini->args[1], ft_strlen(mini->args[1])) != 0)
		{
			new_env[j] = mini->env[i];
			j++;
		}
		else
		{
			free(mini->env[i]);
		}
		i++;
	}
	new_env[j] = NULL;
	free(mini->env);
	mini->env = new_env;
}
