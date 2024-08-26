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
		printf("Error: number of philo, time to die, time to eat, time to sleep, [times to eat]\n");
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
        printf("Error: Invalid argument values.\n");
        exit(EXIT_FAILURE);
    }
}

void *philosopher_life(void *arg)
{
    t_philosopher *philo = (t_philosopher *)arg;
    t_data *data = philo->data;

    while (1)
    {
        // Thinking
        print_status(philo, "is thinking");

        // Take forks
        pthread_mutex_lock(&data->forks[philo->id]);
        print_status(philo, "has taken a fork");
        pthread_mutex_lock(&data->forks[(philo->id + 1) % data->number_of_philosophers]);
        print_status(philo, "has taken a fork");

        // Eating
        print_status(philo, "is eating");
        philo->last_meal_time = current_timestamp();
        usleep(data->time_to_eat * 1000);
        philo->times_eaten++;

        // Put down forks
        pthread_mutex_unlock(&data->forks[(philo->id + 1) % data->number_of_philosophers]);
        pthread_mutex_unlock(&data->forks[philo->id]);

        // Sleeping
        print_status(philo, "is sleeping");
        usleep(data->time_to_sleep * 1000);
    }
    return (NULL);
}

void monitor_philosophers(t_philosopher *philos, t_data *data)
{
    while (1)
    {
        for (int i = 0; i < data->number_of_philosophers; i++)
        {
            if ((current_timestamp() - philos[i].last_meal_time) > data->time_to_die)
            {
                print_status(&philos[i], "died");
                return;
            }
        }
        usleep(1000);
    }
}

int main(int ac, char **av)
{
    t_data data;
    t_philosopher *philos;
    int i;

    parse_arg(ac, av, &data);  // Step 2: Parse arguments and initialize data structure
    data.start_time = current_timestamp();  // Initialize start time
    data.forks = malloc(sizeof(pthread_mutex_t) * data.number_of_philosophers);  // Allocate memory for forks
    for (i = 0; i < data.number_of_philosophers; i++)
        pthread_mutex_init(&data.forks[i], NULL);  // Initialize each fork mutex
    pthread_mutex_init(&data.print_lock, NULL);  // Initialize print lock mutex

    philos = malloc(sizeof(t_philosopher) * data.number_of_philosophers);  // Allocate memory for philosophers
    for (i = 0; i < data.number_of_philosophers; i++)
    {
        philos[i].id = i;  // Assign ID to each philosopher
        philos[i].times_eaten = 0;  // Initialize times eaten
        philos[i].last_meal_time = data.start_time;  // Initialize last meal time
        philos[i].data = &data;  // Link to shared data
        pthread_create(&philos[i].thread, NULL, philosopher_life, &philos[i]);  // Create philosopher thread
    }

    monitor_philosophers(philos, &data);  // Monitor philosophers for death

    for (i = 0; i < data.number_of_philosophers; i++)
        pthread_mutex_destroy(&data.forks[i]);  // Destroy fork mutexes
    pthread_mutex_destroy(&data.print_lock);  // Destroy print lock mutex
    free(data.forks);  // Free allocated memory for forks
    free(philos);  // Free allocated memory for philosophers
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