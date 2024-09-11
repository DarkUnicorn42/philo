/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mwojtcza <mwojtcza@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/17 12:55:25 by mwojtcza          #+#    #+#             */
/*   Updated: 2024/07/17 12:55:25 by mwojtcza         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/philo.h"

long	cur_time_ms(void)
{
	struct timeval	time;

	gettimeofday(&time, NULL);
	return ((time.tv_sec * 1000) + (time.tv_usec / 1000));
}

int	ft_atoi(const char *nptr)
{
	int	i;
	int	sign;
	int	res;

	i = 0;
	sign = 1;
	res = 0;
	while (nptr[i] == 32 || (nptr[i] >= 9 && nptr[i] <= 13))
		i++;
	if (nptr[i] == '+' || nptr[i] == '-')
	{
		if (nptr[i] == '-')
			sign = -1;
		i++;
	}
	while (nptr[i] && nptr[i] >= 48 && nptr[i] <= 57)
	{
		res *= 10;
		res += (nptr[i] - 48);
		i++;
	}
	return (res *= sign);
}

void	print_error(char *message)
{
	printf("Error: %s\n", message);
	exit(EXIT_FAILURE);
}

void	print_action(t_philosopher *philo, const char *action)
{
	long	timestamp;

	pthread_mutex_lock(&philo->sim->log_mutex);
	if (philo->sim->death_flag == 0)
	{
		timestamp = cur_time_ms() - philo->sim->start_time;
		printf("%ld %d %s\n", timestamp, philo->id, action);
	}
	pthread_mutex_unlock(&philo->sim->log_mutex);
}

void	release_forks(t_philosopher *philo)
{
	pthread_mutex_unlock(philo->left_fork);
	pthread_mutex_unlock(philo->right_fork);
}
