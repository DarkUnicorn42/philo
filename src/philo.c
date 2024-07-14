/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mwojtcza <mwojtcza@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/11 15:55:34 by mwojtcza          #+#    #+#             */
/*   Updated: 2024/07/11 16:08:16 by mwojtcza         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/philo.h"

void	parse_arg(int ac, char **av, t_data *data)
{
	if (ac < 5 || ac > 6)
		ft_printf("Error: number of philo, time to die, time to eat, time to sleep, [times to eat]\n");
	data->number_of_philosophers = ft_atoi(av[1]);
	data->time_to_die = ft_atoi(av[2]);
	data->time_to_eat = ft_atoi(av[3]);
	data->time_to_sleep = ft_atoi(av[4]);
	if (ac == 6)
		data->number_of_times_each_philosopher_must_eat = ft_atoi(av[5]);
	else
		data->number_of_times_each_philosopher_must_eat = -1;
	if (data->number_of_philosophers <= 0 || data->time_to_die <= 0 || data->time_to_eat <= 0 || data->time_to_sleep <= 0)
	{
		ft_printf("Error: Invalid argument values.\n");
		print_usage();
	}
}

int	main(int ac, char **av)
{
	t_data data;

	parse_arg(ac, av, &data);
	if (data.number_of_times_each_philosopher_must_eat != -1)
		printf("Number of times each philosopher must eat: %d\n", data.number_of_times_each_philosopher_must_eat);
	else
		printf("Number of times each philosopher must eat: Not specified\n");
	return (0);
}

/*
Your(s) program(s) should take the following arguments:
number_of_philosophers time_to_die time_to_eat time_to_sleep
[number_of_times_each_philosopher_must_eat]
◦ number_of_philosophers: The number of philosophers and also the number
of forks.
◦ time_to_die (in milliseconds): If a philosopher didn’t start eating time_to_die
milliseconds since the beginning of their last meal or the beginning of the simulation, they die.
◦ time_to_eat (in milliseconds): The time it takes for a philosopher to eat.
During that time, they will need to hold two forks.
◦ time_to_sleep (in milliseconds): The time a philosopher will spend sleeping.
◦ number_of_times_each_philosopher_must_eat (optional argument): If all
philosophers have eaten at least number_of_times_each_philosopher_must_eat
times, the simulation stops. If not specified, the simulation stops when a
philosopher dies.
• Each philosopher has a number ranging from 1 to number_of_philosophers.
• Philosopher number 1 sits next to philosopher number number_of_philosophers.
Any other philosopher number N sits between philosopher number N - 1 and philosopher number N + 1.

About the logs of your program:
• Any state change of a philosopher must be formatted as follows:
◦ timestamp_in_ms X has taken a fork
◦ timestamp_in_ms X is eating
◦ timestamp_in_ms X is sleeping
◦ timestamp_in_ms X is thinking
◦ timestamp_in_ms X died
Replace timestamp_in_ms with the current timestamp in milliseconds
and X with the philosopher number.
• A displayed state message should not be mixed up with another message.
• A message announcing a philosopher died should be displayed no more than 10 ms
after the actual death of the philosopher.
• Again, philosophers should avoid dying
*/