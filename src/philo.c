
#include "../include/philo.h"

void *philosopher_routine(void *arg)
{
    t_philosopher *philo = (t_philosopher *)arg;

    while (1)
    {
        // Check if a philosopher has died
        pthread_mutex_lock(&philo->sim->death_mutex);
        if (philo->sim->death_flag == 1)
        {
            pthread_mutex_unlock(&philo->sim->death_mutex);
            pthread_exit(NULL);  // Exit if a philosopher has already died
        }
        pthread_mutex_unlock(&philo->sim->death_mutex);

        long timestamp = current_time_in_ms() - philo->sim->start_time;

        // Thinking
        pthread_mutex_lock(&philo->sim->death_mutex);
        if (philo->sim->death_flag == 0)  // Only print if no philosopher has died
        {
            pthread_mutex_lock(&philo->sim->log_mutex);
            printf("%ld %d is thinking\n", timestamp, philo->id);
            pthread_mutex_unlock(&philo->sim->log_mutex);
        }
        pthread_mutex_unlock(&philo->sim->death_mutex);

        // Fork-picking strategy based on odd/even philosopher ID
        if (philo->id % 2 == 0)
        {
            pthread_mutex_lock(philo->right_fork);
            timestamp = current_time_in_ms() - philo->sim->start_time;

            pthread_mutex_lock(&philo->sim->death_mutex);
            if (philo->sim->death_flag == 0)  // Only print if no philosopher has died
            {
                pthread_mutex_lock(&philo->sim->log_mutex);
                printf("%ld %d has taken a fork\n", timestamp, philo->id);
                pthread_mutex_unlock(&philo->sim->log_mutex);
            }
            pthread_mutex_unlock(&philo->sim->death_mutex);

            pthread_mutex_lock(philo->left_fork);
            timestamp = current_time_in_ms() - philo->sim->start_time;

            pthread_mutex_lock(&philo->sim->death_mutex);
            if (philo->sim->death_flag == 0)
            {
                pthread_mutex_lock(&philo->sim->log_mutex);
                printf("%ld %d has taken a fork\n", timestamp, philo->id);
                pthread_mutex_unlock(&philo->sim->log_mutex);
            }
            pthread_mutex_unlock(&philo->sim->death_mutex);
        }
        else
        {
            pthread_mutex_lock(philo->left_fork);
            timestamp = current_time_in_ms() - philo->sim->start_time;

            pthread_mutex_lock(&philo->sim->death_mutex);
            if (philo->sim->death_flag == 0)
            {
                pthread_mutex_lock(&philo->sim->log_mutex);
                printf("%ld %d has taken a fork\n", timestamp, philo->id);
                pthread_mutex_unlock(&philo->sim->log_mutex);
            }
            pthread_mutex_unlock(&philo->sim->death_mutex);

            pthread_mutex_lock(philo->right_fork);
            timestamp = current_time_in_ms() - philo->sim->start_time;

            pthread_mutex_lock(&philo->sim->death_mutex);
            if (philo->sim->death_flag == 0)
            {
                pthread_mutex_lock(&philo->sim->log_mutex);
                printf("%ld %d has taken a fork\n", timestamp, philo->id);
                pthread_mutex_unlock(&philo->sim->log_mutex);
            }
            pthread_mutex_unlock(&philo->sim->death_mutex);
        }

        // Eating
        philo->last_meal_time = current_time_in_ms();
        timestamp = current_time_in_ms() - philo->sim->start_time;

        pthread_mutex_lock(&philo->sim->death_mutex);
        if (philo->sim->death_flag == 0)
        {
            pthread_mutex_lock(&philo->sim->log_mutex);
            printf("%ld %d is eating\n", timestamp, philo->id);
            pthread_mutex_unlock(&philo->sim->log_mutex);
        }
        pthread_mutex_unlock(&philo->sim->death_mutex);

        usleep(philo->sim->time_to_eat * 1000);
        philo->times_eaten++;

        // Check if philosophers ate enough times
        if (philo->sim->is_optional_arg_present && philo->times_eaten >= philo->sim->number_of_times_each_philosopher_must_eat)
        {
            pthread_mutex_unlock(philo->left_fork);
            pthread_mutex_unlock(philo->right_fork);
            pthread_exit(NULL);
        }

        // Release forks
        pthread_mutex_unlock(philo->left_fork);
        pthread_mutex_unlock(philo->right_fork);

        // Sleeping
        timestamp = current_time_in_ms() - philo->sim->start_time;

        pthread_mutex_lock(&philo->sim->death_mutex);
        if (philo->sim->death_flag == 0)
        {
            pthread_mutex_lock(&philo->sim->log_mutex);
            printf("%ld %d is sleeping\n", timestamp, philo->id);
            pthread_mutex_unlock(&philo->sim->log_mutex);
        }
        pthread_mutex_unlock(&philo->sim->death_mutex);

        usleep(philo->sim->time_to_sleep * 1000);

        // Check for death condition
        if (current_time_in_ms() - philo->last_meal_time > philo->sim->time_to_die)
        {
            timestamp = current_time_in_ms() - philo->sim->start_time;

            pthread_mutex_lock(&philo->sim->death_mutex);
            if (philo->sim->death_flag == 0)  // Only print the first "died" message
            {
                pthread_mutex_lock(&philo->sim->log_mutex);
                printf("%ld %d died\n", timestamp, philo->id);
                pthread_mutex_unlock(&philo->sim->log_mutex);

                // Set death flag
                philo->sim->death_flag = 1;
            }
            pthread_mutex_unlock(&philo->sim->death_mutex);

            pthread_exit(NULL);
        }
    }

    return NULL;
}

void start_simulation(t_philosopher *philosophers, t_simulation *sim)
{
    pthread_t *threads;
    threads = malloc(sizeof(pthread_t) * sim->number_of_philosophers);

    if (!threads)
        print_error("Failed to allocate memory for threads");

    for (int i = 0; i < sim->number_of_philosophers; i++)
    {
        if (pthread_create(&threads[i], NULL, philosopher_routine, &philosophers[i]) != 0)
            print_error("Failed to create philosopher thread");
    }

    // Continuously check if a philosopher has died
    while (1)
    {
        pthread_mutex_lock(&sim->death_mutex);
        if (sim->death_flag == 1)
        {
            pthread_mutex_unlock(&sim->death_mutex);
            break;  // Exit loop if a philosopher has died
        }
        pthread_mutex_unlock(&sim->death_mutex);
        usleep(1000);  // Sleep for a short while before checking again
    }

    // Join all threads
    for (int i = 0; i < sim->number_of_philosophers; i++)
    {
        pthread_join(threads[i], NULL);
    }

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
