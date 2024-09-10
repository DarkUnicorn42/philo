/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mwojtcza <mwojtcza@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/17 12:55:34 by mwojtcza          #+#    #+#             */
/*   Updated: 2024/07/17 12:55:34 by mwojtcza         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PHILO_H
# define PHILO_H

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>

#define THINKING 0
#define EATING 1
#define SLEEPING 2

struct s_simulation;  // Forward declaration of simulation struct

typedef struct s_philosopher
{
    int id;
    int times_eaten;
    long last_meal_time;
    pthread_mutex_t *left_fork;
    pthread_mutex_t *right_fork;
    pthread_mutex_t meal_mutex;
    struct s_simulation *sim;  // Reference to simulation struct
} t_philosopher;

typedef struct s_simulation
{
    int number_of_philosophers;
    int time_to_die;
    int time_to_eat;
    int time_to_sleep;
    int number_of_times_each_philosopher_must_eat;
    int is_optional_arg_present;
    long start_time;

    pthread_mutex_t *forks;
    pthread_mutex_t log_mutex;
    pthread_mutex_t death_mutex;

    int death_flag;
    int finished_philosophers;

    t_philosopher *philosophers;  // Array of philosopher structs
} t_simulation;


void handle_single_philosopher(t_philosopher *philo);
void print_action(t_philosopher *philo, const char *action);
void pick_up_forks(t_philosopher *philo);
void release_forks(t_philosopher *philo);
int check_death(t_philosopher *philo);

// Function prototypes
long current_time_in_ms();  
int ft_atoi(const char *nptr);
void parse_arguments(int argc, char **argv, t_simulation *sim);
void print_error(char *message);
void start_simulation(t_simulation *sim);
void *philosopher_routine(void *arg);
void init_philosophers(t_simulation *sim);
void cleanup_forks(t_simulation *sim);
void init_forks(t_simulation *sim);

#endif