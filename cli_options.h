#ifndef CLI_OPTIONS_H
#define CLI_OPTIONS_H

typedef struct CLIOpts {
    unsigned int show_out: 1;
    int repeat_times;
} CLIOpts;

int get_cli_opts(const int argc, char ***argv, CLIOpts *const cli_opts);

#endif // CLI_OPTIONS_H
