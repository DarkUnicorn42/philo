

#include "../include/philo.h"

// void *supervisor_routine(void *arg)
// {
//     t_simulation *sim = (t_simulation *)arg;

//     while (1)
//     {
//         pthread_mutex_lock(&sim->death_mutex);
//         if (sim->death_flag)
//         {
//             pthread_mutex_unlock(&sim->death_mutex);
//             break;
//         }
//         pthread_mutex_unlock(&sim->death_mutex);

//         int finished_philosophers = 0;  // Keep track of how many have eaten enough

//         for (int i = 0; i < sim->number_of_philosophers; i++)
//         {
//             // Lock the individual philosopher's meal_mutex
//             pthread_mutex_lock(&sim->philosophers[i].meal_mutex);
//             long time_since_last_meal = current_time_in_ms() - sim->philosophers[i].last_meal_time;

//             // Check if the philosopher has died
//             if (time_since_last_meal > sim->time_to_die)
//             {
//                 pthread_mutex_lock(&sim->death_mutex);
//                 print_action(&sim->philosophers[i], "died");
//                 sim->death_flag = 1;
//                 pthread_mutex_unlock(&sim->death_mutex);
//                 pthread_mutex_unlock(&sim->philosophers[i].meal_mutex);
//                 return (NULL);
//             }

//             // Check if this philosopher has eaten enough times
//             if (sim->is_optional_arg_present && sim->philosophers[i].times_eaten >= sim->number_of_times_each_philosopher_must_eat)
//             {
//                 finished_philosophers++;
//             }
//             pthread_mutex_unlock(&sim->philosophers[i].meal_mutex);
//         }

//         // Check if all philosophers have eaten enough times
//         if (sim->is_optional_arg_present && finished_philosophers == sim->number_of_philosophers)
//         {
//             pthread_mutex_lock(&sim->death_mutex);
//             sim->death_flag = 1;  // Set death flag to stop the simulation
//             pthread_mutex_unlock(&sim->death_mutex);
//             printf("All philosophers have eaten enough times. Exiting...\n");
//             return NULL;  // Exit the supervisor thread
//         }

//        // usleep(20);  // Small delay to reduce CPU usage
//     }

//     return (NULL);
// }

int check_philosopher_death(t_simulation *sim, int i)
{
    long time_since_last_meal;

    pthread_mutex_lock(&sim->philosophers[i].meal_mutex);
    time_since_last_meal = current_time_in_ms() - sim->philosophers[i].last_meal_time;
    if (time_since_last_meal > sim->time_to_die)
    {
        pthread_mutex_lock(&sim->death_mutex);
        print_action(&sim->philosophers[i], "died");
        sim->death_flag = 1;
        pthread_mutex_unlock(&sim->death_mutex);
        pthread_mutex_unlock(&sim->philosophers[i].meal_mutex);
        return (1);
    }
    pthread_mutex_unlock(&sim->philosophers[i].meal_mutex);
    return (0);
}

int check_philosopher_meals(t_simulation *sim)
{
    int i;
    int finished_philosophers;

    finished_philosophers = 0;
    for (i = 0; i < sim->number_of_philosophers; i++)
    {
        pthread_mutex_lock(&sim->philosophers[i].meal_mutex);
        if (sim->is_optional_arg_present &&
            sim->philosophers[i].times_eaten >= sim->number_of_times_each_philosopher_must_eat)
        {
            finished_philosophers++;
        }
        pthread_mutex_unlock(&sim->philosophers[i].meal_mutex);
    }
    return (finished_philosophers == sim->number_of_philosophers);
}

void check_philosopher_status(t_simulation *sim)
{
    int i;

    for (i = 0; i < sim->number_of_philosophers; i++)
    {
        if (check_philosopher_death(sim, i))
            return;
    }
}

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
        check_philosopher_status(sim);
        if (check_philosopher_meals(sim))
        {
            pthread_mutex_lock(&sim->death_mutex);
            sim->death_flag = 1;
            pthread_mutex_unlock(&sim->death_mutex);
            printf("All philosophers have eaten enough times. Exiting...\n");
            return (NULL);
        }
       //usleep(20);
    }
    return (NULL);
}

void handle_single_philosopher(t_philosopher *philo)
{
    print_action(philo, "is thinking");
    pthread_mutex_lock(philo->left_fork);
    print_action(philo, "has taken a fork");
    usleep(philo->sim->time_to_die * 1000);
    print_action(philo, "died");
    pthread_mutex_unlock(philo->left_fork);
}