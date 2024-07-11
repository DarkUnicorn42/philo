/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_printfu.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mwojtcza <mwojtcza@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/16 15:50:47 by mwojtcza          #+#    #+#             */
/*   Updated: 2024/07/07 17:02:19 by mwojtcza         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	ft_printfu(unsigned int n, int *len)
{
	if (n > 9)
		ft_printfu(n / 10, len);
	ft_printfc(n % 10 + '0', len);
}
