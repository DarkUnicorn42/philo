
#include "../include/philo.h"

int get_next_philosopher_id(t_philosopher *philo)
{
    t_simulation *sim = philo->sim;

    if (philo->id == sim->number_of_philosophers)
        return (0);
    return (philo->id);
}

int get_previous_philosopher_id(t_philosopher *philo)
{
    t_simulation *sim = philo->sim;

    if (philo->id == 1)
        return (sim->number_of_philosophers - 1);
    return (philo->id - 2);
}

int is_neighbor_hungrier(t_philosopher *philo, int next_philo_id, int prev_philo_id)
{
    t_simulation *sim = philo->sim;
    long time_since_last_meal_next;
    long time_since_last_meal_prev;
    long time_since_last_meal_current;

    pthread_mutex_lock(&sim->philosophers[next_philo_id].meal_mutex);
    time_since_last_meal_next = current_time_in_ms() - sim->philosophers[next_philo_id].last_meal_time;
    pthread_mutex_unlock(&sim->philosophers[next_philo_id].meal_mutex);
    pthread_mutex_lock(&sim->philosophers[prev_philo_id].meal_mutex);
    time_since_last_meal_prev = current_time_in_ms() - sim->philosophers[prev_philo_id].last_meal_time;
    pthread_mutex_unlock(&sim->philosophers[prev_philo_id].meal_mutex);
    pthread_mutex_lock(&philo->meal_mutex);
    time_since_last_meal_current = current_time_in_ms() - philo->last_meal_time;
    pthread_mutex_unlock(&philo->meal_mutex);
    return (time_since_last_meal_next > time_since_last_meal_current ||
            time_since_last_meal_prev > time_since_last_meal_current);
}

void lock_forks(t_philosopher *philo)
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

void pick_up_forks(t_philosopher *philo)
{
    int next_philo_id;
    int prev_philo_id;
    int wait_time;
    const int max_wait_time = 4200;

    next_philo_id = get_next_philosopher_id(philo);
    prev_philo_id = get_previous_philosopher_id(philo);
    wait_time = 0;
    while (1)
    {
        if (is_neighbor_hungrier(philo, next_philo_id, prev_philo_id) && wait_time < max_wait_time)
        {
            usleep(420);
            wait_time += 420;
        }
        else
        {
            lock_forks(philo);
            break;
        }
    }
}
