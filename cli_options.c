#include "cli_options.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int get_cli_opts(const int argc, char ***argv, CLIOpts *const cli_options) {
    int flag;
    int ret_val = 0;

    if (argv && cli_options) {
        cli_options->show_out = 0u;
        cli_options->repeat_times = 1;
        while (-1 != (flag = getopt(argc, *argv, "+sr:"))) {
            switch (flag) {
                case 's':
                    cli_options->show_out = 1u;
                    break;
                case 'r':
                    int times = atoi(optarg);
                    cli_options->repeat_times = 0 < times ? times : 1;
                    break;
                case '?':
                    if ('r' == optopt) {
                        fprintf(stderr, "Option '-%c' requires an argument.\n", optopt);
                    } else {
                        fprintf(stderr, "Unknown option '-%c'.\n", optopt);
                    }
                    // flows through
                default:
                    ret_val = 1;
            }
        }
    } else {
        ret_val = 2;
    }

    return ret_val;
}
