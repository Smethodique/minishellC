/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_split.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: stakhtou <stakhtou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/12 18:03:41 by stakhtou          #+#    #+#             */
/*   Updated: 2023/11/24 05:05:22 by stakhtou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"
#include <stdlib.h>

static size_t	word_counter(const char *s, char c)
{
	size_t	i;
	size_t	count;

	i = 0;
	count = 0;
	if (!s)
		return (0);
	while (s[i] != '\0' && s[i] == c)
		i++;
	while (s[i] != '\0')
	{
		if (s[i] != c)
		{
			count++;
			while (s[i] != '\0' && s[i] != c)
				i++;
		}
		else
			i++;
	}
	return (count);
}

static void	free_split_memory(char **ptr, size_t size)
{
	while (size > 0)
	{
		free(ptr[--size]);
	}
	free(ptr);
}

static char	**ft_oujda(char **ptr, const char *s, char c, size_t words)
{
	size_t	i;
	size_t	j;
	size_t	k;

	i = 0;
	j = 0;
	while (j < words)
	{
		while (s[i] == c)
			i++;
		k = 0;
		while (s[i + k] != '\0' && s[i + k] != c)
			k++;
		ptr[j] = (char *)malloc(sizeof(char) * (k + 1));
		if (ptr[j] == NULL)
		{
			free_split_memory(ptr, j);
			return (NULL);
		}
		ft_strlcpy(ptr[j], &s[i], k + 1);
		i += k;
		j++;
	}
	return (ptr);
}

char	**ft_split(char const *s, char c)
{
	char	**ptr;
	size_t	words;

	words = word_counter(s, c);
	ptr = (char **)malloc(sizeof(char *) * (words + 1));
	if (ptr == NULL)
		return (NULL);
	ptr = ft_oujda(ptr, s, c, words);
	if (ptr == NULL)
		return (NULL);
	ptr[words] = NULL;
	return (ptr);
}

// int	main(void)
// {
// 	const char	*input_string = "Hello World How Are You";
// 	char		delimiter;
// 	char		**result;
// 	size_t		i;

// 	delimiter = ' ';
// 	result = ft_split(input_string, delimiter);
// 	i = 0;
// 	while (result[i] != NULL)
// 	{
// 		printf("%s\n", result[i]);
// 		++i;
// 	}
// }
