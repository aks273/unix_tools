#include <stdlib.h>
#include <stdio.h>

int main(void) {
    char* current_dir_path = ".";
    char full_path[100];
    printf("%s\n", realpath(current_dir_path, full_path));
}