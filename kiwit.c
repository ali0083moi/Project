// hints for me:
// line 0 = username
// line 1 = email

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ansi_color.h"
#include "ansi_color.c"
#include "logo_print.h"
#include "logo_print.c"
#include <dirent.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <sys/time.h>

char *root_email;
char *root_username;
char *root_path = "/Users/ali/Documents/Daaneshgah/Term1/FOP/Project/data/";

// file name maker
/*
        char *file_name = malloc(2000);
        strcpy(file_name, line);
        int i = strlen(file_name), j = 0;
        while (file_name[i] != '/') {
            j++;
            i--;
        }
        memmove(file_name, file_path + i + 1, j * sizeof(char));
 */

// hash code maker
//unsigned long hash(char *str) {
//    unsigned long hash = 5381;
//    int c;
//
//    while ((c = *str++)) {
//        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
//    }
//
//    return hash;
//}

void explore_directory(const char *dirPath, FILE *outputFile) {
    DIR *directory = opendir(dirPath);
    if (directory == NULL) {
        perror(_SGR_REDF"Unable to open directory!\n"_SGR_RESET);
        exit(EXIT_FAILURE);
    }
    struct dirent *entry;
    while ((entry = readdir(directory)) != NULL) {
        if (entry->d_type == DT_DIR) {
            // Ignore "." and ".." directories
            if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
                char newPath[1024];
                snprintf(newPath, sizeof(newPath), "%s/%s", dirPath, entry->d_name);

                // Recursively explore subdirectories
                explore_directory(newPath, outputFile);
            }
        } else if (entry->d_type == DT_REG) {
            // Regular file
            if (strcmp(entry->d_name, ".DS_Store") != 0) {
                fprintf(outputFile, "%s/%s\n", dirPath, entry->d_name);
            }
        }
    }
    closedir(directory);
}

//Sarina
int find_file_in_stage(char file_name[]) {
    struct dirent *entry;
    char file_2[200];
    char cwd[500];
    if (getcwd(cwd, sizeof(cwd)) == NULL) {
        return 1;
    }
    snprintf(file_2, sizeof(file_2), "%s/%s", cwd, ".kiwit/staging_files");
    //printf("%s/%s\n", file_2,file_name);
    DIR *dir = opendir(file_2);
    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_type == DT_REG) {
            if (strncmp(entry->d_name, file_name, strlen(file_name)) == 0) {
                return 1;
            }
        }
    }
    return 0;
}

void add_n_recursive(char *dir_path, int depth, int max_depth) {
    DIR *dir = opendir(dir_path);
    struct dirent *entry;
    char entry_path[1024];
    char out[1024];
    out[0] = '\0';
    if (depth == max_depth) {
        return;
    }
    //int flag = 0;
    while ((entry = readdir(dir)) != NULL) {

        if (entry->d_type == DT_DIR) {
            // Ignore "." and ".." directories
            if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0 &&
                strcmp(entry->d_name, ".kiwit") != 0) {
                for (int i = 0; i < depth; ++i) {
                    strcat(out, "\t");
                }
                strcat(out, "└─── ");
                strcpy(entry_path, dir_path);
                strcat(entry_path, "/");
                strcat(entry_path, entry->d_name);
                strcat(out, entry->d_name);

                printf("%s\n", out);
                out[0] = '\0';
                add_n_recursive(entry_path, depth + 1, max_depth);

                //closedir(entry_dir);
            }

        } else if (entry->d_type == DT_REG) {
            // Regular file
            if (strcmp(entry->d_name, ".DS_Store") != 0) {
                for (int i = 0; i < depth; ++i) {
                    strcat(out, "\t");
                }
                strcat(out, "└─── ");
                strcat(out, entry->d_name);
                if (find_file_in_stage(entry->d_name)) {
                    printf(_SGR_GREENF"%s\n", out);
                    printf(_SGR_RESET);
                } else {
                    printf(_SGR_REDF"%s\n", out);
                    printf(_SGR_RESET);
                }
                out[0] = '\0';
            }
        }
    }
    //closedir(dir);
}

void add_n(char *path, int max_depth) {
    printf(_C_L_PURPLE"Hints:\n"_SGR_RESET);
    printf("The directories are White.\n");
    printf(_SGR_GREENF"Staged files are Green.\n"_SGR_RESET);
    printf(_SGR_REDF"Un-staged files are Red.\n"_SGR_RESET);
    printf("\n");
    add_n_recursive(path, 0, max_depth);
    printf("\n");
}

int file_content_checker(FILE *file1, FILE *file2) {
    char c1 = fgetc(file1);
    char c2 = fgetc(file2);
    while (c1 != EOF || c2 != EOF) {
        if (c1 != c2) {
            return 0;
        }
        c1 = fgetc(file1);
        c2 = fgetc(file2);
    }
    return 1;
}

char *find_source() {
    char cwd[2024];
    if (getcwd(cwd, sizeof(cwd)) == NULL) return NULL;
    char *ret = (char *) malloc(2024);
    strcpy(ret, cwd);
    strcat(ret, "/");
    return ret;
}

int is_dir(char *path) {
    struct stat st_buf;
    int status = stat(path, &st_buf);
    if (status != 0) return -1;
    if (S_ISREG(st_buf.st_mode)) return 0;
    if (S_ISDIR(st_buf.st_mode)) return 1;
}

void copy_file(char *source, char *destination) {
    FILE *file1 = fopen(source, "r");
    FILE *file2 = fopen(destination, "w");
    char *line = malloc(1000);
    while (fgets(line, 1000, file1) != NULL) {
        fprintf(file2, "%s", line);
    }
    fclose(file1);
    fclose(file2);
}

char *path_maker(char *path, char *file_name) {
    char *new_path = malloc(strlen(path) + strlen(file_name) + 1);
    strcpy(new_path, path);
    strcat(new_path, file_name);
    return new_path;
}

int create_configs(char *username, char *email) {
    FILE *file = fopen(".kiwit/config", "w");
    if (file == NULL)
        return 1;

    fprintf(file, "%s\n", username);
    fprintf(file, "%s\n", email);
    fprintf(file, "last_commit_ID: %d\n", 0);
    fprintf(file, "current_commit_ID: %d\n", 0);
    fprintf(file, "branch: %s", "master");

    fclose(file);

    // create commits folder
    if (mkdir(".kiwit/commits", 0755) != 0)
        return 1;

    // create files folder
    if (mkdir(".kiwit/staging_files", 0755) != 0)
        return 1;

    if (mkdir(".kiwit/unstaging_files", 0755) != 0)
        return 1;

    if (mkdir(".kiwit/commits/master", 0755) != 0)
        return 1;

    file = fopen(".kiwit/staging", "w");
    fclose(file);

    file = fopen(".kiwit/staging_2", "w");
    fclose(file);

    file = fopen(".kiwit/tracks", "w");
    fclose(file);

    file = fopen(".kiwit/unstaging", "w");
    fclose(file);

    file = fopen(".kiwit/unstaging_2", "w");
    fclose(file);

    file = fopen(".kiwit/commit_ID", "w");
    fprintf(file, "0\n");
    fclose(file);

    file = fopen(".kiwit/commit_message_shortcuts", "w");
    fclose(file);

    file = fopen(".kiwit/current_branch", "w");
    fprintf(file, path_maker(find_source(), ".kiwit/commits/master"));
    fclose(file);

    file = fopen(".kiwit/deleted", "w");
    fclose(file);

    file = fopen(".kiwit/number", "w");
    fprintf(file, "0\n");
    fclose(file);

    copy_file(path_maker(root_path, "root_alias"), ".kiwit/alias");
    logo_print();
    return 0;
}

int run_init(int argc, char *const argv[]) {
    FILE *file = fopen(path_maker(root_path, "root_config"), "r");
    if (file == NULL) {
        printf(_SGR_REDF _SGR_BOLD "Error opening file!\n"_SGR_RESET);
        exit(1);
    }
    char *root_config_line = malloc(1000);
    int line_number = 0;
    bool is_email = false;
    root_email = malloc(1000);
    root_username = malloc(1000);
    while (fgets(root_config_line, 1000, file) != NULL) {
        if (root_config_line[strlen(root_config_line) - 1] == '\n') {
            root_config_line[strlen(root_config_line) - 1] = '\0';
        }
        if (line_number == is_email) {
            strcpy(root_username, root_config_line);
        } else {
            strcpy(root_email, root_config_line);
        }
        line_number++;
    }
    fclose(file);
    if ((root_username != NULL && (int) root_username[0] != 0) && (root_email != NULL && (int) root_email[0] != 0)) {
        char cwd[1024];
        if (getcwd(cwd, sizeof(cwd)) == NULL)
            return 1;
        char tmp_cwd[1024];
        bool exists = false;
        struct dirent *entry;
        do {
            // find .neogit
            DIR *dir = opendir(".");
            if (dir == NULL) {
                perror(_SGR_REDF _SGR_BOLD "Error opening current directory\n"_SGR_RESET);
                return 1;
            }
            while ((entry = readdir(dir)) != NULL) {
                if (entry->d_type == DT_DIR && strcmp(entry->d_name, ".kiwit") == 0)
                    exists = true;
            }
            closedir(dir);

            // update current working directory
            if (getcwd(tmp_cwd, sizeof(tmp_cwd)) == NULL)
                return 1;

            // change cwd to parent
            if (strcmp(tmp_cwd, "/") != 0) {
                if (chdir("..") != 0)
                    return 1;
            }

        } while (strcmp(tmp_cwd, "/") != 0);

        // return to the initial cwd
        if (chdir(cwd) != 0)
            return 1;

        if (!exists) {
            if (mkdir(".kiwit", 0755) != 0)
                return 1;
            //saving ripo path
            bool exist_path = false;
            char *line = malloc(1000);
            FILE *ripo_list = fopen(path_maker(root_path, "ripo_list"), "r");
            while (fgets(line, 1000, ripo_list) != NULL) {
                if (line[strlen(line) - 1] == '\n') {
                    line[strlen(line) - 1] = '\0';
                }
                if (strcmp(line, cwd) == 0) {
                    exist_path = true;
                    break;
                }
            }
            fclose(ripo_list);
            if (!exist_path) {
                ripo_list = fopen(path_maker(root_path, "ripo_list"), "a");
                fprintf(ripo_list, "%s\n", cwd);
                fclose(ripo_list);
            }
            printf(_SGR_GREENF"kiwit did the init function successfully!\n"_SGR_RESET);
            return create_configs(root_username, root_email);
        } else {
            perror(_SGR_REDF _SGR_BOLD "kiwit repository has already initialized\n"_SGR_RESET);
        }
        return 0;
    } else if (root_username == NULL || (int) root_username[0] == 0) {
        printf(_SGR_REDF _SGR_BOLD "You did not set user.name\n"_SGR_RESET);
        printf(_SGR_GREENF "use kiwit config -global user.name <username>\n"_SGR_RESET);
        printf(_SGR_YELLOWF "then you can use kiwit init\n"_SGR_RESET);
        return 0;
    } else if (root_email == NULL || (int) root_email[0] == 0) {
        printf(_SGR_REDF _SGR_BOLD "You did not set user.email\n"_SGR_RESET);
        printf(_SGR_GREENF "use kiwit config -global user.email <email>\n"_SGR_RESET);
        printf(_SGR_YELLOWF "then you can use kiwit init\n"_SGR_RESET);
        return 0;
    } else {
        printf(_SGR_REDF _SGR_BOLD "You did not set user.name and user.email\n"_SGR_RESET);
        printf(_SGR_GREENF "use kiwit config -global user.name <username>\nuse kiwit config -global user.email <email>\n"_SGR_RESET);
        printf(_SGR_YELLOWF "then you can use kiwit init\n"_SGR_RESET);
        return 0;
    }
}

void config_alias(int argc, const char *argv[]) {
    // line 0 = username
    // line 1 = email
    int line_number = 0;
    bool is_email = false;
    FILE *file;
    FILE *file2;
    if (strcmp(argv[2], "-global") == 0) {
        if (argv[4] == NULL || argv[5] == NULL) {
            printf(_SGR_REDB "invalid command\n"_SGR_RESET);
            return;
        }
        file = fopen(path_maker(root_path, "root_alias"), "r");
        if (file == NULL) {
            printf("Error opening file!\n");
            exit(1);
        }
        file2 = fopen(path_maker(root_path, "root_alias2"), "w");
        char *line = malloc(1000);
        while (fgets(line, 1000, file) != NULL) {
            if (line[strlen(line) - 1] == '\n') {
                line[strlen(line) - 1] = '\0';
            }
            fprintf(file2, "%s", line);
            if (strstr(line, argv[5]) != NULL) {
                fprintf(file2, " %s", argv[4]);
            }
            fprintf(file2, "\n");
        }
        fclose(file);
        fclose(file2);
        remove(path_maker(root_path, "root_alias"));
        rename(path_maker(root_path, "root_alias2"),
               path_maker(root_path, "root_alias"));
        //
        char *each_ripo = malloc(10000);
        FILE *ripo_list_file = fopen(path_maker(root_path, "ripo_list"), "r");
        while (fgets(each_ripo, 10000, ripo_list_file) != NULL) {
            if (each_ripo[strlen(each_ripo) - 1] == '\n') {
                each_ripo[strlen(each_ripo) - 1] = '\0';
            }
            strcat(each_ripo, "/.kiwit/");
            file = fopen(path_maker(each_ripo, "alias"), "r");
            file2 = fopen(path_maker(each_ripo, "alias2"), "w");
            while (fgets(line, 1000, file) != NULL) {
                if (line[strlen(line) - 1] == '\n') {
                    line[strlen(line) - 1] = '\0';
                }
                fprintf(file2, "%s", line);
                if (strstr(line, argv[5]) != NULL) {
                    fprintf(file2, " %s", argv[4]);
                }
                fprintf(file2, "\n");
            }
            fclose(file);
            fclose(file2);
            remove(path_maker(each_ripo, "alias"));
            rename(path_maker(each_ripo, "alias2"),
                   path_maker(each_ripo, "alias"));
        }
        fclose(ripo_list_file);
        printf(_SGR_GREENF"The alias has been added successfully globally.\n"_SGR_RESET);
    } else {
        if (argv[3] == NULL || argv[4] == NULL) {
            printf(_SGR_REDB "invalid command\n"_SGR_RESET);
            return;
        } else {
            file = fopen(".kiwit/alias", "r");
            if (file == NULL) {
                printf("Error opening file!\n");
                exit(1);
            }
            file2 = fopen(".kiwit/alias2", "w");
        }
        char *line = malloc(1000);
        while (fgets(line, 1000, file) != NULL) {
            if (line[strlen(line) - 1] == '\n') {
                line[strlen(line) - 1] = '\0';
            }
            fprintf(file2, "%s", line);
            if (strstr(line, argv[4]) != NULL) {
                fprintf(file2, " %s", argv[3]);
            }
            fprintf(file2, "\n");
        }
        fclose(file);
        fclose(file2);
        remove(".kiwit/alias");
        rename(".kiwit/alias2", ".kiwit/alias");
        printf(_SGR_GREENF"The alias has been added successfully in this project.\n"_SGR_RESET);
    }
}

void config_root(int argc, const char *argv[]) {
    int line_number = 0;
    bool is_email = false;
    FILE *file;
    FILE *file2;
    if (strcmp(argv[2], "-global") == 0 &&
        (strcmp(argv[3], "user.name") == 0 || strcmp(argv[3], "user.email") == 0)) {
        if (argv[4] == NULL) {
            printf(_SGR_REDB "invalid command\n"_SGR_RESET);
            return;
        }
        //printf("%s\n", path_maker(root_path, "root_config"));
        file = fopen(path_maker(root_path, "root_config"), "r");
        if (file == NULL) {
            printf("Error opening file!\n");
            exit(1);
        }
        file2 = fopen(path_maker(root_path, "root_config2"), "w");

        int flag = 0;
        char *root_config_line = malloc(1000);
        if (strcmp(argv[3], "user.email") == 0) {
            is_email = 1;
        }
        while (fgets(root_config_line, 1000, file) != NULL) {
            if (root_config_line[strlen(root_config_line) - 1] == '\n') {
                root_config_line[strlen(root_config_line) - 1] = '\0';
            }
            if (line_number == is_email) {
                fprintf(file2, "%s\n", argv[4]);
                flag = 1;
            } else {
                fprintf(file2, "%s\n", root_config_line);
            }
            line_number++;
        }
        if (flag == 0) {
            fprintf(file2, "%s\n", argv[4]);
        }
        fclose(file);
        fclose(file2);
        remove(path_maker(root_path, "root_config"));
        rename(path_maker(root_path, "root_config2"),
               path_maker(root_path, "root_config"));
        char *each_ripo = malloc(10000);
        FILE *ripo_list_file = fopen(path_maker(root_path, "ripo_list"), "r");
        while (fgets(each_ripo, 10000, ripo_list_file) != NULL) {
            if (each_ripo[strlen(each_ripo) - 1] == '\n') {
                each_ripo[strlen(each_ripo) - 1] = '\0';
            }
            line_number = 0;
            is_email = false;
            strcat(each_ripo, "/.kiwit/");
            file = fopen(path_maker(each_ripo, "config"), "r");
            if (file == NULL) {
                //printf("Error opening file!\n");
                exit(1);
            }
            file2 = fopen(path_maker(each_ripo, "config2"), "w");

            flag = 0;
            char *config_line = malloc(1000);
            if (strcmp(argv[3], "user.email") == 0) {
                is_email = 1;
            }
            while (fgets(config_line, 1000, file) != NULL) {
                if (config_line[strlen(config_line) - 1] == '\n') {
                    config_line[strlen(config_line) - 1] = '\0';
                }
                if (line_number == is_email) {
                    fprintf(file2, "%s\n", argv[4]);
                    flag = 1;
                } else {
                    fprintf(file2, "%s\n", config_line);
                }
                line_number++;
            }
            if (flag == 0) {
                fprintf(file2, "%s\n", argv[4]);
            }
            fclose(file);
            fclose(file2);
            remove(path_maker(each_ripo, "config"));
            rename(path_maker(each_ripo, "config2"),
                   path_maker(each_ripo, "config"));
        }
        fclose(ripo_list_file);
        printf(_SGR_GREENF"The config has been added successfully globally.\n"_SGR_RESET);
    } else {
        if (argv[3] == NULL) {
            printf(_SGR_REDF _SGR_BOLD "invalid command\n"_SGR_RESET);
            return;
        } else {
            file = fopen(".kiwit/config", "r");
            if (file == NULL) {
                printf("Error opening file!\n");
                exit(1);
            }
            file2 = fopen(".kiwit/config2", "w");
        }
        int flag = 0;
        char *root_config_line = malloc(1000);
        if (strcmp(argv[2], "user.email") == 0) {
            is_email = 1;
        }
        while (fgets(root_config_line, 1000, file) != NULL) {
            if (root_config_line[strlen(root_config_line) - 1] == '\n') {
                root_config_line[strlen(root_config_line) - 1] = '\0';
            }
            if (line_number == is_email) {
                fprintf(file2, "%s\n", argv[3]);
                flag = 1;
            } else {
                fprintf(file2, "%s\n", root_config_line);
            }
            line_number++;
        }
        if (flag == 0) {
            fprintf(file2, "%s\n", argv[3]);
        }
        fclose(file);
        fclose(file2);
        remove(".kiwit/config");
        rename(".kiwit/config2", ".kiwit/config");
        printf(_SGR_GREENF"The config has been added successfully.\n"_SGR_RESET);
    }
}


int run_add_file(FILE *output_file) {
    char *line = malloc(2000);
    while (fgets(line, 2000, output_file) != NULL) {
        if (line[strlen(line) - 1] == '\n') {
            line[strlen(line) - 1] = '\0';
        }
        //printf("%s\n", line);
        // write a strtok() func on the line
        char *file_path = malloc(2000);
        strcpy(file_path, line);
        char *file_name = malloc(2000);
        strcpy(file_name, line);
        int i = strlen(file_name), j = 0;
        while (file_name[i] != '/') {
            j++;
            i--;
        }
        memmove(file_name, file_path + i + 1, j * sizeof(char));
        //printf("%s\n", file_name);
        char copied_file_address[1000];
        strcpy(copied_file_address, ".kiwit/staging_files/");
        strcat(copied_file_address, file_name);
        FILE *file1;
        FILE *file2;
        if (find_file_in_stage(file_name) == 1) {
            // if: file the same: file_content_checker = 1 // else: file_content_checker = 0
            file1 = fopen(path_maker(find_source(), copied_file_address), "r");
            file2 = fopen(file_path, "r");
            int is_same_content = file_content_checker(file1, file2);
            fclose(file1);
            fclose(file2);
            if (is_same_content) {
                printf(_SGR_REDF "%s is already added.\n", file_name);
                printf(_SGR_RESET);
                continue;
            } else {
                copy_file(file_path, path_maker(find_source(), copied_file_address));
                printf(_SGR_GREENF "%s successfully added.\n", file_name);
                printf(_SGR_RESET);
            }
        } else {
            copy_file(file_path, path_maker(find_source(), copied_file_address));
            FILE *staging_file = fopen(path_maker(find_source(), ".kiwit/staging"), "a");
            FILE *staging_file_2 = fopen(path_maker(find_source(), ".kiwit/staging_2"), "a");
            fprintf(staging_file_2, "%s\n", file_path);
            fprintf(staging_file, "%s\n", path_maker(find_source(), copied_file_address));
            fclose(staging_file);
            fclose(staging_file_2);
            printf(_SGR_GREENF "%s successfully added.\n", file_name);
            printf(_SGR_RESET);
        }
    }
}

int run_add_dir(int argc, char *const argv[], char *file_path, int i) {
    FILE *outputFile = fopen("output.txt", "w");
    explore_directory(file_path, outputFile);
    fclose(outputFile);
    outputFile = fopen("output.txt", "r");
    run_add_file(outputFile);
    fclose(outputFile);
    system("rm output.txt");
}

int run_add(int argc, char *const argv[]) {
    // I have to use + is_f in the argv[num]
    int is_f = 0;
    if ((strcmp(argv[2], "-f") == 0)) {
        if (argc < 4) {
            printf(_SGR_REDF "Please write your file name\n" _SGR_RESET);
            return 1;
        }
        is_f = 1;
    }
    if (strcmp(argv[2], "-n") == 0 && argc > 3) {
        if (atoi(argv[3]) == 0) {
            printf(_SGR_REDF"Please enter a number bigger than 0.\n"_SGR_RESET);
            return 0;
        }
        add_n(find_source(), atoi(argv[3]));
        return 0;
    } else if (strcmp(argv[2], "-n") == 0 && argc <= 3) {
        printf(_SGR_REDF"Please enter the depth.\n"_SGR_RESET);
        return 0;
    }
    for (int i = 2 + is_f; i < argc; ++i) {
        //printf("%s\n", argv[i]);
        char *file_path = find_source();
        strcat(file_path, argv[i]);
        if (is_dir(file_path) == -1) {
            printf(_SGR_REDF "There is no file or directory with this name: " _SGR_RESET);
            printf(_SGR_REDF "%s\n", argv[i]);
            printf(_SGR_RESET);
            //return 1;
        } else if (is_dir(file_path) == 0) {
            char copied_file_address[1000];
            strcpy(copied_file_address, ".kiwit/staging_files/");
            strcat(copied_file_address, argv[i]);
            FILE *file1;
            FILE *file2;
            if (find_file_in_stage(argv[i]) == 1) {
                // if: file the same: file_content_checker = 1 // else: file_content_checker = 0
                file1 = fopen(path_maker(find_source(), copied_file_address), "r");
                file2 = fopen(file_path, "r");
                int is_same_content = file_content_checker(file1, file2);
                fclose(file1);
                fclose(file2);
                if (is_same_content) {
                    printf(_SGR_REDF "%s is already added.\n", argv[i]);
                    printf(_SGR_RESET);
                    continue;
                } else {
                    copy_file(file_path, path_maker(find_source(), copied_file_address));
                    printf(_SGR_GREENF "%s successfully added.\n", argv[i]);
                    printf(_SGR_RESET);
                }
            } else {
                copy_file(file_path, path_maker(find_source(), copied_file_address));
                FILE *staging_file = fopen(path_maker(find_source(), ".kiwit/staging"), "a");
                FILE *staging_file_2 = fopen(path_maker(find_source(), ".kiwit/staging_2"), "a");
                fprintf(staging_file_2, "%s\n", file_path);
                fprintf(staging_file, "%s\n", path_maker(find_source(), copied_file_address));
                fclose(staging_file);
                fclose(staging_file_2);
                printf(_SGR_GREENF "%s successfully added.\n", argv[i]);
                printf(_SGR_RESET);
            }
        } else if (is_dir(file_path) == 1) {
            run_add_dir(argc, argv, file_path, i);
        }
    }
}

int run_add_redo(int argc, char *const argv[]) {
    FILE *staging_address1 = fopen(path_maker(find_source(), ".kiwit/staging"), "r");
    FILE *staging_address2 = fopen(path_maker(find_source(), ".kiwit/staging_2"), "r");
    char *line1 = malloc(1000);
    char *line2 = malloc(1000);
    int flag = 0;
    while (fgets(line1, 1000, staging_address1) != NULL) {
        if (line1[strlen(line1) - 1] == '\n') {
            line1[strlen(line1) - 1] = '\0';
        }
        fgets(line2, 1000, staging_address2);
        if (line2[strlen(line2) - 1] == '\n') {
            line2[strlen(line2) - 1] = '\0';
        }
        FILE *file1 = fopen(line1, "r");
        FILE *file2 = fopen(line2, "r");
        int is_same_content = file_content_checker(file1, file2);
        fclose(file1);
        fclose(file2);
        if (!is_same_content) {
            copy_file(line2, line1);
            char *file_name = malloc(2000);
            strcpy(file_name, line1);
            int i = strlen(file_name), j = 0;
            while (file_name[i] != '/') {
                j++;
                i--;
            }
            memmove(file_name, line1 + i + 1, j * sizeof(char));
            printf(_SGR_GREENF "%s is successfully added.\n", file_name);
            printf(_SGR_RESET);
            flag = 1;
        }
    }
    if (flag == 0) {
        printf(_C_L_PURPLE "Everything is already added.\n"_SGR_RESET);
    }
}

int run_reset_file() {
    int line_counter = 0;
    char *file_path = find_source();
    FILE *tmp = fopen("output.txt", "r");
    FILE *final_output = fopen("output2.txt", "w");
    FILE *staged_files_address2 = fopen(path_maker(find_source(), ".kiwit/staging_2"), "r");
    char *line = malloc(1025);
    char *line2 = malloc(1025);
    int flag = 0;
    while (fgets(line, 1000, tmp) != NULL) {
        flag = 0;
        if (line[strlen(line) - 1] == '\n') {
            line[strlen(line) - 1] = '\0';
        }
        while (fgets(line2, 1000, staged_files_address2) != NULL) {
            if (line2[strlen(line2) - 1] == '\n') {
                line2[strlen(line2) - 1] = '\0';
            }
            //printf("line 2 : %s\n", line2);
            //printf("line : %s\n", line);
            if (strcmp(line, line2) == 0) {
                flag = 1;
                line_counter++;
                break;
            }
        }
        line_counter++;
        if (flag == 0) {
            //printf("flag : %d\n", flag);
            char *file_name = malloc(2000);
            strcpy(file_name, line);
            int i = strlen(file_name), j = 0;
            while (file_name[i] != '/') {
                j++;
                i--;
            }
            memmove(file_name, line + i + 1, j * sizeof(char));
            printf(_SGR_REDF "%s is not staged before.\n", file_name);
            printf(_SGR_RESET);
            continue;
        } else {
            //printf("flag : %d\n", flag);
            fprintf(final_output, "%s\n", line);
            //printf("line : %s\n", line);
        }
    }
    fclose(tmp);
    fclose(staged_files_address2);
    fclose(final_output);


    FILE *staged_files_address1 = fopen(path_maker(find_source(), ".kiwit/staging"), "r");
    staged_files_address2 = fopen(path_maker(find_source(), ".kiwit/staging_2"), "r");
    final_output = fopen("output2.txt", "r");
    char *main_line = malloc(1025);
    while (fgets(main_line, 1024, final_output) != NULL) {
        if (main_line[strlen(main_line) - 1] == '\n') {
            main_line[strlen(main_line) - 1] = '\0';
        }
        printf("main Line:  %s\n", main_line);
        char *file_name = malloc(2000);
        strcpy(file_name, main_line);
        int i = strlen(file_name), j = 0;
        while (file_name[i] != '/') {
            j++;
            i--;
        }
        memmove(file_name, main_line + i + 1, j * sizeof(char));
        //printf("%s\n", line);
        char copied_file_address[2000];
        strcpy(copied_file_address, ".kiwit/staging_files/");
        strcat(copied_file_address, file_name);
        strcpy(copied_file_address, path_maker(find_source(), copied_file_address));
        //printf("%s\n", copied_file_address);
        char move_file_address[2000];
        strcpy(move_file_address, ".kiwit/unstaging_files/");
        strcpy(move_file_address, path_maker(find_source(), move_file_address));
        char *move_command = malloc(4000);
        strcpy(move_command, "mv ");
        strcat(move_command, copied_file_address);
        strcat(move_command, " ");
        strcat(move_command, move_file_address);
        system(move_command);

        FILE *unstaging_file = fopen(path_maker(find_source(), ".kiwit/unstaging"), "a");
        strcat(move_file_address, file_name);
        fprintf(unstaging_file, "%s\n", move_file_address);
        fclose(unstaging_file);

        FILE *unstaging_file_2 = fopen(path_maker(find_source(), ".kiwit/unstaging_2"), "a");
        fprintf(unstaging_file_2, "%s\n", main_line);
        fclose(unstaging_file_2);

        char *file_address_tmp = malloc(2000);
        strcpy(file_address_tmp, main_line);
        FILE *tmp_output = fopen(path_maker(find_source(), ".kiwit/tmp_output"), "w");
        int flag_file_address_found = 0;
        while (fgets(line, 1024, staged_files_address2) != NULL) {
            if (line[strlen(line) - 1] == '\n') {
                line[strlen(line) - 1] = '\0';
            }
            if (strcmp(line, file_address_tmp) == 0) {
                flag_file_address_found = 1;
                break;
            }
        }
        if (flag_file_address_found == 0) {
            fprintf(tmp_output, "%s\n", line);
        }
        fclose(tmp_output);
        fclose(staged_files_address2);
        remove(path_maker(find_source(), ".kiwit/staging_2"));
        rename(path_maker(find_source(), ".kiwit/tmp_output"), path_maker(find_source(), ".kiwit/staging_2"));
        tmp_output = fopen(path_maker(find_source(), ".kiwit/tmp_output"), "w");
        while (fgets(line, 1024, staged_files_address1) != NULL) {
            if (line[strlen(line) - 1] == '\n') {
                line[strlen(line) - 1] = '\0';
            }
            if (strcmp(line, copied_file_address) == 0) {
                flag_file_address_found = 1;
                break;
            }
        }
        if (flag_file_address_found == 0) {
            fprintf(tmp_output, "%s\n", line);
        }
        fclose(tmp_output);
        fclose(staged_files_address1);
        remove(path_maker(find_source(), ".kiwit/staging"));
        rename(path_maker(find_source(), ".kiwit/tmp_output"), path_maker(find_source(), ".kiwit/staging"));
        printf(_SGR_GREENF "%s successfully unstaged.\n", file_name);
        printf(_SGR_RESET);
    }
    remove("output2.txt");
}

int run_reset_dir(int argc, char *const argv[], char *file_path, int i) {
    FILE *outputFile = fopen("output.txt", "w");
    explore_directory(file_path, outputFile);
    fclose(outputFile);
    run_reset_file();
    system("rm output.txt");
}

int run_reset(int argc, char *const argv[]) {
    int is_f = 0;
    if ((strcmp(argv[2], "-f") == 0)) {
        if (argc < 4) {
            printf(_SGR_REDF "Please write your file name\n" _SGR_RESET);
            return 1;
        }
        is_f = 1;
    }
    for (int i = 2 + is_f; i < argc; ++i) {
        //printf("%s\n", argv[i]);
        int line_counter = 0;
        char *file_path = find_source();
        strcat(file_path, argv[i]);
        if (is_dir(file_path) == -1) {
            printf(_SGR_REDF "There is no file or directory with this name: " _SGR_RESET);
            printf(_SGR_REDF "%s\n", argv[i]);
            printf(_SGR_RESET);
            //return 1;
        } else if (is_dir(file_path) == 0) {
            FILE *staged_files_address2 = fopen(path_maker(find_source(), ".kiwit/staging_2"), "r");
            char *line = malloc(1025);
            int flag = 0;
            while (fgets(line, 1000, staged_files_address2) != NULL) {
                if (line[strlen(line) - 1] == '\n') {
                    line[strlen(line) - 1] = '\0';
                }
                if (strcmp(line, file_path) == 0) {
                    flag = 1;
                    line_counter++;
                    break;
                }
                line_counter++;
            }
            if (flag == 0) {
                printf(_SGR_REDF "%s is not staged before.\n", argv[i]);
                continue;
            }
            fclose(staged_files_address2);
            FILE *staged_files_address1 = fopen(path_maker(find_source(), ".kiwit/staging"), "r");
            staged_files_address2 = fopen(path_maker(find_source(), ".kiwit/staging_2"), "r");
            //printf("%s\n", line);
            char copied_file_address[2000];
            strcpy(copied_file_address, ".kiwit/staging_files/");
            strcat(copied_file_address, argv[i]);
            strcpy(copied_file_address, path_maker(find_source(), copied_file_address));
            //printf("%s\n", copied_file_address);
            char move_file_address[2000];
            strcpy(move_file_address, ".kiwit/unstaging_files/");
            strcpy(move_file_address, path_maker(find_source(), move_file_address));
            char *move_command = malloc(4000);
            strcpy(move_command, "mv ");
            strcat(move_command, copied_file_address);
            strcat(move_command, " ");
            strcat(move_command, move_file_address);
            system(move_command);

            FILE *unstaging_file = fopen(path_maker(find_source(), ".kiwit/unstaging"), "a");
            strcat(move_file_address, argv[i]);
            fprintf(unstaging_file, "%s\n", move_file_address);
            fclose(unstaging_file);

            FILE *unstaging_file_2 = fopen(path_maker(find_source(), ".kiwit/unstaging_2"), "a");
            fprintf(unstaging_file_2, "%s\n", line);
            fclose(unstaging_file_2);
            char *file_address_tmp = malloc(2000);
            strcpy(file_address_tmp, line);
            FILE *tmp_output = fopen(path_maker(find_source(), ".kiwit/tmp_output"), "w");
            while (fgets(line, 1024, staged_files_address2) != NULL) {
                if (line[strlen(line) - 1] == '\n') {
                    line[strlen(line) - 1] = '\0';
                }
                if (strcmp(line, file_address_tmp) != 0) {
                    fprintf(tmp_output, "%s\n", line);
                }
            }
            fclose(tmp_output);
            fclose(staged_files_address2);
            remove(path_maker(find_source(), ".kiwit/staging_2"));
            rename(path_maker(find_source(), ".kiwit/tmp_output"),
                   path_maker(find_source(), ".kiwit/staging_2"));
            tmp_output = fopen(path_maker(find_source(), ".kiwit/tmp_output"), "w");
            while (fgets(line, 1024, staged_files_address1) != NULL) {
                if (line[strlen(line) - 1] == '\n') {
                    line[strlen(line) - 1] = '\0';
                }
                if (strcmp(line, copied_file_address) != 0) {
                    fprintf(tmp_output, "%s\n", line);
                }
            }
            fclose(tmp_output);
            fclose(staged_files_address1);
            remove(path_maker(find_source(), ".kiwit/staging"));
            rename(path_maker(find_source(), ".kiwit/tmp_output"), path_maker(find_source(), ".kiwit/staging"));
            printf(_SGR_GREENF "%s successfully unstaged.\n", argv[i]);
            printf(_SGR_RESET);
        } else if (is_dir(file_path) == 1) {
            run_reset_dir(argc, argv, file_path, i);
        }
    }
}

int run_reset_undo(int argc, char *const argv[]) {
    FILE *unstaged = fopen(path_maker(find_source(), ".kiwit/unstaging"), "r");
    FILE *unstaged_2 = fopen(path_maker(find_source(), ".kiwit/unstaging_2"), "r");
    FILE *staged = fopen(path_maker(find_source(), ".kiwit/staging"), "a");
    FILE *staged_2 = fopen(path_maker(find_source(), ".kiwit/staging_2"), "a");
    char line[1025];
    while (fgets(line, 1024, unstaged) != NULL) {
        if (line[strlen(line) - 1] == '\n') {
            line[strlen(line) - 1] = '\0';
        }
        char *file_name = malloc(2000);
        strcpy(file_name, line);
        int i = strlen(file_name), j = 0;
        while (file_name[i] != '/') {
            j++;
            i--;
        }
        memmove(file_name, line + i + 1, j * sizeof(char));
        char copied_file_address[2000];
        strcpy(copied_file_address, ".kiwit/staging_files/");
        strcat(copied_file_address, file_name);
        strcpy(copied_file_address, path_maker(find_source(), copied_file_address));
        fprintf(staged, "%s\n", copied_file_address);
        char *move_command = malloc(4000);
        strcpy(move_command, "mv ");
        strcat(move_command, line);
        strcat(move_command, " ");
        strcat(move_command, copied_file_address);
        system(move_command);
        fgets(line, 1024, unstaged_2);
        if (line[strlen(line) - 1] == '\n') {
            line[strlen(line) - 1] = '\0';
        }
        fprintf(staged_2, "%s\n", line);
    }
    fclose(unstaged);
    fclose(unstaged_2);
    fclose(staged);
    fclose(staged_2);
    unstaged = fopen(path_maker(find_source(), ".kiwit/unstaging"), "w");
    unstaged_2 = fopen(path_maker(find_source(), ".kiwit/unstaging_2"), "w");
    fclose(unstaged);
    fclose(unstaged_2);
    printf(_SGR_GREENF "Undo is successfully done.\n"_SGR_RESET);
}

int run_commit(int argc, char *const argv[]) {
    if (argc < 4) {
        printf(_SGR_REDF "Please write your commit message\n"_SGR_RESET);
        return 1;
    }
    if (strlen(argv[3]) > 72) {
        printf(_SGR_REDF "The commit message should have at most 72 characters\n"_SGR_RESET);
        return 1;
    }

    char *commit_message = malloc(1000);
    if (strcmp(argv[2], "-m") == 0) {
        strcpy(commit_message, argv[3]);
    } else if (strcmp(argv[2], "-s") == 0) {
        FILE *commit_message_shortcuts = fopen(path_maker(find_source(), ".kiwit/commit_message_shortcuts"), "r");
        char *line = malloc(1000);
        while (fgets(line, 1000, commit_message_shortcuts) != NULL) {
            if (line[strlen(line) - 1] == '\n') {
                line[strlen(line) - 1] = '\0';
            }
            if (strstr(line, argv[3]) != NULL) {
                strcpy(commit_message, line);
                break;
            }
        }
        fclose(commit_message_shortcuts);
    }
    char *final_commit_message = malloc(1000);
    for (int k = 0; k < strlen(commit_message); ++k) {
        if (commit_message[k] == '&') {
            break;
        }
        final_commit_message[k] = commit_message[k];
    }
    final_commit_message[strlen(final_commit_message)] = '\0';

    if (strlen(final_commit_message) == 0) {
        printf(_SGR_REDF "There is no commit message with this shortcut.\n"_SGR_RESET);
        return 1;
    }

    FILE *staged_files_address = fopen(path_maker(find_source(), ".kiwit/staging"), "r");
    char *line = malloc(1025);
    int flag = 0;
    int file_counter = 0;
    while (fgets(line, 1024, staged_files_address) != NULL) {
        if (line[strlen(line) - 1] == '\n') {
            line[strlen(line) - 1] = '\0';
        }
        flag = 1;
        break;
    }
    if (flag == 0) {
        printf(_SGR_REDF "There is no file to commit\n"_SGR_RESET);
        return 1;
    }
    fclose(staged_files_address);
    staged_files_address = fopen(path_maker(find_source(), ".kiwit/staging"), "r");
    while (fgets(line, 1024, staged_files_address) != NULL) {
        if (line[strlen(line) - 1] == '\n') {
            line[strlen(line) - 1] = '\0';
        }
        file_counter++;
    }
    fclose(staged_files_address);
    FILE *current_branch = fopen(path_maker(find_source(), ".kiwit/current_branch"), "r");
    char *branch_address = malloc(1025);
    char *branch_name = malloc(1025);
    fgets(branch_address, 1024, current_branch);
    if (branch_address[strlen(branch_address) - 1] == '\n') {
        branch_address[strlen(branch_address) - 1] = '\0';
    }
    fclose(current_branch);
    int i = strlen(branch_address), j = 0;
    while (branch_address[i] != '/') {
        j++;
        i--;
    }
    memmove(branch_name, branch_address + i + 1, j * sizeof(char));
    FILE *commit_ID_file = fopen(path_maker(find_source(), ".kiwit/commit_ID"), "r");
    char *commit_ID = malloc(1025);
    fgets(commit_ID, 1024, commit_ID_file);
    if (commit_ID[strlen(commit_ID) - 1] == '\n') {
        commit_ID[strlen(commit_ID) - 1] = '\0';
    }
    fclose(commit_ID_file);
    int commit_ID_int = atoi(commit_ID);
    commit_ID_int++;
    fopen(path_maker(find_source(), ".kiwit/commit_ID"), "w");
    fprintf(commit_ID_file, "%d\n", commit_ID_int);
    fclose(commit_ID_file);
    char *commit_address = malloc(1025);
    strcpy(commit_address, branch_address);
    strcat(commit_address, "/");
    strcat(commit_address, commit_ID);
    if (mkdir(commit_address, 0755) != 0)
        return 1;
    char *commit_data_folder = malloc(1025);
    strcpy(commit_data_folder, commit_address);
    strcat(commit_data_folder, "/data");
    if (mkdir(commit_data_folder, 0755) != 0)
        return 1;
    strcat(commit_data_folder, "/");
    char *commit_address_slash = malloc(1025);
    strcpy(commit_address_slash, commit_address);
    strcat(commit_address_slash, "/");
    FILE *stage_file = fopen(path_maker(find_source(), ".kiwit/staging"), "r");
    while (fgets(line, 1024, stage_file)) {
        if (line[strlen(line) - 1] == '\n') {
            line[strlen(line) - 1] = '\0';
        }
        char *file_name = malloc(2000);
        strcpy(file_name, line);
        int i = strlen(file_name), j = 0;
        while (file_name[i] != '/') {
            j++;
            i--;
        }
        memmove(file_name, line + i + 1, j * sizeof(char));
        copy_file(line, path_maker(commit_address_slash, file_name));
        remove(line);
    }
    fclose(stage_file);
    stage_file = fopen(path_maker(find_source(), ".kiwit/staging"), "r");
    FILE *commit_file = fopen(path_maker(commit_data_folder, "staging"), "w");
    while (fgets(line, 1024, stage_file)) {
        if (line[strlen(line) - 1] == '\n') {
            line[strlen(line) - 1] = '\0';
        }
        char *file_name = malloc(2000);
        strcpy(file_name, line);
        int i = strlen(file_name), j = 0;
        while (file_name[i] != '/') {
            j++;
            i--;
        }
        memmove(file_name, line + i + 1, j * sizeof(char));
        fprintf(commit_file, "%s\n", path_maker(commit_address_slash, file_name));
    }
    fclose(stage_file);
    fclose(commit_file);
    char *staging_2_address = malloc(1025);
    strcpy(staging_2_address, find_source());
    strcat(staging_2_address, ".kiwit/staging_2");
    char *command = malloc(4000);
    strcpy(command, "cp ");
    strcat(command, staging_2_address);
    strcat(command, " ");
    strcat(command, commit_data_folder);
    system(command);
    stage_file = fopen(path_maker(find_source(), ".kiwit/staging"), "w");
    fclose(stage_file);
    stage_file = fopen(path_maker(find_source(), ".kiwit/staging_2"), "w");
    fclose(stage_file);

    // commit log
    // first line : the date of the commit in this format: "YYYY-MM-DD HH:MM:SS"
    // second line : the commit message
    // third line : the username of the committer
    // fourth line : the commit ID
    // fifth line : the branch name
    // sixth line : count of the files in the commit
    FILE *commit_log = fopen(path_maker(commit_data_folder, "commit_log"), "w");
    FILE *all_logs = fopen(path_maker(find_source(), ".kiwit/commits/all_logs"), "r");
    FILE *all_logs_2 = fopen(path_maker(find_source(), ".kiwit/commits/all_logs_2"), "w");

    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    fprintf(commit_log, "%d-%02d-%02d %02d:%02d:%02d\n", tm.tm_year + 1900, tm.tm_mon + 1,
            tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
    fprintf(all_logs_2, "The commit time is: %d-%02d-%02d %02d:%02d:%02d\n", tm.tm_year + 1900, tm.tm_mon + 1,
            tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
    fprintf(commit_log, "%s\n", argv[3]);
    fprintf(all_logs_2, "The commit message is: %s\n", argv[3]);
    FILE *config_file = fopen(path_maker(find_source(), ".kiwit/config"), "r");
    char *committer_username = malloc(1025);
    fgets(committer_username, 1024, config_file);
    if (committer_username[strlen(committer_username) - 1] == '\n') {
        committer_username[strlen(committer_username) - 1] = '\0';
    }
    fclose(config_file);
    fprintf(commit_log, "%s\n", committer_username);
    fprintf(all_logs_2, "The committer user.name is: %s\n", committer_username);
    fprintf(commit_log, "%s\n", commit_ID);
    fprintf(all_logs_2, "The commit ID is: %s\n", commit_ID);
    fprintf(commit_log, "%s\n", branch_name);
    fprintf(all_logs_2, "The branch name is: %s\n", branch_name);
    fprintf(commit_log, "%d\n", file_counter);
    fprintf(all_logs_2, "The files count of this commit is: %d\n", file_counter);
    fclose(commit_log);
    fprintf(all_logs_2, "-\n");
    char *line2 = malloc(1025);
    while (fgets(line, 1024, all_logs) != NULL) {
        if (line[strlen(line) - 1] == '\n') {
            line[strlen(line) - 1] = '\0';
        }
        fprintf(all_logs_2, "%s\n", line);
    }
    fclose(all_logs);
    fclose(all_logs_2);
    remove(path_maker(find_source(), ".kiwit/commits/all_logs"));
    rename(path_maker(find_source(), ".kiwit/commits/all_logs_2"),
           path_maker(find_source(), ".kiwit/commits/all_logs"));
    //

    // commit printng
    printf(_SGR_GREENF "The commit has been done successfully.\n"_SGR_RESET);
    printf("The commit ID is: %s\n", commit_ID);
    printf("The commit time is: %d-%02d-%02d %02d:%02d:%02d\n", tm.tm_year + 1900, tm.tm_mon + 1,
           tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
    printf("The commit message is: %s\n", argv[3]);
}

int run_set(int argc, char *const argv[]) {
    //shortcut message format is message&shortcut

    if (strcmp(argv[2], "-m") != 0 || strcmp(argv[4], "-s") != 0) {
        printf(_SGR_REDF "invalid command\n"_SGR_RESET);
        return 1;
    }
    if (argc < 6) {
        printf(_SGR_REDF "Please write your shortcut.\n"_SGR_RESET);
        return 1;
    }
    if (strlen(argv[3]) > 72) {
        printf(_SGR_REDF "The shortcut should have at most 72 characters\n"_SGR_RESET);
        return 1;
    }
    FILE *shortcut_file = fopen(path_maker(find_source(), ".kiwit/commit_message_shortcuts"), "r");
    if (shortcut_file == NULL) {
        printf("Error opening file!\n");
        exit(1);
    }
    char *line = malloc(1000);
    while (fgets(line, 1000, shortcut_file) != NULL) {
        if (line[strlen(line) - 1] == '\n') {
            line[strlen(line) - 1] = '\0';
        }
        if (strstr(line, argv[3]) != NULL) {
            printf(_SGR_REDF "This shortcut is already exist.\n"_SGR_RESET);
            fclose(shortcut_file);
            break;
        }
    }
    fclose(shortcut_file);
    shortcut_file = fopen(path_maker(find_source(), ".kiwit/commit_message_shortcuts"), "a");
    fprintf(shortcut_file, "%s&%s\n", argv[3], argv[5]);
    fclose(shortcut_file);
    printf(_SGR_GREENF"The shortcut message has been added successfully in this project.\n"_SGR_RESET);
}

int run_replace(int argc, char *const argv[]) {
    if (strcmp(argv[2], "-m") != 0 || strcmp(argv[4], "-s") != 0) {
        printf(_SGR_REDF "invalid command\n"_SGR_RESET);
        return 1;
    }
    if (argc < 6) {
        printf(_SGR_REDF "Please write your shortcut.\n"_SGR_RESET);
        return 1;
    }
    if (strlen(argv[3]) > 72) {
        printf(_SGR_REDF "The shortcut should have at most 72 characters\n"_SGR_RESET);
        return 1;
    }
    FILE *shortcut_file = fopen(path_maker(find_source(), ".kiwit/commit_message_shortcuts"), "r");
    FILE *shortcut_file_2 = fopen(path_maker(find_source(), ".kiwit/commit_message_shortcuts_2"), "w");
    char *line = malloc(1000);
    int flag = 0;
    while (fgets(line, 1000, shortcut_file) != NULL) {
        if (strstr(line, argv[5]) != NULL) {
            fprintf(shortcut_file_2, "%s&%s\n", argv[3], argv[5]);
            flag = 1;
        } else {
            fprintf(shortcut_file_2, "%s\n", line);
        }
    }
    fclose(shortcut_file);
    fclose(shortcut_file_2);
    if (flag == 0) {
        printf(_SGR_REDF "This shortcut is not exist.\n"_SGR_RESET);
        return 1;
    }
    remove(path_maker(find_source(), ".kiwit/commit_message_shortcuts"));
    rename(path_maker(find_source(), ".kiwit/commit_message_shortcuts_2"),
           path_maker(find_source(), ".kiwit/commit_message_shortcuts"));
    printf(_SGR_GREENF"The shortcut message has been replaced successfully in this project.\n"_SGR_RESET);
}

int run_remove(int argc, char *const argv[]) {
    if (strcmp(argv[2], "-s") != 0) {
        printf(_SGR_REDF "invalid command\n"_SGR_RESET);
        return 1;
    }
    FILE *shortcut_file = fopen(path_maker(find_source(), ".kiwit/commit_message_shortcuts"), "r");
    FILE *shortcut_file_2 = fopen(path_maker(find_source(), ".kiwit/commit_message_shortcuts_2"), "w");
    char *line = malloc(1000);
    int flag = 0;
    while (fgets(line, 1000, shortcut_file) != NULL) {
        if (strstr(line, argv[3]) != NULL) {
            flag = 1;
        } else {
            fprintf(shortcut_file_2, "%s\n", line);
        }
    }
    fclose(shortcut_file);
    fclose(shortcut_file_2);
    if (flag == 0) {
        printf(_SGR_REDF "This shortcut is not exist.\n"_SGR_RESET);
        return 1;
    }
    remove(path_maker(find_source(), ".kiwit/commit_message_shortcuts"));
    rename(path_maker(find_source(), ".kiwit/commit_message_shortcuts_2"),
           path_maker(find_source(), ".kiwit/commit_message_shortcuts"));
    printf(_SGR_GREENF"The shortcut message has been removed successfully in this project.\n"_SGR_RESET);
}

int run_log(int argc, char *const argv[]) {
    FILE *all_logs = fopen(path_maker(find_source(), ".kiwit/commits/all_logs"), "r");
    int counter = 0;
    char *line = malloc(1000);
    while (fgets(line, 1000, all_logs) != NULL) {
        if (line[strlen(line) - 1] == '\n') {
            line[strlen(line) - 1] = '\0';
        }
        switch (counter % 7) {
            case 0:
                printf(_SGR_YELLOWF "%s\n"_SGR_RESET, line);
                break;
            case 1:
                printf(_C_L_ORANGE "%s\n"_SGR_RESET, line);
                break;
            case 2:
                printf(_C_L_PURPLE "%s\n"_SGR_RESET, line);
                break;
            case 3:
                printf(_C_L_GRAY "%s\n"_SGR_RESET, line);
                break;
            case 4:
                printf(_C_L_GREEN"%s\n"_SGR_RESET, line);
                break;
            case 5:
                printf(_C_D_GREEN "%s\n"_SGR_RESET, line);
                break;
            case 6:
                printf(_SGR_RESET"%s\n"_SGR_RESET, line);
                break;
        }
        counter++;
    }
    fclose(all_logs);
}

int run_log_n(int argc, char *const argv[]) {
    if (strcmp(argv[2], "-n") != 0) {
        printf(_SGR_REDF "invalid command\n"_SGR_RESET);
        return 1;
    }
    FILE *all_logs = fopen(path_maker(find_source(), ".kiwit/commits/all_logs"), "r");
    int counter = 0;
    char *line = malloc(1000);
    int n = atoi(argv[3]);
    while (fgets(line, 1000, all_logs) != NULL && counter < n * 7) {
        if (line[strlen(line) - 1] == '\n') {
            line[strlen(line) - 1] = '\0';
        }
        printf("%s\n", line);
        counter++;
    }
    fclose(all_logs);
}

int run_log_branch(int argc, char *const argv[]) {
    if (strcmp(argv[2], "-branch") != 0) {
        printf(_SGR_REDF "invalid command\n"_SGR_RESET);
        return 1;
    }
    // this function should only print the logs that have the same branch name with argv[3]
    // the branch name is written in the line 5 of each commit log in the all_logs file
    // the printing don't need to be colorized
    FILE *all_logs = fopen(path_maker(find_source(), ".kiwit/commits/all_logs"), "r");
    int counter = 0;
    char *line = malloc(1000);
    char **logs = malloc(8 * sizeof(char *));
    for (int i = 0; i < 8; ++i) {
        logs[i] = malloc(1000);
    }
    while (fgets(line, 1000, all_logs) != NULL) {
        if (line[strlen(line) - 1] == '\n') {
            line[strlen(line) - 1] = '\0';
        }
        strcpy(logs[counter % 7], line);
        if (counter % 7 == 6) {
            if (strstr(logs[4], argv[3]) != NULL) {
                for (int i = 0; i < 7; ++i) {
                    printf("%s\n", logs[i]);
                }
            }
        }
        counter++;
    }
    fclose(all_logs);
}

int run_log_author(int argc, char *const argv[]) {
    if (strcmp(argv[2], "-author") != 0) {
        printf(_SGR_REDF "invalid command\n"_SGR_RESET);
        return 1;
    }
    // this function should only print the logs that have the same author name with argv[3]
    // the author name is written in the line 3 of each commit log in the all_logs file
    // the printing don't need to be colorized
    FILE *all_logs = fopen(path_maker(find_source(), ".kiwit/commits/all_logs"), "r");
    int counter = 0;
    char *line = malloc(1000);
    char **logs = malloc(8 * sizeof(char *));
    for (int i = 0; i < 8; ++i) {
        logs[i] = malloc(1000);
    }
    while (fgets(line, 1000, all_logs) != NULL) {
        if (line[strlen(line) - 1] == '\n') {
            line[strlen(line) - 1] = '\0';
        }
        strcpy(logs[counter % 7], line);
        if (counter % 7 == 6) {
            if (strstr(logs[2], argv[3]) != NULL) {
                for (int i = 0; i < 7; ++i) {
                    printf("%s\n", logs[i]);
                }
            }
        }
        counter++;
    }
    fclose(all_logs);
}

time_t string_to_time(char time_string[]) {
    struct tm temp;
    if (strptime(time_string, "%Y-%m-%d %H:%M:%S", &temp) == NULL) {
        return -1;
    }
    return (mktime(&temp));
}

int run_log_since(int argc, char *const argv[]) {
    if (strcmp(argv[2], "-since") != 0) {
        printf(_SGR_REDF "invalid command\n"_SGR_RESET);
        return 1;
    }
    // this function should only print the logs that have the commit time after argv[3]
    // the commit time is written in the line 1 of each commit log in the all_logs file
    // the printing don't need to be colorized
    // this function should have an if to compare the time that its get in the argv[3] with this format: 2024-02-02 16:10 and then compare it from the times that saved in the all_logs file
    FILE *all_logs = fopen(path_maker(find_source(), ".kiwit/commits/all_logs"), "r");
    int counter = 0;
    char *line = malloc(1000);
    char **logs = malloc(8 * sizeof(char *));
    for (int i = 0; i < 8; ++i) {
        logs[i] = malloc(1000);
    }
    time_t since_time = string_to_time(argv[3]);
    if (since_time == -1) {
        printf(_SGR_REDF "invalid time format\n"_SGR_RESET);
        return 1;
    }
    while (fgets(line, 1000, all_logs) != NULL) {
        if (line[strlen(line) - 1] == '\n') {
            line[strlen(line) - 1] = '\0';
        }
        strcpy(logs[counter % 7], line);
        if (counter % 7 == 6) {
            // logs[0] is something like this: The commit time is: 2024-02-02 16:10:59 and it has to change to 2024-02-02 16:10:59 that means the "The commit time is: " has to be removed
            char *time_string = malloc(1000);
            strcpy(time_string, logs[0]);
            memmove(time_string, time_string + 20, strlen(time_string) - 20);
            time_string[19] = '\0';
            time_t commit_time = string_to_time(time_string);
            printf("argv[ 3 ] : ****%s****\n", argv[3]);
            printf("since_time: ****%s****\n", asctime(localtime(&since_time)));
            printf("time_string: ****%s****\n", time_string);
            printf("commit_time: ****%s****\n", asctime(localtime(&commit_time)));
            printf("\n");

//            if (commit_time >= since_time) {
//                for (int i = 0; i < 7; ++i) {
//                    printf("%s\n", logs[i]);
//                }
//            }
        }
        counter++;
    }
    fclose(all_logs);
}

int main(int argc, const char *argv[]) {
    if (argc < 2) {
        printf("Usage : %s command\n", argv[0]);
        return 1;
    }
    char *cwd = malloc(2024);
    getcwd(cwd, sizeof(cwd));
    strcat(cwd, "/.kiwit/alias");
    char line[1000];
    char *command = calloc(20, sizeof(char));
    FILE *file = fopen(path_maker(root_path, "root_alias"), "r");
    while (fgets(line, 1000, file) != NULL) {
        if (strstr(line, argv[1]) != NULL) {
            sscanf(line, "%s", command);
            if (strcmp(command, argv[1]) == 0) {
                break;
            }
        }
    }
    fclose(file);
    if (command[0] == '\0') {
        char *dir = find_source();
        strcat(dir, ".kiwit/alias");
        file = fopen(dir, "r");
        while (fgets(line, 1000, file) != NULL) {
            if (strstr(line, argv[1]) != NULL) {
                sscanf(line, "%s", command);
                if (strcmp(command, argv[1]) == 0) {
                    break;
                }
            }
        }
        fclose(file);
    }
    if (strcmp(command, "init") == 0 && argc == 2) {
        return run_init(argc, argv);
    } else if (strcmp(command, "config") == 0 && argc >= 3) {
        if (strcmp(argv[3], "alias") == 0 || strcmp(argv[2], "alias") == 0) {
            config_alias(argc, argv);
        } else {
            config_root(argc, argv);
        }
    } else if (strcmp(command, "--start") == 0 && argc == 2) {
        logo_print();
    } else if (strcmp(command, "add") == 0 && argc >= 3) {
        if (strcmp(argv[2], "-redo") == 0) {
            return run_add_redo(argc, argv);
        } else {
            return run_add(argc, argv);
        }
    } else if (strcmp(command, "reset") == 0 && argc >= 3) {
        if (strcmp(argv[2], "-undo") == 0) {
            return run_reset_undo(argc, argv);
        } else {
            return run_reset(argc, argv);
        }
    } else if (strcmp(command, "commit") == 0 && argc >= 4 &&
               (strcmp(argv[2], "-m") == 0 || strcmp(argv[2], "-s") == 0)) {
        return run_commit(argc, argv);
    } else if (strcmp(command, "set") == 0 && argc == 6 && strcmp(argv[2], "-m") == 0 &&
               strcmp(argv[4], "-s") == 0) {
        return run_set(argc, argv);
    } else if (strcmp(command, "replace") == 0 && argc == 6 && strcmp(argv[2], "-m") == 0 &&
               strcmp(argv[4], "-s") == 0) {
        return run_replace(argc, argv);
    } else if (strcmp(command, "remove") == 0 && argc == 4 && strcmp(argv[2], "-s") == 0) {
        return run_remove(argc, argv);
    } else if (strcmp(command, "log") == 0) {
        if (argc == 2) {
            return run_log(argc, argv);
        } else if (argc == 4) {
            if (strcmp(argv[2], "-n") == 0) {
                return run_log_n(argc, argv);
            } else if (strcmp(argv[2], "-branch") == 0) {
                return run_log_branch(argc, argv);
            } else if (strcmp(argv[2], "-author") == 0) {
                return run_log_author(argc, argv);
            } else if (strcmp(argv[2], "-since") == 0) {
                return run_log_since(argc, argv);
            }
//            else if (strcmp(argv[2], "-before") == 0) {
//                return run_log_before(argc, argv);
//            }
//            else if (strcmp(argv[2], "-search") == 0) {
//                return run_log_search(argc, argv);
//            }
        }

    } else {
        printf(_SGR_REDB "invalid command\n"_SGR_RESET);
    }
    return 0;
}


