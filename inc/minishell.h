
# ifndef MINISHELL_H
#  define MINISHELL_H

# include <stdio.h>
# include <stdlib.h>
# include <unistd.h>
# include <string.h>
# include <sys/types.h>
# include <sys/wait.h>
# include <signal.h>
# include <errno.h>
# include <fcntl.h>
# include <readline/readline.h>
# include <readline/history.h>
# include "../libft/libft.h"
# include <stdbool.h>

# define MAX_TOKENS 300
# define CMD 1
# define PIPE 2
# define REDIR_IN 3
# define REDIR_OUT 4
# define REDIR_APPEND 5
# define HEREDOC 6
# define FILE 7
# define CD 8
# define PWD 9
# define EXPORT 10
# define UNSET 11
# define ENV 12
# define BUFFER_SIZE 1024

typedef struct s_mini
{
	char		**new_tokens;
	char		**exp_tokens;
	char		**env;
	char		*line;
	char		**args;
	int			shlvl;
	int 		exit_status;
	int			pipe_fd[2];
}	t_mini;

typedef struct s_token_node
{
    char **tokens;
    struct s_token_node *next;
}	t_token_node;

typedef struct s_redirection
{
	int					type;
	char				*target;
	struct s_redirection *next;
}	t_redirection;

typedef struct s_ast_node
{
	int					type;//	Type of node
	char				*cmd; //used for command nodes
	char				**args; //arguments for command nodes
	t_redirection 		*redirs;
	struct s_ast_node 	*left;
	struct s_ast_node 	*right;
}	t_ast_node;


typedef struct
{
	char	*exp;
	char	*var_start;
	char	*temp;
	int		i;
	int		d_quote;
	int		s_quote;
	int		len;
	int		start;
}	expand_data;

typedef struct
{
	char **t;
	int	i;
	int	start;
	int	ti;
	int	d_quote;
	int	s_quote;
}	tokenize_data;

//BUILTINS

//ENV
void			ft_env(t_mini *mini);
void			init_envp(t_mini *mini, char **envp);
char 			*encontra_barra(char *s);
char 			*get_var_env(char **env, char *to_find);
void            init_myown_envp(t_mini *mini);

//CD
void 			update_env_back_cd(char *new_pwd, char*pwd, t_mini *mini);
void 			update_env(char *new_pwd, char *pwd,t_mini *mini);
void 			update_env_abs(char *pwd, char *home, t_mini *mini);
void 			replace_var_env(char **envp, char *to_found, char *to_replace);
void 			ft_cd(t_ast_node *node,t_mini *mini);

//ECHO
void            ft_echo(t_mini *mini);

//EXPORT
void			ft_export(t_mini *mini);

//UNSET
void			ft_unset(t_mini *mini);

//EXIT
void			ft_exit(t_mini *mini);

//PWD
void			ft_pwd();

//MAIN
void			init_args(t_mini *mini, int ac, char **envp);
void			shell_looping(t_mini *mini, t_ast_node *ast_root);

//PARSE
char			**tokenize(char *str);
char			**expand_vars(char **new_tokens, char **env_copy);
char			**slice_tokens(char **tokens, int i, int end);
t_ast_node		*new_ast_node(int type, char *command, char **args);
t_ast_node	    *parse_tokens(char **tokens, t_token_node **sliced_tokens_list);
int				find_pipe(char **tokens);
bool 			check_for_unclosed_quotes(char *input);
int 			semicolon_checker(char *str);
bool 			checker_quotes(char *input);

//REDIR
t_redirection	*new_redirection(int type ,char *target);
int				noredirs_orheredoc_singlestdin(t_ast_node *node);
void			handle_redirections(t_ast_node *node);


//EXEC
char			*path_find(char **envp, char *cmd);
void			exec(t_ast_node *node, t_mini *mini);
void			execute_ast(t_ast_node *node, t_mini *mini);
char			*get_path(char *cmd);

//UTILS
void 			free_all(char **tokens, char *input);
void			free_args(char **args);
void 			free_2_all(char **args1, char **args2);
void            free_token_list(t_token_node *head);
void			ft_free_env(char **env);
void			free_sliced(char **sliced, int count);
void			free_ast(t_ast_node *node);
void			ft_close_all_fds(t_mini *mini);
void			ft_close(int fd);
void 			print_ast(t_ast_node *root);

//UTILS
int				here_doc(char *str);

#endif
