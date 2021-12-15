#include "akhil_ls.h"

int main(int argc, char* argv[]) {
    // To handle a new option, we need to add a new flag here, a new input into
    // parse_options, and another case in the switch statement to handle it.
    bool flag_a, flag_h, flag_l;
    flag_a = flag_h = flag_l = false;
    parse_options(argc, argv, "lah", &flag_a, &flag_l, &flag_h);

    bool have_parsed_options = false;

    for (int i = 1; i < argc; i++) {
        // If this statement is true we are currently still parsing the options
        // in the argv array.
        if (argv[i][0] == '-' && !have_parsed_options) {
            continue;
        }

        have_parsed_options = true;
        lookup_dir(argv[i], flag_a, flag_l, flag_h);
    }

    if (!have_parsed_options) {
        // We received no directories to print, we must print the current directory
        lookup_dir(".", flag_a, flag_l, flag_h);
    }

    return 0;
}