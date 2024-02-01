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
            break;
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
                break;
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
    } else {
        printf(_SGR_REDB "invalid command\n"_SGR_RESET);
    }
    return 0;
}


