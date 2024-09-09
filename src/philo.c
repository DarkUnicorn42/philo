/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mwojtcza <mwojtcza@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/09 13:40:27 by mwojtcza          #+#    #+#             */
/*   Updated: 2024/09/09 14:04:46 by mwojtcza         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/philo.h"

void *supervisor_routine(void *arg)
{
    t_philosopher *philosophers = (t_philosopher *)arg;
    t_simulation *sim = philosophers[0].sim;

    while (1)
    {
        pthread_mutex_lock(&sim->death_mutex);
        if (sim->death_flag)
        {
            pthread_mutex_unlock(&sim->death_mutex);
            break;
        }
        pthread_mutex_unlock(&sim->death_mutex);

        int finished_philosophers = 0;

        for (int i = 0; i < sim->number_of_philosophers; i++)
        {
            pthread_mutex_lock(&sim->meal_mutex);
            long time_since_last_meal = current_time_in_ms() - philosophers[i].last_meal_time;

            // Check if a philosopher has died
            if (time_since_last_meal > sim->time_to_die)
            {
                pthread_mutex_lock(&sim->death_mutex);
                print_action(&philosophers[i], "died");
                sim->death_flag = 1;

                pthread_mutex_unlock(&sim->death_mutex);
                pthread_mutex_unlock(&sim->meal_mutex);

                return NULL;  // Exit the supervisor thread after printing the death message
            }

            // Check if a philosopher has eaten enough times
            if (sim->is_optional_arg_present && philosophers[i].times_eaten >= sim->number_of_times_each_philosopher_must_eat)
            {
                finished_philosophers++;
            }

            pthread_mutex_unlock(&sim->meal_mutex);
        }

        // If all philosophers have eaten enough, print message and exit
        if (sim->is_optional_arg_present && finished_philosophers == sim->number_of_philosophers)
        {
            pthread_mutex_lock(&sim->death_mutex);
            // pthread_mutex_lock(&sim->log_mutex);
            printf("All philosophers have eaten enough times. Exiting...\n");
            sim->death_flag = 1;  // Set death flag to stop the simulation
            // pthread_mutex_unlock(&sim->log_mutex);
            pthread_mutex_unlock(&sim->death_mutex);
            return NULL;  // Exit the supervisor thread
        }

        usleep(100);  // Small delay to reduce CPU usage
    }

    return NULL;
}

// Helper function to pick up forks
void pick_up_forks(t_philosopher *philo)
{
    if (philo->id % 2 == 0)
    {
        pthread_mutex_lock(philo->right_fork);
        print_action(philo, "has taken a fork");
        pthread_mutex_lock(philo->left_fork);
        print_action(philo, "has taken a fork");
    }
    else
    {
        pthread_mutex_lock(philo->left_fork);
        print_action(philo, "has taken a fork");
        pthread_mutex_lock(philo->right_fork);
        print_action(philo, "has taken a fork");
    }
}

// Helper function to release forks
void release_forks(t_philosopher *philo)
{
    pthread_mutex_unlock(philo->left_fork);
    pthread_mutex_unlock(philo->right_fork);
}

void *philosopher_routine(void *arg)
{
    t_philosopher *philo = (t_philosopher *)arg;

    // Special case: one philosopher
    if (philo->sim->number_of_philosophers == 1)
    {
        handle_single_philosopher(philo);
        pthread_exit(NULL);
    }

    while (1)
    {
        // Check if a philosopher has already died
        pthread_mutex_lock(&philo->sim->death_mutex);
        if (philo->sim->death_flag == 1)
        {
            pthread_mutex_unlock(&philo->sim->death_mutex);
            // printf("Philosopher %d died\n", philo->id);
            pthread_exit(NULL);  // Exit if the death flag is set
        }
        pthread_mutex_unlock(&philo->sim->death_mutex);

        // Thinking
        print_action(philo, "is thinking");

        // Pick up forks
        pick_up_forks(philo);

        // Eating
        pthread_mutex_lock(&philo->sim->meal_mutex);
        philo->last_meal_time = current_time_in_ms();  // Update last meal time
        pthread_mutex_unlock(&philo->sim->meal_mutex);

        print_action(philo, "is eating");
        usleep(philo->sim->time_to_eat * 1000);
        philo->times_eaten++;

        // // Check if the philosopher has eaten enough times
        // if (philo->sim->is_optional_arg_present && philo->times_eaten >= philo->sim->number_of_times_each_philosopher_must_eat)
        // {
        //     release_forks(philo);
            
        //     // Increment the finished philosophers count
        //     pthread_mutex_lock(&philo->sim->death_mutex);
        //     philo->sim->finished_philosophers++;
        //     if (philo->sim->finished_philosophers == philo->sim->number_of_philosophers)
        //     {
                
        //         philo->sim->death_flag = 1;  // Set death_flag to stop the simulation if all have finished
        //     }
        //     pthread_mutex_unlock(&philo->sim->death_mutex);

        //     pthread_exit(NULL);  // Exit if the philosopher has eaten enough times
        // }

        // Release forks
        release_forks(philo);

        // Sleeping
        print_action(philo, "is sleeping");
        usleep(philo->sim->time_to_sleep * 1000);
    }

    return NULL;
}

void start_simulation(t_philosopher *philosophers, t_simulation *sim)
{
    pthread_t *threads;
    pthread_t supervisor_thread;
    threads = malloc(sizeof(pthread_t) * sim->number_of_philosophers);

    if (!threads)
        print_error("Failed to allocate memory for threads");

    // Create the supervisor thread
    if (pthread_create(&supervisor_thread, NULL, supervisor_routine, philosophers) != 0)
        print_error("Failed to create supervisor thread");

    // Create the philosopher threads
    for (int i = 0; i < sim->number_of_philosophers; i++)
    {
        if (pthread_create(&threads[i], NULL, philosopher_routine, &philosophers[i]) != 0)
            print_error("Failed to create philosopher thread");
    }

    // Join all philosopher threads
    for (int i = 0; i < sim->number_of_philosophers; i++)
    {
        pthread_join(threads[i], NULL);
    }

    // Join the supervisor thread
    pthread_join(supervisor_thread, NULL);

    free(threads);
}



int main(int argc, char **argv)
{
    t_simulation sim;
    t_philosopher *philosophers;

    // Parse the arguments
    parse_arguments(argc, argv, &sim);

    // Record the start time of the simulation
    sim.start_time = current_time_in_ms();

    // Allocate memory for philosophers
    philosophers = malloc(sizeof(t_philosopher) * sim.number_of_philosophers);
    if (!philosophers)
        print_error("Failed to allocate memory for philosophers");

    // Initialize forks and philosophers
    init_forks(&sim);
    init_philosophers(philosophers, &sim);

    // Start the simulation
    start_simulation(philosophers, &sim);

    // Clean up forks and philosophers
    cleanup_forks(&sim);
    free(philosophers);

    return 0;
}
