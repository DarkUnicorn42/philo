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
    t_simulation *sim = (t_simulation *)arg;

    while (1)
    {
        pthread_mutex_lock(&sim->death_mutex);
        if (sim->death_flag)
        {
            pthread_mutex_unlock(&sim->death_mutex);
            break;
        }
        pthread_mutex_unlock(&sim->death_mutex);

        int finished_philosophers = 0;  // Keep track of how many have eaten enough

        for (int i = 0; i < sim->number_of_philosophers; i++)
        {
            // Lock the individual philosopher's meal_mutex
            pthread_mutex_lock(&sim->philosophers[i].meal_mutex);
            long time_since_last_meal = current_time_in_ms() - sim->philosophers[i].last_meal_time;

            // Check if the philosopher has died
            if (time_since_last_meal > sim->time_to_die)
            {
                pthread_mutex_lock(&sim->death_mutex);
                print_action(&sim->philosophers[i], "died");
                sim->death_flag = 1;
                pthread_mutex_unlock(&sim->death_mutex);
                pthread_mutex_unlock(&sim->philosophers[i].meal_mutex);
                return (NULL);
            }

            // Check if this philosopher has eaten enough times
            if (sim->is_optional_arg_present && sim->philosophers[i].times_eaten >= sim->number_of_times_each_philosopher_must_eat)
            {
                finished_philosophers++;
            }
            pthread_mutex_unlock(&sim->philosophers[i].meal_mutex);
        }

        // Check if all philosophers have eaten enough times
        if (sim->is_optional_arg_present && finished_philosophers == sim->number_of_philosophers)
        {
            pthread_mutex_lock(&sim->death_mutex);
            sim->death_flag = 1;  // Set death flag to stop the simulation
            pthread_mutex_unlock(&sim->death_mutex);
            printf("All philosophers have eaten enough times. Exiting...\n");
            return NULL;  // Exit the supervisor thread
        }

       // usleep(20);  // Small delay to reduce CPU usage
    }

    return NULL;
}

void pick_up_forks(t_philosopher *philo)
{
    t_simulation *sim = philo->sim;
    int next_philo_id, prev_philo_id;
    int wait_time = 0;
    int max_wait_time = 5000;  // 5 seconds timeout for fairness

    // Handle circular neighbor correctly: if last philosopher, next is the first one (ID 1)
    if (philo->id == sim->number_of_philosophers)
        next_philo_id = 0;  // Last philosopher's neighbor is the first one
    else
        next_philo_id = philo->id;  // Otherwise, the next philosopher is the next in sequence

    // Handle circular neighbor for the previous philosopher: if first philosopher, previous is the last one
    if (philo->id == 1)
        prev_philo_id = sim->number_of_philosophers - 1;  // First philosopher's previous neighbor is the last one
    else
        prev_philo_id = philo->id - 2;  // Otherwise, the previous philosopher is the previous in sequence

    while (1)
    {
        // Check if the next philosopher is hungrier
        pthread_mutex_lock(&sim->philosophers[next_philo_id].meal_mutex);
        long time_since_last_meal_next = current_time_in_ms() - sim->philosophers[next_philo_id].last_meal_time;
        pthread_mutex_unlock(&sim->philosophers[next_philo_id].meal_mutex);

        // Check if the previous philosopher is hungrier
        pthread_mutex_lock(&sim->philosophers[prev_philo_id].meal_mutex);
        long time_since_last_meal_prev = current_time_in_ms() - sim->philosophers[prev_philo_id].last_meal_time;
        pthread_mutex_unlock(&sim->philosophers[prev_philo_id].meal_mutex);

        // Get current philosopher's last meal time
        pthread_mutex_lock(&philo->meal_mutex);
        long time_since_last_meal_current = current_time_in_ms() - philo->last_meal_time;
        pthread_mutex_unlock(&philo->meal_mutex);

        // If either the next or previous philosopher is hungrier, wait before picking up forks
        if ((time_since_last_meal_next > time_since_last_meal_current || time_since_last_meal_prev > time_since_last_meal_current) && wait_time < max_wait_time)
        {
            usleep(420);  // Wait for the hungrier philosopher to eat first
            wait_time += 420;
        }
        else
        {
            // If the philosopher has waited too long, proceed regardless
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
            break;
        }
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

    if (philo->sim->number_of_philosophers == 1)
    {
        handle_single_philosopher(philo);
        pthread_exit(NULL);
    }
    while (1)
    {
        pthread_mutex_lock(&philo->sim->death_mutex);
        if (philo->sim->death_flag == 1)
        {
            pthread_mutex_unlock(&philo->sim->death_mutex);
            pthread_exit(NULL);
        }
        pthread_mutex_unlock(&philo->sim->death_mutex);

        // Thinking
        print_action(philo, "is thinking");

        // Pick up forks
        pick_up_forks(philo);

        // Eating
        pthread_mutex_lock(&philo->meal_mutex);
        philo->last_meal_time = current_time_in_ms();
        pthread_mutex_unlock(&philo->meal_mutex);

        print_action(philo, "is eating");
        usleep(philo->sim->time_to_eat * 1000);
        philo->times_eaten++;

        // Release forks
        release_forks(philo);

        // Sleeping
        print_action(philo, "is sleeping");
        usleep(philo->sim->time_to_sleep * 1000);
    }
    return NULL;
}


void start_simulation(t_simulation *sim)
{
    pthread_t *threads;
    pthread_t supervisor_thread;

    threads = malloc(sizeof(pthread_t) * sim->number_of_philosophers);
    if (!threads)
        print_error("Failed to allocate memory for threads");

    // Create the supervisor thread
    if (pthread_create(&supervisor_thread, NULL, supervisor_routine, sim) != 0)
        print_error("Failed to create supervisor thread");

    // Create the philosopher threads
    for (int i = 0; i < sim->number_of_philosophers; i++)
    {
        if (pthread_create(&threads[i], NULL, philosopher_routine, &sim->philosophers[i]) != 0)
            print_error("Failed to create philosopher thread");
        usleep(10);
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

    parse_arguments(argc, argv, &sim);
    sim.start_time = current_time_in_ms();
    init_forks(&sim);
    init_philosophers(&sim);
    start_simulation(&sim);
    cleanup_forks(&sim);
    return (0);
}
