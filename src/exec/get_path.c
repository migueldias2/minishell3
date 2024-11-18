
#include "../../inc/minishell.h"


extern char **environ;

void	ft_free(char *list[])
{
	int	i;

	if (!*list)
		return;
	i = 0;
	while (list[i] != NULL)
	{
		free(list[i]);
		i++;
	}
	free(list);
}

char	*get_env(char **env)
{
	int	i = 0;

	while (env[i])
	{
		// Find the PATH environment variable
		if (ft_strncmp(env[i], "PATH=", 5) == 0)
			return (env[i] + 5);  // Return everything after "PATH="
		i++;
	}
	return (NULL);
}

char	*get_path(char *cmd, char **env)
{
	int		i;
	char	*exec;
	char	**allpath;
	char	*path_part;
	char	*env_path;

	if (!cmd || !*cmd)
        return NULL;
	env_path = get_env(env);
	if (!env_path)
		return (NULL);
	allpath = ft_split(env_path, ':');
	if (!allpath)
		return (NULL);
	i = 0;
	while (allpath[i])
	{
		path_part = ft_strjoin(allpath[i], "/");
		exec = ft_strjoin(path_part, cmd);
		free(path_part);
		if (access(exec, F_OK | X_OK) == 0)  // Check if the executable exists and is executable
		{
			ft_free(allpath);
			return (exec);  // Found the executable
		}
		free(exec);
		i++;
	}
	ft_free(allpath);
	return (NULL);  // Return cmd if not found in PATH
}
