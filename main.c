#include "cli_options.h"

#include <fcntl.h>
#include <stdio.h>
#include <sys/resource.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

#define NS_IN_US 1000 
#define NS_IN_S 1000000000

typedef struct RUsageAvg {
    struct rusage rusage;
    int count;
} RUsageAvg;

typedef struct RealTimeAvg {
    struct timespec sum;
    int count;
} RealTimeAvg;

void calculate_real_time_difference(const struct timespec *const begin,
                                    const struct timespec *const end,
                                    struct timespec *const intermediate_result,
                                    RealTimeAvg *const real_time_avg);

int main(int argc, char **argv) {
    CLIOpts cli_options;
    int ret_val;

    if (0 == (ret_val = get_cli_opts(argc, &argv, &cli_options))) {
        RealTimeAvg real_time_avg;
        RUsageAvg rusage_avg;

        real_time_avg.sum.tv_sec = 0;
        real_time_avg.sum.tv_sec = 0;
        real_time_avg.count = 0;

        rusage_avg.rusage.ru_utime.tv_sec = 0;
        rusage_avg.rusage.ru_utime.tv_usec = 0;
        rusage_avg.rusage.ru_stime.tv_sec = 0;
        rusage_avg.rusage.ru_stime.tv_usec = 0;
        rusage_avg.count = 0;

        for (int i = 0; cli_options.repeat_times > i; ++i) {
            struct timespec begin;
            struct timespec end;
            struct timespec intermediate_result;
            struct rusage rusage;

            clock_gettime(CLOCK_REALTIME, &begin);
            pid_t pid = fork();
            if (0 < pid) {
                int status;
                wait4(pid, &status, 0, &rusage);
                
                clock_gettime(CLOCK_REALTIME, &end);
                calculate_real_time_difference(&begin, &end, &intermediate_result, &real_time_avg);
                rusage_avg.rusage.ru_utime.tv_sec += rusage.ru_utime.tv_sec;
                rusage_avg.rusage.ru_utime.tv_usec += rusage.ru_utime.tv_usec;
                rusage_avg.rusage.ru_stime.tv_sec += rusage.ru_stime.tv_sec;
                rusage_avg.rusage.ru_stime.tv_usec += rusage.ru_stime.tv_usec;
                rusage_avg.count++;
                
                if (1 != cli_options.repeat_times) {
                    if (0 < i) {
                        printf("\n");
                    }
                    printf("i = %d\n", i);
                }
                printf("real time: %lds %ldus\n", intermediate_result.tv_sec, intermediate_result.tv_nsec / NS_IN_US);
                printf("user CPU time: %lds %ldus\n", rusage.ru_utime.tv_sec, rusage.ru_utime.tv_usec);
                printf("system CPU time: %lds %ldus\n", rusage.ru_stime.tv_sec, rusage.ru_stime.tv_usec);
            } else {
                if (!cli_options.show_out) {
                    close(1);
                    close(2);
                    int out = open("/dev/null", O_WRONLY);
                    dup2(out, 1);
                    dup2(out, 2);
                }
                execvp(argv[optind], &argv[optind]);
            }
        }
        if (1 != cli_options.repeat_times) {
            printf("\n");
            printf("Average:\n");
            printf("real time: %lds %ldus\n", real_time_avg.sum.tv_sec / real_time_avg.count, real_time_avg.sum.tv_nsec / real_time_avg.count / NS_IN_US);
            printf("user CPU time: %lds %ldus\n", rusage_avg.rusage.ru_utime.tv_sec / rusage_avg.count, rusage_avg.rusage.ru_utime.tv_usec / rusage_avg.count);
            printf("system CPU time: %lds %ldus\n", rusage_avg.rusage.ru_stime.tv_sec / rusage_avg.count, rusage_avg.rusage.ru_stime.tv_usec / rusage_avg.count);
        }
    }

    return ret_val;
}

void calculate_real_time_difference(const struct timespec *const begin,
                                    const struct timespec *const end,
                                    struct timespec *const intermediate_result,
                                    RealTimeAvg *const real_time_avg) {
    if (begin && end && intermediate_result && real_time_avg) {
        intermediate_result->tv_sec = end->tv_sec - begin->tv_sec;
        intermediate_result->tv_nsec = end->tv_nsec - begin->tv_nsec;
        if (0 > intermediate_result->tv_nsec) {
            intermediate_result->tv_sec -= 1;
            intermediate_result->tv_nsec = NS_IN_S - intermediate_result->tv_nsec;
        }
        real_time_avg->sum.tv_sec += intermediate_result->tv_sec;
        real_time_avg->sum.tv_nsec += intermediate_result->tv_nsec;
        real_time_avg->count++;
    }
}
