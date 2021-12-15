#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
#include <stdbool.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>

void parse_options(
    int argc, char* argv[], char* options_str, bool* flag_a, bool* flag_l, bool* flag_h
) {
    char next_option;
    while ((next_option = getopt(argc, argv, "lah")) != -1) {
        switch (next_option) {
        case 'a':
            *flag_a = true;
            break;
        case 'l':
            *flag_l = true;
            break;
        case 'h':
            *flag_h = true;
            break;
        }
    }
}

char* get_terminator(bool flag_l, int count) {
    if (flag_l || count % 5 == 0) {
        return "\n";
    } else {
        return "\t\t";
    }
}

// When the l flag is on we print (permissions, user, group, size, date) before
// the name
void print_extra_into(struct dirent* dir_info, bool flag_l, bool flag_h) {
    if (!flag_l)
        return;

    struct stat *file_info;
    file_info = malloc(sizeof(struct stat));
    stat(dir_info->d_name, file_info);

    // permissions
    int permissions = file_info->st_mode;
    printf(S_ISDIR(permissions) ? "d" : "-");
    printf((permissions & S_IRUSR) ? "r" : "-");
    printf((permissions & S_IWUSR) ? "w" : "-");
    printf((permissions & S_IXUSR) ? "x" : "-");
    printf((permissions & S_IRGRP) ? "r" : "-");
    printf((permissions & S_IWGRP) ? "w" : "-");
    printf((permissions & S_IXGRP) ? "x" : "-");
    printf((permissions & S_IROTH) ? "r" : "-");
    printf((permissions & S_IWOTH) ? "w" : "-");
    printf((permissions & S_IXOTH) ? "x" : "-");
    printf("\t");

    // group and user name
    struct passwd *user;
    struct group *grp;
    user = getpwuid(file_info->st_uid);
    grp = getgrgid(file_info->st_gid);
    printf("%s\t", user->pw_name);
    printf("%s\t", grp->gr_name);

    // file size in bytes
    if (flag_h) {
        if (file_info->st_size < 1000)
            printf("%lldB\t", file_info->st_size);
        else if (file_info->st_size < 1000000)
            printf("%.lldK\t", ((file_info->st_size)/1000));
    }
    else
        printf("%lld\t", file_info->st_size);

    // time file last modified and remove the newline at the end of it.
    char* time_last_modified = ctime(&(file_info->st_mtime));
    time_last_modified[strcspn(time_last_modified, "\r\n")] = '\0';
    printf("%s\t", time_last_modified);

    free(file_info);
}

void print_directory_contents(DIR* directory, bool flag_a, bool flag_l, bool flag_h) {
    int count = 1;
    struct dirent* dir_info;
    while ((dir_info = readdir(directory)) != NULL) {
        if (!flag_a  && dir_info->d_name[0] == '.') {
            continue;
        }

        print_extra_into(dir_info, flag_l, flag_h);

        char* terminator = get_terminator(flag_l, count);
        printf("%s%s", dir_info->d_name, terminator);
        count++;
    }
}

void lookup_dir(char* directory_path, bool flag_a, bool flag_l, bool flag_h) {
    DIR *lookup_dir = opendir(directory_path);
    if (lookup_dir == NULL) {
        printf("akhil_ls: %s: No such file or directory\n\n", directory_path);
        return;
    }

    printf("%s:\n", directory_path);
    print_directory_contents(lookup_dir, flag_a, flag_l, flag_h);

    closedir(lookup_dir);
    printf("\n\n");
}