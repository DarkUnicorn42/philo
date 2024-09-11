/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mwojtcza <mwojtcza@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/09 13:40:27 by mwojtcza          #+#    #+#             */
/*   Updated: 2024/09/11 15:55:08 by mwojtcza         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/philo.h"

void	*philosopher_routine(void *arg)
{
	t_philosopher	*philo;

	philo = (t_philosopher *)arg;
	while (1)
	{
		pthread_mutex_lock(&philo->sim->death_mutex);
		if (philo->sim->death_flag == 1)
		{
			pthread_mutex_unlock(&philo->sim->death_mutex);
			pthread_exit(NULL);
		}
		pthread_mutex_unlock(&philo->sim->death_mutex);
		print_action(philo, "is thinking");
		pick_up_forks(philo);
		pthread_mutex_lock(&philo->meal_mutex);
		philo->last_meal_time = cur_time_ms();
		pthread_mutex_unlock(&philo->meal_mutex);
		print_action(philo, "is eating");
		usleep(philo->sim->time_to_eat * 1000);
		philo->times_eaten++;
		release_forks(philo);
		print_action(philo, "is sleeping");
		usleep(philo->sim->time_to_sleep * 1000);
	}
	return (NULL);
}

void	start_simulation(t_simulation *sim)
{
	pthread_t	*threads;
	pthread_t	supervisor_thread;
	int			i;	

	threads = malloc(sizeof(pthread_t) * sim->number_of_philosophers);
	if (!threads)
		print_error("Failed to allocate memory for threads");
	if (pthread_create(&supervisor_thread, NULL, supervisor_routine, sim) != 0)
		print_error("Failed to create supervisor thread");
	i = 0;
	while (i < sim->number_of_philosophers)
	{
		if (pthread_create(&threads[i], NULL
				, philosopher_routine, &sim->philosophers[i]) != 0)
			print_error("Failed to create philosopher thread");
		usleep(10);
		i++;
	}
	i = 0;
	while (i < sim->number_of_philosophers)
		pthread_join(threads[i++], NULL);
	pthread_join(supervisor_thread, NULL);
	free(threads);
}

int	main(int argc, char **argv)
{
	t_simulation	sim;	

	parse_arguments(argc, argv, &sim);
	sim.start_time = cur_time_ms();
	init_forks(&sim);
	init_philosophers(&sim);
	if (sim.number_of_philosophers == 1)
		handle_single_philosopher(&sim.philosophers[0]);
	else
		start_simulation(&sim);
	cleanup_forks(&sim);
	return (0);
}
