//Ali Moghadasi
//402106542

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
#include <libgen.h>

char *root_email;
char *root_username;
char *root_path = "/Users/ali/Documents/Daaneshgah/Term1/FOP/Project/data/";

char *get_relative_path(char *absolute_path, char *base_path) {
    char *occurrence = strstr(absolute_path, base_path);
    if (occurrence != NULL) {
        return occurrence + strlen(base_path);
    }
    return NULL;
}

void truncate_path(char *path) {
    char *last_slash = strrchr(path, '/');
    if (last_slash != NULL) {
        *last_slash = '\0';
    }
}

char *file_name_maker(char *line) {
    char *file_name = malloc(2000);
    strcpy(file_name, line);
    int i = strlen(file_name), j = 0;
    while (file_name[i] != '/') {
        j++;
        i--;
    }
    memmove(file_name, line + i + 1, j * sizeof(char));
    return file_name;
}

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

void delete_files(char *source_dir) {
    DIR *dir;
    struct dirent *entry;

    if ((dir = opendir(source_dir)) == NULL) {
        perror("opendir");
        exit(EXIT_FAILURE);
    }
    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".kiwit") != 0 && strcmp(entry->d_name, ".") != 0 &&
            strcmp(entry->d_name, "..") != 0) {
            char *file_path = malloc(strlen(source_dir) + strlen(entry->d_name) + 2);
            sprintf(file_path, "%s/%s", source_dir, entry->d_name);
            if (entry->d_type == DT_DIR) {
                delete_files(file_path); // Call function recursively on the directory
                rmdir(file_path);
                //printf("DR Removed %s\n", file_path);
            } else {
                remove(file_path);
                //printf("RE Removed %s\n", file_path);
            }
            free(file_path);
        }
    }
    closedir(dir);
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
    fprintf(file, "1\n");
    fclose(file);

    file = fopen(".kiwit/all_branch_names", "w");
    fprintf(file, "master\n");
    fclose(file);

    file = fopen(".kiwit/checkouted", "w");
    fclose(file);

    file = fopen(".kiwit/commits/master/last_commit_id", "w");
    fprintf(file, "1\n");
    fclose(file);

    file = fopen(".kiwit/commit_message_shortcuts", "w");
    fclose(file);

    file = fopen(".kiwit/all_hooks", "w");
    fprintf(file, "todo-check\neof-blank-space\nformat-check\nbalance-braces\nfile-size-check\ncharacter-limit\nstatic-error-check\ntime-limit");
    fclose(file);

    file = fopen(".kiwit/applied_hooks", "w");
    fclose(file);

    file = fopen(".kiwit/formats", "w");
    fprintf(file, ".txt\n.c\n.cpp\n.mp4\n.wav\n.mp3\n.php\n");
    fclose(file);

    file = fopen(".kiwit/current_branch", "w");
    fprintf(file, path_maker(find_source(), ".kiwit/commits/master"));
    fclose(file);

    file = fopen(".kiwit/commits/all_logs", "w");
    fclose(file);

    file = fopen(".kiwit/commits/all_commits_address", "w");
    fclose(file);

    file = fopen(".kiwit/deleted", "w");
    fclose(file);

    file = fopen(".kiwit/tag_names", "w");
    fclose(file);

    file = fopen(".kiwit/tags", "w");
    fclose(file);

//    file = fopen(".kiwit/number", "w");
//    fprintf(file, "0\n");
//    fclose(file);

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
            char **all_tracks = (char *) malloc(1000 * sizeof(char *));

            FILE *tracks = fopen(path_maker(find_source(), ".kiwit/tracks"), "r");
            char *line2 = malloc(1000);
            int counter = 0;
            while (fgets(line2, 1000, tracks) != NULL) {
                if (line2[strlen(line2) - 1] == '\n') {
                    line2[strlen(line2) - 1] = '\0';
                }
                all_tracks[counter] = malloc(1000);
                strcpy(all_tracks[counter], line2);
                counter++;
            }
            fclose(tracks);
            tracks = fopen(path_maker(find_source(), ".kiwit/tracks"), "a");
            int flag = 0;
            for (int k = 0; k < counter; ++k) {
                if (strcmp(all_tracks[k], file_path) == 0) {
                    flag = 1;
                    break;
                }
            }
            if (flag == 0) {
                fprintf(tracks, "%s\n", file_path);
            }
            fclose(tracks);
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

//    bool is_HEAD = true;
//    FILE *checkouted = fopen(path_maker(find_source(), ".kiwit/checkouted"), "r");
//    char *line = malloc(1030);
//    fgets(line, 1000, checkouted);
//    if (line == NULL) {
//        is_HEAD = true;
//    } else if (line[strlen(line) - 1] == '\n') {
//        line[strlen(line) - 1] = '\0';
//        char *this_commit_id = malloc(1000);
//        strcpy(this_commit_id, file_name_maker(line));
//        truncate_path(line);
//        strcat(line, "/last_commit_id");
//        FILE *last_commit_id = fopen(line, "r");
//        char *last_commit_id_str = malloc(1000);
//        fgets(last_commit_id_str, 1000, last_commit_id);
//        if (last_commit_id_str[strlen(last_commit_id_str) - 1] == '\n') {
//            last_commit_id_str[strlen(last_commit_id_str) - 1] = '\0';
//        }
//        fclose(last_commit_id);
//        int last_commit_id_int = atoi(last_commit_id_str);
//        last_commit_id_int--;
//        sprintf(last_commit_id_str, "%d", last_commit_id_int);
//        if (strcmp(this_commit_id, last_commit_id_str) == 0) {
//            is_HEAD = true;
//        } else {
//            is_HEAD = false;
//        }
//    }
//    if (is_HEAD == false) {
//        printf(_SGR_REDF "You are not in the HEAD of the branch.\n"_SGR_RESET);
//        printf(_SGR_REDF "You can not add in this state.\n"_SGR_RESET);
//        printf("please checkout to the HEAD of the branch.\n");
//        return 1;
//    }

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
                char **all_tracks = malloc(2000);
                FILE *tracks = fopen(path_maker(find_source(), ".kiwit/tracks"), "r");
                char *line2 = malloc(1000);
                int counter = 0;
                while (fgets(line2, 1000, tracks) != NULL) {
                    if (line2[strlen(line2) - 1] == '\n') {
                        line2[strlen(line2) - 1] = '\0';
                    }
                    all_tracks[counter] = malloc(1000);
                    strcpy(all_tracks[counter], line2);
                    counter++;
                }
                fclose(tracks);
                tracks = fopen(path_maker(find_source(), ".kiwit/tracks"), "a");
                int flag = 0;
                for (int j = 0; j < counter; ++j) {
                    if (strcmp(all_tracks[j], file_path) == 0) {
                        flag = 1;
                        break;
                    }
                }
                if (flag == 0) {
                    fprintf(tracks, "%s\n", file_path);
                }
                fclose(tracks);
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
    rewind(staged_files_address2);
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
        //printf("main Line:  %s\n", main_line);
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
        //printf("file_address_tmp: %s\n", file_address_tmp);
        FILE *tmp_output = fopen(path_maker(find_source(), ".kiwit/tmp_output"), "w");
        int flag_file_address_found = 0;
        rewind(staged_files_address2);
        while (fgets(line, 1024, staged_files_address2) != NULL) {
            if (line[strlen(line) - 1] == '\n') {
                line[strlen(line) - 1] = '\0';
            }
            if (strcmp(line, file_address_tmp) == 0) {
                continue;
            }
            //printf("line: %s\n", line);
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
        file_path[strlen(file_path)] = '\0';
        //printf("%s\n", file_path);
        if (is_dir(file_path) == -1) {
            printf(_SGR_REDF "There is no file or directory with this name: " _SGR_RESET);
            printf(_SGR_REDF "%s\n", argv[i]);
            printf(_SGR_RESET);
            //return 1;
        } else if (is_dir(file_path) == 0) {
            //printf(_C_L_PURPLE"file_path: %s\n"_SGR_RESET, file_path);
            FILE *staged_files_address2 = fopen(path_maker(find_source(), ".kiwit/staging_2"), "r");
            //         printf("%s\n", path_maker(find_source(), ".kiwit/staging_2"));
            char *line = malloc(1025);
            int flag = 0;
            rewind(staged_files_address2);
            while (fgets(line, 1000, staged_files_address2) != NULL) {
                if (line[strlen(line) - 1] == '\n') {
                    line[strlen(line) - 1] = '\0';
                }
                line_counter++;
//                printf(_C_L_PURPLE"line: %s\n"_SGR_RESET, line);
                //               printf(_C_L_PURPLE"file_path: %s\n"_SGR_RESET, file_path);
                if (strcmp(line, file_path) == 0) {
                    flag = 1;
                    break;
                }
            }
            if (flag == 0) {
                printf(_SGR_REDF "%s is not staged before.\n", argv[i]);
                continue;
            }
            fclose(staged_files_address2);
            FILE *staged_files_address1 = fopen(path_maker(find_source(), ".kiwit/staging"), "r");
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
            staged_files_address2 = fopen(path_maker(find_source(), ".kiwit/staging_2"), "r");
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
    bool is_HEAD = true;
    FILE *checkouted = fopen(path_maker(find_source(), ".kiwit/checkouted"), "r");
    char *line = malloc(1030);
    fgets(line, 1000, checkouted);
    if (line == NULL) {
        is_HEAD = true;
    } else if (line[strlen(line) - 1] == '\n') {
        line[strlen(line) - 1] = '\0';
        char *this_commit_id = malloc(1000);
        strcpy(this_commit_id, file_name_maker(line));
        truncate_path(line);
        strcat(line, "/last_commit_id");
        FILE *last_commit_id = fopen(line, "r");
        char *last_commit_id_str = malloc(1000);
        fgets(last_commit_id_str, 1000, last_commit_id);
        if (last_commit_id_str[strlen(last_commit_id_str) - 1] == '\n') {
            last_commit_id_str[strlen(last_commit_id_str) - 1] = '\0';
        }
        fclose(last_commit_id);
        int last_commit_id_int = atoi(last_commit_id_str);
        last_commit_id_int--;
        sprintf(last_commit_id_str, "%d", last_commit_id_int);
        if (strcmp(this_commit_id, last_commit_id_str) == 0) {
            is_HEAD = true;
        } else {
            is_HEAD = false;
        }
    }
    if (is_HEAD == false) {
        printf(_SGR_REDF "You are not in the HEAD of the branch.\n"_SGR_RESET);
        printf(_SGR_REDF "You can not commit in this state.\n"_SGR_RESET);
        printf("please checkout to the HEAD of the branch.\n");
        return 1;
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
    // commit counter for all branches
    FILE *commit_ID_file = fopen(path_maker(find_source(), ".kiwit/commit_ID"), "r");
    char *commit_ID = malloc(1025);
    fgets(commit_ID, 1024, commit_ID_file);
    if (commit_ID[strlen(commit_ID) - 1] == '\n') {
        commit_ID[strlen(commit_ID) - 1] = '\0';
    }
    fclose(commit_ID_file);
    int commit_ID_int = atoi(commit_ID);
    commit_ID_int++;
    commit_ID_file = fopen(path_maker(find_source(), ".kiwit/commit_ID"), "w");
    fprintf(commit_ID_file, "%d\n", commit_ID_int);
    fclose(commit_ID_file);
    // commit counter inside any branch
    FILE *last_commit_id = fopen(path_maker(branch_address, "/last_commit_id"), "r");
    char *last_commit_id_str = malloc(1025);
    fgets(last_commit_id_str, 1024, last_commit_id);
    if (last_commit_id_str[strlen(last_commit_id_str) - 1] == '\n') {
        last_commit_id_str[strlen(last_commit_id_str) - 1] = '\0';
    }
    fclose(last_commit_id);
    bool commit_exist = false;
    int last_commit_id_int = atoi(last_commit_id_str);
    if (last_commit_id_int != 1) {
        commit_exist = true;
    }
    last_commit_id_int++;
    last_commit_id = fopen(path_maker(branch_address, "/last_commit_id"), "w");
    fprintf(last_commit_id, "%d\n", last_commit_id_int);
    fclose(last_commit_id);

    char *commit_address = malloc(1025);
    strcpy(commit_address, branch_address);
    strcat(commit_address, "/");
    strcat(commit_address, last_commit_id_str);
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

    char *command = malloc(4000);
    if (commit_exist) {
        last_commit_id_int -= 2;
        sprintf(last_commit_id_str, "%d", last_commit_id_int);
        strcpy(command, "cp -r ");
        strcat(command, branch_address);
        strcat(command, "/");
        strcat(command, last_commit_id_str);
        last_commit_id_int += 1;
        char *commit_ID_2 = malloc(1025);
        sprintf(commit_ID_2, "%d", last_commit_id_int);
        strcat(command, "/ ");
        strcat(command, commit_address);
        system(command);
        last_commit_id_int += 1;
        sprintf(last_commit_id_str, "%d", last_commit_id_int);
    }

    char *past_commit_id = malloc(1025);
    sprintf(past_commit_id, "%d", last_commit_id_int - 2);
    commit_ID_int += 2;
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

    char *past_commit_address = malloc(1025);
    strcpy(past_commit_address, branch_address);
    strcat(past_commit_address, "/");
    strcat(past_commit_address, past_commit_id);

    if (commit_exist) {
        stage_file = fopen(path_maker(past_commit_address, "/data/staging"), "r");
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
        stage_file = fopen(path_maker(find_source(), ".kiwit/staging"), "r");
        commit_file = fopen(path_maker(commit_data_folder, "staging_tmp"), "w");
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

        FILE *file1 = fopen(path_maker(commit_data_folder, "staging"), "r");
        FILE *file2 = fopen(path_maker(commit_data_folder, "staging_tmp"), "r");
        FILE *file3 = fopen(path_maker(commit_data_folder, "staging_tmp2"), "w");
        char *line1 = malloc(1025);
        char *line2 = malloc(1025);
        int flag2 = 0;
        while (fgets(line2, 1024, file2) != NULL) {
            if (line2[strlen(line2) - 1] == '\n') {
                line2[strlen(line2) - 1] = '\0';
            }
            flag2 = 0;
            while (fgets(line1, 1024, file1) != NULL) {
                if (line1[strlen(line1) - 1] == '\n') {
                    line1[strlen(line1) - 1] = '\0';
                }
                if (strcmp(line1, line2) == 0) {
                    flag2 = 1;
                    break;
                }
            }
            rewind(file1);
            if (flag2 == 0) {
                fprintf(file3, "%s\n", line2);
            }
        }
        fclose(file1);
        fclose(file2);
        fclose(file3);
        remove(path_maker(commit_data_folder, "staging_tmp"));

        file1 = fopen(path_maker(commit_data_folder, "staging_tmp2"), "r");
        file2 = fopen(path_maker(commit_data_folder, "staging"), "a");
        while (fgets(line1, 1024, file1) != NULL) {
            if (line1[strlen(line1) - 1] == '\n') {
                line1[strlen(line1) - 1] = '\0';
            }
            fprintf(file2, "%s\n", line1);
        }
        fclose(file1);
        fclose(file2);
        remove(path_maker(commit_data_folder, "staging_tmp2"));
    } else {
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
    }

    if (commit_exist) {

        char *staging_2_address = malloc(1025);
        strcpy(staging_2_address, past_commit_address);
        strcat(staging_2_address, "/data/staging_2");
        strcpy(command, "cp ");
        strcat(command, staging_2_address);
        strcat(command, " ");
        strcat(command, commit_data_folder);
        system(command);

        FILE *file1 = fopen(path_maker(commit_data_folder, "staging_2"), "r");
        FILE *file2 = fopen(path_maker(find_source(), ".kiwit/staging_2"), "r");
        FILE *file3 = fopen(path_maker(commit_data_folder, "staging_2_tmp"), "w");
        char *line1 = malloc(2000);
        char *line2 = malloc(2000);
        int flag2 = 0;
        while (fgets(line2, 1024, file2) != NULL) {
            if (line2[strlen(line2) - 1] == '\n') {
                line2[strlen(line2) - 1] = '\0';
            }
            flag2 = 0;
            while (fgets(line1, 1024, file1) != NULL) {
                if (line1[strlen(line1) - 1] == '\n') {
                    line1[strlen(line1) - 1] = '\0';
                }
                if (strcmp(line1, line2) == 0) {
                    flag2 = 1;
                    break;
                }
            }
            rewind(file1);
            if (flag2 == 0) {
                fprintf(file3, "%s\n", line2);
            }
        }
        fclose(file1);
        fclose(file2);
        fclose(file3);

        file1 = fopen(path_maker(commit_data_folder, "staging_2_tmp"), "r");
        file2 = fopen(path_maker(commit_data_folder, "staging_2"), "a");
        while (fgets(line1, 1024, file1) != NULL) {
            if (line1[strlen(line1) - 1] == '\n') {
                line1[strlen(line1) - 1] = '\0';
            }
            fprintf(file2, "%s\n", line1);
        }
        fclose(file1);
        fclose(file2);
        remove(path_maker(commit_data_folder, "staging_2_tmp"));
    } else {
        char *staging_2_address = malloc(1025);
        strcpy(staging_2_address, find_source());
        strcat(staging_2_address, ".kiwit/staging_2");
        strcpy(command, "cp ");
        strcat(command, staging_2_address);
        strcat(command, " ");
        strcat(command, commit_data_folder);
        system(command);
    }

    stage_file = fopen(path_maker(find_source(), ".kiwit/staging"), "w");
    fclose(stage_file);
    stage_file = fopen(path_maker(find_source(), ".kiwit/staging_2"), "w");
    fclose(stage_file);

    FILE *unstage_file = fopen(path_maker(find_source(), ".kiwit/unstaging"), "w");
    fclose(stage_file);
    unstage_file = fopen(path_maker(find_source(), ".kiwit/unstaging_2"), "w");
    fclose(stage_file);

    char *unstaging_address = malloc(1025);
    strcpy(unstaging_address, find_source());
    strcat(unstaging_address, ".kiwit/unstaging_files");
    delete_files(unstaging_address);


    FILE *all_commits_address = fopen(path_maker(find_source(), ".kiwit/commits/all_commits_address"), "a");
    fprintf(all_commits_address, "%s\n", commit_address);
    fclose(all_commits_address);

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
    int flag = 0;
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
        flag = 1;
        counter++;
    }
    fclose(all_logs);
    if (flag == 0) {
        printf(_SGR_REDF "There is no commit in this project.\n"_SGR_RESET);
    }
}

int run_log_n(int argc, char *const argv[]) {
    if (strcmp(argv[2], "-n") != 0) {
        printf(_SGR_REDF "invalid command\n"_SGR_RESET);
        return 1;
    }
    int flag = 0;
    FILE *all_logs = fopen(path_maker(find_source(), ".kiwit/commits/all_logs"), "r");
    int counter = 0;
    char *line = malloc(1000);
    int n = atoi(argv[3]);
    while (fgets(line, 1000, all_logs) != NULL && counter < n * 7) {
        if (line[strlen(line) - 1] == '\n') {
            line[strlen(line) - 1] = '\0';
        }
        printf("%s\n", line);
        flag = 1;
        counter++;
    }
    fclose(all_logs);
    if (flag == 0) {
        printf(_SGR_REDF "There is no commit in this project.\n"_SGR_RESET);
    }
}

int run_log_branch(int argc, char *const argv[]) {
    if (strcmp(argv[2], "-branch") != 0) {
        printf(_SGR_REDF "invalid command\n"_SGR_RESET);
        return 1;
    }
    int flag = 0;
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
                    flag = 1;
                }
            }
        }
        counter++;
    }
    fclose(all_logs);
    if (flag == 0) {
        printf(_SGR_REDF "There is no commit in this branch.\n"_SGR_RESET);
    }
}

int run_log_author(int argc, char *const argv[]) {
    if (strcmp(argv[2], "-author") != 0) {
        printf(_SGR_REDF "invalid command\n"_SGR_RESET);
        return 1;
    }
    FILE *all_logs = fopen(path_maker(find_source(), ".kiwit/commits/all_logs"), "r");
    int flag = 0;
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
                    flag = 1;
                }
            }
        }
        counter++;
    }
    fclose(all_logs);
    if (flag == 0) {
        printf(_SGR_REDF "There is no commit with this author.\n"_SGR_RESET);
    }
}

// Sobhan
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
    FILE *all_logs = fopen(path_maker(find_source(), ".kiwit/commits/all_logs"), "r");
    int flag = 0;
    int counter = 0;
    char *line = malloc(1000);
    char **logs = malloc(8 * sizeof(char *));
    for (int i = 0; i < 8; ++i) {
        logs[i] = malloc(1000);
    }
    char *since_time_string = malloc(1000);
    strcpy(since_time_string, argv[3]);
    since_time_string[19] = '\0';
    time_t since_time = string_to_time(since_time_string);
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
            char *time_string = malloc(1000);
            strcpy(time_string, logs[0]);
            memmove(time_string, time_string + 20, strlen(time_string) - 20);
            time_string[19] = '\0';
            time_t commit_time = string_to_time(time_string);
            if (commit_time >= since_time) {
                for (int i = 0; i < 7; ++i) {
                    printf("%s\n", logs[i]);
                    flag = 1;
                }
            }
        }
        counter++;
    }
    fclose(all_logs);
    if (flag == 0) {
        printf(_SGR_REDF "There is no commit after this time.\n"_SGR_RESET);
    }
}

int run_log_before(int argc, char *const argv[]) {
    if (strcmp(argv[2], "-before") != 0) {
        printf(_SGR_REDF "invalid command\n"_SGR_RESET);
        return 1;
    }
    FILE *all_logs = fopen(path_maker(find_source(), ".kiwit/commits/all_logs"), "r");
    int flag = 0;
    int counter = 0;
    char *line = malloc(1000);
    char **logs = malloc(8 * sizeof(char *));
    for (int i = 0; i < 8; ++i) {
        logs[i] = malloc(1000);
    }
    char *since_time_string = malloc(1000);
    strcpy(since_time_string, argv[3]);
    since_time_string[19] = '\0';
    time_t since_time = string_to_time(since_time_string);
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
            char *time_string = malloc(1000);
            strcpy(time_string, logs[0]);
            memmove(time_string, time_string + 20, strlen(time_string) - 20);
            time_string[19] = '\0';
            time_t commit_time = string_to_time(time_string);
            if (commit_time <= since_time) {
                for (int i = 0; i < 7; ++i) {
                    printf("%s\n", logs[i]);
                    flag = 1;
                }
            }
        }
        counter++;
    }
    fclose(all_logs);
    if (flag == 0) {
        printf(_SGR_REDF "There is no commit before this time.\n"_SGR_RESET);
    }
}

int run_log_search(int argc, char *const argv[]) {
    if (strcmp(argv[2], "-search") != 0) {
        printf(_SGR_REDF "invalid command\n"_SGR_RESET);
        return 1;
    }
    FILE *all_logs = fopen(path_maker(find_source(), ".kiwit/commits/all_logs"), "r");
    int counter = 0;
    int flag = 0;
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
            char *commit_message = malloc(1000);
            strcpy(commit_message, logs[1]);
            memmove(commit_message, commit_message + 23, strlen(commit_message) - 23);
            commit_message[strlen(commit_message) - 23] = '\0';
            if (strstr(commit_message, argv[3]) != NULL) {
                for (int i = 0; i < 7; ++i) {
                    printf("%s\n", logs[i]);
                    flag = 1;
                }
            }
        }
        counter++;
    }
    fclose(all_logs);
    if (flag == 0) {
        printf(_SGR_REDF "There is no commit with this word.\n"_SGR_RESET);
    }
}

int run_branch(int argc, char *const argv[]) {
    char *current_branch_address = malloc(1000);
    FILE *current_branch_file = fopen(path_maker(find_source(), ".kiwit/current_branch"), "r");
    fgets(current_branch_address, 1000, current_branch_file);
    if (current_branch_address[strlen(current_branch_address) - 1] == '\n') {
        current_branch_address[strlen(current_branch_address) - 1] = '\0';
    }
    fclose(current_branch_file);
    char *current_branch_name = malloc(1000);
    strcpy(current_branch_name, file_name_maker(current_branch_address));
    if (argc == 2) {
        FILE *branches = fopen(path_maker(find_source(), ".kiwit/all_branch_names"), "r");
        char *line = malloc(1000);
        int flag = 0;
        while (fgets(line, 1000, branches) != NULL) {
            if (line[strlen(line) - 1] == '\n') {
                line[strlen(line) - 1] = '\0';
            }
            if (strcmp(line, current_branch_name) == 0) {
                printf(_SGR_GREENF"%s\n"_SGR_RESET, line);
            } else {
                printf("%s\n", line);
            }
            flag = 1;
        }
        fclose(branches);
        if (flag == 0) {
            printf(_SGR_REDF "There is no branch in this project.\n"_SGR_RESET);
        }
    } else if (argc == 3) {
        FILE *branches = fopen(path_maker(find_source(), ".kiwit/all_branch_names"), "r");
        char *line = malloc(1000);
        while (fgets(line, 1000, branches) != NULL) {
            if (line[strlen(line) - 1] == '\n') {
                line[strlen(line) - 1] = '\0';
            }
            if (strcmp(line, argv[2]) == 0) {
                printf(_SGR_REDF "This branch is already exist.\n"_SGR_RESET);
                return 1;
            }
        }
        fclose(branches);
        branches = fopen(path_maker(find_source(), ".kiwit/all_branch_names"), "a");
        fprintf(branches, "%s\n", argv[2]);
        fclose(branches);
        char *branch_address = malloc(1000);
        strcpy(branch_address, find_source());
        strcat(branch_address, ".kiwit/commits/");
        strcat(branch_address, argv[2]);
        if (mkdir(branch_address, 0755) != 0) {
            return 1;
        }


        char *current_branch_id = malloc(1000);
        FILE *last_commit_id = fopen(path_maker(current_branch_address, "/last_commit_id"), "r");
        fgets(current_branch_id, 1000, last_commit_id);
        if (current_branch_id[strlen(current_branch_id) - 1] == '\n') {
            current_branch_id[strlen(current_branch_id) - 1] = '\0';
        }
        fclose(last_commit_id);
        int last_commit_id_int = atoi(current_branch_id);
        last_commit_id_int--;
        sprintf(current_branch_id, "%d", last_commit_id_int);
        strcat(current_branch_address, "/");
        strcat(current_branch_address, current_branch_id);
        strcat(current_branch_address, "/");
        char *command = malloc(4000);
        strcpy(command, "cp -r ");
        strcat(command, current_branch_address);
        strcat(command, " ");
        strcat(command, branch_address);
        strcat(command, "/1/");
        system(command);

        last_commit_id = fopen(path_maker(branch_address, "/last_commit_id"), "w");
        fprintf(last_commit_id, "2\n");
        fclose(last_commit_id);

        FILE *commit_ID_file = fopen(path_maker(find_source(), ".kiwit/commit_ID"), "r");
        char *commit_ID = malloc(1025);
        fgets(commit_ID, 1024, commit_ID_file);
        if (commit_ID[strlen(commit_ID) - 1] == '\n') {
            commit_ID[strlen(commit_ID) - 1] = '\0';
        }
        fclose(commit_ID_file);
        int commit_ID_int = atoi(commit_ID);
        commit_ID_int++;
        commit_ID_file = fopen(path_maker(find_source(), ".kiwit/commit_ID"), "w");
        fprintf(commit_ID_file, "%d\n", commit_ID_int);
        fclose(commit_ID_file);

        //current_branch_address
        char *current_data_folder = malloc(2000);
        strcpy(current_data_folder, current_branch_address);
        strcat(current_data_folder, "data/");
        char *new_commit_folder = malloc(2000);
        strcpy(new_commit_folder, branch_address);
        strcat(new_commit_folder, "/1/");
        FILE *staging_file = fopen(path_maker(current_data_folder, "staging"), "r");
        FILE *new_staging_file = fopen(path_maker(new_commit_folder, "data/staging"), "w");
        char *file_name = malloc(1000);
        while (fgets(line, 1000, staging_file) != NULL) {
            if (line[strlen(line) - 1] == '\n') {
                line[strlen(line) - 1] = '\0';
            }
            strcpy(file_name, file_name_maker(line));
            sprintf(line, "%s%s", new_commit_folder, file_name);
            fprintf(new_staging_file, "%s\n", line);
        }
        fclose(staging_file);
        fclose(new_staging_file);

        new_commit_folder[strlen(new_commit_folder) - 1] = '\0';
        FILE *all_commits_address = fopen(path_maker(find_source(), ".kiwit/commits/all_commits_address"), "a");
        fprintf(all_commits_address, "%s\n", new_commit_folder);
        fclose(all_commits_address);
        FILE *new_commit_log_file = fopen(path_maker(new_commit_folder, "/data/commit_log"), "w");
        FILE *current_commit_log_file = fopen(path_maker(current_data_folder, "commit_log"), "r");
        FILE *all_logs = fopen(path_maker(find_source(), ".kiwit/commits/all_logs"), "r");
        FILE *all_logs_2 = fopen(path_maker(find_source(), ".kiwit/commits/all_logs_2"), "w");
        time_t t = time(NULL);
        struct tm tm = *localtime(&t);
        fgets(line, 1000, current_commit_log_file);
        fprintf(new_commit_log_file, "%d-%02d-%02d %02d:%02d:%02d\n", tm.tm_year + 1900, tm.tm_mon + 1,
                tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
        fprintf(all_logs_2, "The commit time is: %d-%02d-%02d %02d:%02d:%02d\n", tm.tm_year + 1900, tm.tm_mon + 1,
                tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
        fgets(line, 1000, current_commit_log_file);
        if (line[strlen(line) - 1] == '\n') {
            line[strlen(line) - 1] = '\0';
        }
        fprintf(new_commit_log_file, "%s\n", line);
        fgets(line, 1000, current_commit_log_file);
        FILE *config_file = fopen(path_maker(find_source(), ".kiwit/config"), "r");
        char *committer_username = malloc(1025);
        fgets(committer_username, 1024, config_file);
        if (committer_username[strlen(committer_username) - 1] == '\n') {
            committer_username[strlen(committer_username) - 1] = '\0';
        }
        fclose(config_file);
        fprintf(new_commit_log_file, "%s\n", committer_username);
        fprintf(all_logs_2, "The committer user.name is: %s\n", committer_username);
        fgets(line, 1000, current_commit_log_file);
        fprintf(new_commit_log_file, "%s\n", commit_ID);
        fprintf(all_logs_2, "The commit ID is: %s\n", commit_ID);
        fgets(line, 1000, current_commit_log_file);
        fprintf(new_commit_log_file, "%s\n", argv[2]);
        fprintf(all_logs_2, "The branch name is: %s\n", argv[2]);
        fgets(line, 1000, current_commit_log_file);
        if (line[strlen(line) - 1] == '\n') {
            line[strlen(line) - 1] = '\0';
        }
        fprintf(new_commit_log_file, "%s\n", line);
        fprintf(all_logs_2, "The files count of this commit is: %s\n", line);
        fclose(new_commit_log_file);
        fprintf(all_logs_2, "-\n");
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

        printf(_SGR_GREENF "The branch has been created successfully.\n"_SGR_RESET);
    }
}

int run_checkout(int argc, char *const argv[]) {
    if (atoi(argv[2]) == 0 && strcmp(argv[2], "HEAD") != 0) {
        FILE *branches = fopen(path_maker(find_source(), ".kiwit/all_branch_names"), "r");
        char *line = malloc(1000);
        bool branch_exist = false;
        while (fgets(line, 1000, branches) != NULL) {
            if (line[strlen(line) - 1] == '\n') {
                line[strlen(line) - 1] = '\0';
            }
            if (strcmp(line, argv[2]) == 0) {
                branch_exist = true;
                break;
            }
        }
        fclose(branches);
        if (branch_exist == false) {
            printf(_SGR_REDF "This branch is not exist.\n"_SGR_RESET);
            return 1;
        }
        FILE *staging_file = fopen(path_maker(find_source(), ".kiwit/staging"), "r");
        char *staging_line = malloc(1000);
        if (fgets(staging_line, 1000, staging_file) != NULL) {
            printf(_SGR_REDF "The staging is not empty.\n"_SGR_RESET);
            printf("Please commit your changes before checkout.\n");
            return 1;
        }
        char *dest_branch_name = malloc(2000);
        strcpy(dest_branch_name, argv[2]);
        dest_branch_name[strlen(dest_branch_name)] = '\0';
        char *source_branch_name = malloc(2000);
        FILE *current_branch = fopen(path_maker(find_source(), ".kiwit/current_branch"), "r");
        fgets(source_branch_name, 1000, current_branch);
        if (source_branch_name[strlen(source_branch_name) - 1] == '\n') {
            source_branch_name[strlen(source_branch_name) - 1] = '\0';
        }
        fclose(current_branch);
        source_branch_name = file_name_maker(source_branch_name);
        if (strcmp(source_branch_name, dest_branch_name) == 0) {
            printf(_SGR_REDF "You are already in this branch.\n"_SGR_RESET);
            return 1;
        }
        char *source_branch_address = malloc(2000);
        strcpy(source_branch_address, find_source());
        strcat(source_branch_address, ".kiwit/commits/");
        strcat(source_branch_address, source_branch_name);
        char *dest_branch_address = malloc(2000);
        strcpy(dest_branch_address, find_source());
        strcat(dest_branch_address, ".kiwit/commits/");
        strcat(dest_branch_address, dest_branch_name);
        FILE *dest_branch_last_commit_id = fopen(path_maker(dest_branch_address, "/last_commit_id"), "r");
        char *dest_branch_last_commit_id_string = malloc(1000);
        fgets(dest_branch_last_commit_id_string, 1000, dest_branch_last_commit_id);
        if (dest_branch_last_commit_id_string[strlen(dest_branch_last_commit_id_string) - 1] == '\n') {
            dest_branch_last_commit_id_string[strlen(dest_branch_last_commit_id_string) - 1] = '\0';
        }
        fclose(dest_branch_last_commit_id);
        bool dest_branch_is_empty = false;
        if (strcmp(dest_branch_last_commit_id_string, "2") == 0) {
            dest_branch_is_empty = true;
        }
        if (dest_branch_is_empty == true) {
            current_branch = fopen(path_maker(find_source(), ".kiwit/current_branch"), "w");
            fprintf(current_branch, "%s\n", dest_branch_address);
            fclose(current_branch);
            printf(_SGR_GREENF "The branch has been checked out successfully.\n"_SGR_RESET);
            printf("The branch name is: %s\n", dest_branch_name);
        } else {
            int dest_branch_last_commit_id_int = atoi(dest_branch_last_commit_id_string);
            dest_branch_last_commit_id_int--;
            sprintf(dest_branch_last_commit_id_string, "%d", dest_branch_last_commit_id_int);
            char *dest_branch_last_commit_data_address = malloc(2000);
            strcpy(dest_branch_last_commit_data_address, dest_branch_address);
            strcat(dest_branch_last_commit_data_address, "/");
            strcat(dest_branch_last_commit_data_address, dest_branch_last_commit_id_string);
            strcat(dest_branch_last_commit_data_address, "/data/");

            char source_dir[2024];
            if (getcwd(source_dir, sizeof(source_dir)) == NULL)
                return 0;
            delete_files(source_dir);

            staging_file = fopen(path_maker(dest_branch_last_commit_data_address, "staging"), "r");
            FILE *staging_file_2 = fopen(path_maker(dest_branch_last_commit_data_address, "staging_2"), "r");
            char line_2[1024];
            char *source_dir_slash = malloc(2000);
            strcpy(source_dir_slash, source_dir);
            strcat(source_dir_slash, "/");
            while (fgets(line, 2000, staging_file) != NULL) {
                fgets(line_2, 2000, staging_file_2);
                if (line[strlen(line) - 1] == '\n') {
                    line[strlen(line) - 1] = '\0';
                }
                if (line_2[strlen(line_2) - 1] == '\n') {
                    line_2[strlen(line_2) - 1] = '\0';
                }
                char *file_name = file_name_maker(line_2);

                char *file_address = malloc(2000);
                strncpy(file_address, line_2, strlen(line_2) - strlen(file_name));
                char *relative_path = get_relative_path(file_address, source_dir_slash);
                char *command = malloc(4000);
                if (relative_path != NULL) {
                    strcpy(command, "mkdir -p ");
                    strcat(command, relative_path);
                    strcat(command, " > /dev/null 2>&1");
                    system(command);
                }
                strcpy(command, "cp ");
                strcat(command, line);
                strcat(command, " ");
                strcat(command, line_2);
                system(command);
            }

            fclose(staging_file);
            fclose(staging_file_2);
            printf(_SGR_GREENF "The branch has been checked out successfully.\n"_SGR_RESET);
            printf("The branch name is: %s\n", dest_branch_name);
        }
    } else if (strcmp(argv[2], "HEAD") == 0) {
        //printf("salam\n");
        char *current_branch_address = malloc(1000);

        FILE *current_branch_file = fopen(path_maker(find_source(), ".kiwit/current_branch"), "r");
        fgets(current_branch_address, 1000, current_branch_file);
        if (current_branch_address[strlen(current_branch_address) - 1] == '\n') {
            current_branch_address[strlen(current_branch_address) - 1] = '\0';
        }
        fclose(current_branch_file);
        char *last_commit_id = malloc(1000);

        FILE *last_commit_id_file = fopen(path_maker(current_branch_address, "/last_commit_id"), "r");
        fgets(last_commit_id, 1000, last_commit_id_file);
        if (last_commit_id[strlen(last_commit_id) - 1] == '\n') {
            last_commit_id[strlen(last_commit_id) - 1] = '\0';
        }
        fclose(last_commit_id_file);
        int last_commit_id_int = atoi(last_commit_id);
        last_commit_id_int--;
        sprintf(last_commit_id, "%d", last_commit_id_int);
        char *last_commit_address = malloc(2000);
        strcpy(last_commit_address, current_branch_address);
        strcat(last_commit_address, "/");
        strcat(last_commit_address, last_commit_id);

        FILE *checkouted = fopen(path_maker(find_source(), ".kiwit/checkouted"), "w");
        fprintf(checkouted, "%s\n", last_commit_address);
        fclose(checkouted);

        char *dest_branch_last_commit_data_address = malloc(2000);
        strcpy(dest_branch_last_commit_data_address, last_commit_address);
        strcat(dest_branch_last_commit_data_address, "/data/");

        char source_dir[2024];
        if (getcwd(source_dir, sizeof(source_dir)) == NULL)
            return 0;
        delete_files(source_dir);


        FILE *staging_file = fopen(path_maker(dest_branch_last_commit_data_address, "staging"), "r");
        FILE *staging_file_2 = fopen(path_maker(dest_branch_last_commit_data_address, "staging_2"), "r");
        char line_2[1024];
        char *source_dir_slash = malloc(2000);
        strcpy(source_dir_slash, source_dir);
        strcat(source_dir_slash, "/");
        char *line = malloc(2000);
        while (fgets(line, 2000, staging_file) != NULL) {
            fgets(line_2, 2000, staging_file_2);
            if (line[strlen(line) - 1] == '\n') {
                line[strlen(line) - 1] = '\0';
            }
            if (line_2[strlen(line_2) - 1] == '\n') {
                line_2[strlen(line_2) - 1] = '\0';
            }
            char *file_name = file_name_maker(line_2);

            char *file_address = malloc(2000);
            strncpy(file_address, line_2, strlen(line_2) - strlen(file_name));
            char *relative_path = get_relative_path(file_address, source_dir_slash);
            char *command = malloc(4000);
            if (relative_path != NULL) {
                strcpy(command, "mkdir -p ");
                strcat(command, relative_path);
                strcat(command, " > /dev/null 2>&1");
                system(command);
            }
            strcpy(command, "cp ");
            strcat(command, line);
            strcat(command, " ");
            strcat(command, line_2);
            system(command);
        }

        fclose(staging_file);
        fclose(staging_file_2);

        printf(_SGR_GREENF "The commit ID has been checked out successfully.\n"_SGR_RESET);
        printf("Now you are on the HEAD of the current branch.\n");


    } else {
        FILE *last_unique_commit_id = fopen(path_maker(find_source(), ".kiwit/commit_ID"), "r");
        char *last_commit_id = malloc(1000);
        fgets(last_commit_id, 1000, last_unique_commit_id);
        if (last_commit_id[strlen(last_commit_id) - 1] == '\n') {
            last_commit_id[strlen(last_commit_id) - 1] = '\0';
        }
        fclose(last_unique_commit_id);
        if (atoi(argv[2]) > (atoi(last_commit_id) - 1) || atoi(argv[2]) < 1) {
            printf(_SGR_REDF "This commit is not exist.\n"_SGR_RESET);
            return 1;
        }
        FILE *staging_file = fopen(path_maker(find_source(), ".kiwit/staging"), "r");
        char *staging_line = malloc(1000);
        if (fgets(staging_line, 1000, staging_file) != NULL) {
            printf(_SGR_REDF "The staging is not empty.\n"_SGR_RESET);
            printf("Please commit your changes before checkout.\n");
            return 1;
        }
        char *dest_commit_id = malloc(1000);
        strcpy(dest_commit_id, argv[2]);
        dest_commit_id[strlen(dest_commit_id)] = '\0';
        char *dest_commit_address = malloc(2000);
        char *source_commit_id = malloc(1000);
        FILE *all_commits_address = fopen(path_maker(find_source(), ".kiwit/commits/all_commits_address"), "r");
        char *line = malloc(1000);
        int counter = 1;
        while (fgets(line, 1000, all_commits_address) != NULL) {
            if (counter == atoi(dest_commit_id)) {
                strcpy(dest_commit_address, line);
                if (dest_commit_address[strlen(dest_commit_address) - 1] == '\n') {
                    dest_commit_address[strlen(dest_commit_address) - 1] = '\0';
                }
                break;
            }
            counter++;
        }
        fclose(all_commits_address);

        //changing the current branch
        char *temp = malloc(2000);
        strcpy(temp, dest_commit_address);
        truncate_path(temp);
        //printf("%s\n", temp);
        FILE *current_branch = fopen(path_maker(find_source(), ".kiwit/current_branch"), "w");
        fprintf(current_branch, "%s\n", temp);
        fclose(current_branch);

        char *dest_branch_last_commit_data_address = malloc(2000);
        strcpy(dest_branch_last_commit_data_address, dest_commit_address);
        strcat(dest_branch_last_commit_data_address, "/data/");

        char source_dir[2024];
        if (getcwd(source_dir, sizeof(source_dir)) == NULL)
            return 0;
        delete_files(source_dir);

        staging_file = fopen(path_maker(dest_branch_last_commit_data_address, "staging"), "r");
        FILE *staging_file_2 = fopen(path_maker(dest_branch_last_commit_data_address, "staging_2"), "r");
        char line_2[1024];
        char *source_dir_slash = malloc(2000);
        strcpy(source_dir_slash, source_dir);
        strcat(source_dir_slash, "/");
        while (fgets(line, 2000, staging_file) != NULL) {
            fgets(line_2, 2000, staging_file_2);
            if (line[strlen(line) - 1] == '\n') {
                line[strlen(line) - 1] = '\0';
            }
            if (line_2[strlen(line_2) - 1] == '\n') {
                line_2[strlen(line_2) - 1] = '\0';
            }
            char *file_name = file_name_maker(line_2);

            char *file_address = malloc(2000);
            strncpy(file_address, line_2, strlen(line_2) - strlen(file_name));
            char *relative_path = get_relative_path(file_address, source_dir_slash);
            char *command = malloc(4000);
            if (relative_path != NULL) {
                strcpy(command, "mkdir -p ");
                strcat(command, relative_path);
                strcat(command, " > /dev/null 2>&1");
                system(command);
            }
            strcpy(command, "cp ");
            strcat(command, line);
            strcat(command, " ");
            strcat(command, line_2);
            system(command);
        }

        fclose(staging_file);
        fclose(staging_file_2);

        FILE *checkouted = fopen(path_maker(find_source(), ".kiwit/checkouted"), "w");
        fprintf(checkouted, "%s\n", dest_commit_address);
        fclose(checkouted);

        printf(_SGR_GREENF "The commit ID has been checked out successfully.\n"_SGR_RESET);
        printf("The commit ID is: %s\n", dest_commit_id);
    }
}

void writeFilePathsToFile(const char *dirPath, FILE *outputFile) {
    DIR *directory = opendir(dirPath);
    if (directory == NULL) {
        printf("Cannot open directory '%s'\n", dirPath);
        return;
    }

    struct dirent *entry;
    while ((entry = readdir(directory)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0 ||
            strcmp(entry->d_name, ".kiwit") == 0 || strcmp(entry->d_name, ".DS_Store") == 0)
            continue;

        char filePath[1024];
        snprintf(filePath, sizeof(filePath), "%s/%s", dirPath, entry->d_name);

        struct stat sb;
        if (stat(filePath, &sb) == 0 && S_ISDIR(sb.st_mode)) {
            writeFilePathsToFile(filePath, outputFile);
        } else {
            fprintf(outputFile, "%s\n", filePath);
        }
    }

    closedir(directory);
}

int run_status(int argc, char *const argv[]) {
    FILE *outputFile = fopen(path_maker(find_source(), ".kiwit/temp_file.txt"), "w");
    if (outputFile == NULL) {
        printf("Cannot open output file\n");
        return 1;
    }
    writeFilePathsToFile("/Users/ali/Documents/Daaneshgah/test-kiwit-2", outputFile);
    fclose(outputFile);
    FILE *commit_addresses = fopen(path_maker(find_source(), ".kiwit/commits/all_commits_address"), "r");
    char *line = malloc(1000);
    char *last_commit_address = malloc(1000);
    while (fgets(line, 1000, commit_addresses) != NULL) {
        if (line[strlen(line) - 1] == '\n') {
            line[strlen(line) - 1] = '\0';
        }
        strcpy(last_commit_address, line);
    }
    fclose(commit_addresses);
    char **file_names_and_state = malloc(1000 * sizeof(char *));
    FILE *all_files = fopen(path_maker(find_source(), ".kiwit/temp_file.txt"), "r");
    FILE *staging_2 = fopen(path_maker(find_source(), ".kiwit/staging_2"), "r");
    char *line_2 = malloc(1000);
    int counter = 0;
    int flag = 0;
    while (fgets(line, 1000, all_files) != NULL) {
        if (line[strlen(line) - 1] == '\n') {
            line[strlen(line) - 1] = '\0';
        }
        flag = 0;
        file_names_and_state[counter] = malloc(1000);
        while (fgets(line_2, 1000, staging_2) != NULL) {
            if (line_2[strlen(line_2) - 1] == '\n') {
                line_2[strlen(line_2) - 1] = '\0';
            }
            if (strcmp(line, line_2) == 0) {

                strcpy(file_names_and_state[counter], file_name_maker(line));
                strcat(file_names_and_state[counter], " +");
                flag = 1;
                break;
            }
        }
        rewind(staging_2);
        if (flag == 0) {
            strcpy(file_names_and_state[counter], file_name_maker(line));
            strcat(file_names_and_state[counter], " -");
        }
        counter++;
    }
    fclose(all_files);
    fclose(staging_2);

    all_files = fopen(path_maker(find_source(), ".kiwit/temp_file.txt"), "r");
    staging_2 = fopen(path_maker(find_source(), ".kiwit/staging_2"), "r");
    FILE *last_commit_files = fopen(path_maker(last_commit_address, "/data/staging_2"), "r");
    char *line_3 = malloc(1000);
    int counter_2 = 0;
    int flag_2 = 0;
    while (fgets(line, 1000, all_files) != NULL) {
        if (line[strlen(line) - 1] == '\n') {
            line[strlen(line) - 1] = '\0';
        }
        flag_2 = 0;
        flag = 0;
        while (fgets(line_3, 1000, last_commit_files) != NULL) {
            if (line_3[strlen(line_3) - 1] == '\n') {
                line_3[strlen(line_3) - 1] = '\0';
            }
            fgets(line_2, 1000, staging_2);
            if (line_2[strlen(line_2) - 1] == '\n') {
                line_2[strlen(line_2) - 1] = '\0';
            }
            if (strcmp(line, line_2) == 0) {
                flag = 1;
            }
            if (strcmp(line, line_3) == 0) {
                flag_2 = 1;
                break;
            }
        }
        rewind(last_commit_files);
        rewind(staging_2);
        if (flag_2 == 0) {
            if (flag == 1) {
                strcat(file_names_and_state[counter_2], " M");
            } else {
                strcat(file_names_and_state[counter_2], " Not Tracked");
            }
        }
        counter_2++;
    }
    fclose(all_files);
    fclose(staging_2);
    fclose(last_commit_files);

    all_files = fopen(path_maker(find_source(), ".kiwit/temp_file.txt"), "r");
    FILE *trackes = fopen(path_maker(find_source(), ".kiwit/tracks"), "r");
    int counter_3 = 0;
    while (fgets(line, 1000, trackes) != NULL) {
        if (line[strlen(line) - 1] == '\n') {
            line[strlen(line) - 1] = '\0';
        }
        flag = 0;
        while (fgets(line_2, 1000, all_files) != NULL) {
            if (line_2[strlen(line_2) - 1] == '\n') {
                line_2[strlen(line_2) - 1] = '\0';
            }
            if (strcmp(line, line_2) == 0) {
                flag = 1;
                break;
            }
        }
        rewind(all_files);
        if (flag == 0) {
            file_names_and_state[counter] = malloc(1000);
            strcpy(file_names_and_state[counter], file_name_maker(line));
            strcat(file_names_and_state[counter], " -");
            strcat(file_names_and_state[counter], " D");
            counter++;
        }
        counter_3++;
    }
    fclose(all_files);
    fclose(trackes);
    all_files = fopen(path_maker(find_source(), ".kiwit/temp_file.txt"), "r");
    staging_2 = fopen(path_maker(find_source(), ".kiwit/staging_2"), "r");
    last_commit_files = fopen(path_maker(last_commit_address, "/data/staging_2"), "r");
    FILE *last_commit_files_2 = fopen(path_maker(last_commit_address, "/data/staging"), "r");
    counter_2 = 0;
    FILE *file1;
    FILE *file2;
    while (fgets(line, 1000, all_files) != NULL) {
        if (line[strlen(line) - 1] == '\n') {
            line[strlen(line) - 1] = '\0';
        }
        flag = 0;
        while (fgets(line_2, 1000, last_commit_files) != NULL) {
            if (line_2[strlen(line_2) - 1] == '\n') {
                line_2[strlen(line_2) - 1] = '\0';
            }
            fgets(line_3, 1000, last_commit_files_2);
            if (line_3[strlen(line_3) - 1] == '\n') {
                line_3[strlen(line_3) - 1] = '\0';
            }
            if (strcmp(line, line_2) == 0) {
                flag = 1;
                break;
            }
        }
        rewind(last_commit_files);
        rewind(last_commit_files_2);
        if (flag == 1) {
            file1 = fopen(line, "r");
            file2 = fopen(line_3, "r");
            int is_same = file_content_checker(file1, file2);
            fclose(file1);
            fclose(file2);
            if (is_same == 0) {
                strcat(file_names_and_state[counter_2], " M");
            } else {
                strcat(file_names_and_state[counter_2], " A");
            }
        }
        counter_2++;
    }
    fclose(all_files);
    fclose(staging_2);
    fclose(last_commit_files);
    for (int i = 0; i < counter; i++) {
        printf("%s\n", file_names_and_state[i]);
    }
}

int run_revert(int argc, char *const argv[]) {
    if (argc == 3) {
        if (strstr(argv[2], "HEAD") != NULL) {
            char *number = malloc(1000);
            sscanf(number, "HEAD-%s", argv[4]);
            int number_int = atoi(number);
            FILE *last_unique_commit_id = fopen(path_maker(find_source(), ".kiwit/commit_ID"), "r");
            char *last_commit_id = malloc(1000);
            fgets(last_commit_id, 1000, last_unique_commit_id);
            if (last_commit_id[strlen(last_commit_id) - 1] == '\n') {
                last_commit_id[strlen(last_commit_id) - 1] = '\0';
            }
            fclose(last_unique_commit_id);
            int final_commit_id = atoi(last_commit_id) - number_int;
            char *final_commit_id_string = malloc(1000);
            sprintf(final_commit_id_string, "%d", final_commit_id);
            char *command = malloc(2000);
            strcpy(command, "kiwit ");
            strcat(command, "revert ");
            strcat(command, final_commit_id_string);
            strcat(command, " > /dev/null 2>&1");
            system(command);
            printf(_SGR_GREENF "The revert has been done successfully.\n"_SGR_RESET);
        } else {
            char *commit_id = malloc(1000);
            strcpy(commit_id, argv[2]);
            commit_id[strlen(commit_id)] = '\0';
            int commit_id_int = atoi(commit_id);
            FILE *last_unique_commit_id = fopen(path_maker(find_source(), ".kiwit/commit_ID"), "r");
            char *last_commit_id = malloc(1000);
            fgets(last_commit_id, 1000, last_unique_commit_id);
            if (last_commit_id[strlen(last_commit_id) - 1] == '\n') {
                last_commit_id[strlen(last_commit_id) - 1] = '\0';
            }
            fclose(last_unique_commit_id);
            if (commit_id_int > (atoi(last_commit_id) - 1) || commit_id_int < 1) {
                printf(_SGR_REDF "This commit is not exist.\n"_SGR_RESET);
                return 1;
            }
            FILE *staging_file = fopen(path_maker(find_source(), ".kiwit/staging"), "r");
            char *staging_line = malloc(1000);
            if (fgets(staging_line, 1000, staging_file) != NULL) {
                printf(_SGR_REDF "The staging is not empty.\n"_SGR_RESET);
                printf("Please commit your changes before revert.\n");
                return 1;
            }
            char *command = malloc(2000);
            strcpy(command, "kiwit ");
            strcat(command, "checkout ");
            strcat(command, commit_id);
            strcat(command, " > /dev/null 2>&1");
            system(command);
            strcpy(command, "kiwit ");
            strcat(command, "add *");
            strcat(command, " > /dev/null 2>&1");
            system(command);
            strcpy(command, "kiwit ");
            strcat(command, "checkout HEAD");
            strcat(command, " > /dev/null 2>&1");
            system(command);
            strcpy(command, "kiwit ");
            strcat(command, "commit -m \"Revert to commit ");
            strcat(command, commit_id);
            strcat(command, "\"");
            strcat(command, " > /dev/null 2>&1");
            system(command);
            printf(_SGR_GREENF "The revert has been done successfully.\n"_SGR_RESET);
        }
    } else if (argc == 5) {
        if (strcmp(argv[2], "-m") != 0) {
            printf(_SGR_REDF "invalid command\n"_SGR_RESET);
            return 1;
        }
        if (strstr(argv[4], "HEAD") != NULL) {
            char *number = malloc(1000);
            sscanf(number, "HEAD-%s", argv[4]);
            int number_int = atoi(number);
            FILE *last_unique_commit_id = fopen(path_maker(find_source(), ".kiwit/commit_ID"), "r");
            char *last_commit_id = malloc(1000);
            fgets(last_commit_id, 1000, last_unique_commit_id);
            if (last_commit_id[strlen(last_commit_id) - 1] == '\n') {
                last_commit_id[strlen(last_commit_id) - 1] = '\0';
            }
            fclose(last_unique_commit_id);
            int final_commit_id = atoi(last_commit_id) - number_int;
            char *final_commit_id_string = malloc(1000);
            sprintf(final_commit_id_string, "%d", final_commit_id);
            char *commit_message = malloc(1000);
            strcpy(commit_message, argv[3]);
            commit_message[strlen(commit_message)] = '\0';
            char *command = malloc(2000);
            strcpy(command, "kiwit ");
            strcat(command, "revert ");
            strcat(command, "-m ");
            strcat(command, commit_message);
            strcat(command, " ");
            strcat(command, final_commit_id_string);
            strcat(command, " > /dev/null 2>&1");
            system(command);
            printf(_SGR_GREENF "The revert has been done successfully.\n"_SGR_RESET);
        } else {
            char *commit_message = malloc(1000);
            strcpy(commit_message, argv[3]);
            commit_message[strlen(commit_message)] = '\0';
            char *commit_id = malloc(1000);
            strcpy(commit_id, argv[4]);
            commit_id[strlen(commit_id)] = '\0';
            int commit_id_int = atoi(commit_id);
            FILE *last_unique_commit_id = fopen(path_maker(find_source(), ".kiwit/commit_ID"), "r");
            char *last_commit_id = malloc(1000);
            fgets(last_commit_id, 1000, last_unique_commit_id);
            if (last_commit_id[strlen(last_commit_id) - 1] == '\n') {
                last_commit_id[strlen(last_commit_id) - 1] = '\0';
            }
            fclose(last_unique_commit_id);
            if (commit_id_int > (atoi(last_commit_id) - 1) || commit_id_int < 1) {
                printf(_SGR_REDF "This commit is not exist.\n"_SGR_RESET);
                return 1;
            }
            FILE *staging_file = fopen(path_maker(find_source(), ".kiwit/staging"), "r");
            char *staging_line = malloc(1000);
            if (fgets(staging_line, 1000, staging_file) != NULL) {
                printf(_SGR_REDF "The staging is not empty.\n"_SGR_RESET);
                printf("Please commit your changes before revert.\n");
                return 1;
            }
            char *command = malloc(2000);
            strcpy(command, "kiwit ");
            strcat(command, "checkout ");
            strcat(command, commit_id);
            strcat(command, " > /dev/null 2>&1");
            system(command);
            strcpy(command, "kiwit ");
            strcat(command, "add *");
            strcat(command, " > /dev/null 2>&1");
            system(command);
            strcpy(command, "kiwit ");
            strcat(command, "checkout HEAD");
            strcat(command, " > /dev/null 2>&1");
            system(command);
            strcpy(command, "kiwit ");
            strcat(command, "commit -m \"");
            strcat(command, commit_message);
            strcat(command, "\"");
            strcat(command, " > /dev/null 2>&1");
            system(command);
            printf(_SGR_GREENF "The revert has been done successfully.\n"_SGR_RESET);
        }
    } else if (argc == 4) {
        if (strcmp(argv[2], "-n") != 0) {
            printf(_SGR_REDF "invalid command\n"_SGR_RESET);
            return 1;
        }
        char *commit_id = malloc(1000);
        strcpy(commit_id, argv[2]);
        commit_id[strlen(commit_id)] = '\0';
        int commit_id_int = atoi(commit_id);
        FILE *last_unique_commit_id = fopen(path_maker(find_source(), ".kiwit/commit_ID"), "r");
        char *last_commit_id = malloc(1000);
        fgets(last_commit_id, 1000, last_unique_commit_id);
        if (last_commit_id[strlen(last_commit_id) - 1] == '\n') {
            last_commit_id[strlen(last_commit_id) - 1] = '\0';
        }
        fclose(last_unique_commit_id);
        if (commit_id_int > (atoi(last_commit_id) - 1) || commit_id_int < 1) {
            printf(_SGR_REDF "This commit is not exist.\n"_SGR_RESET);
            return 1;
        }
        FILE *staging_file = fopen(path_maker(find_source(), ".kiwit/staging"), "r");
        char *staging_line = malloc(1000);
        if (fgets(staging_line, 1000, staging_file) != NULL) {
            printf(_SGR_REDF "The staging is not empty.\n"_SGR_RESET);
            printf("Please commit your changes before revert.\n");
            return 1;
        }
        char *command = malloc(2000);
        strcpy(command, "kiwit ");
        strcat(command, "checkout ");
        strcat(command, commit_id);
        strcat(command, " > /dev/null 2>&1");
        system(command);
        printf(_SGR_GREENF "The revert has been done successfully.\n"_SGR_RESET);
    } else {
        printf(_SGR_REDF "invalid command\n"_SGR_RESET);
        return 1;
    }
}

int compare(const void *a, const void *b) {
    return strcmp(*(char **) a, *(char **) b);
}

int run_tag(int argc, char *const argv[]) {
    if (argc == 2) {
        FILE *tags = fopen(path_maker(find_source(), ".kiwit/tag_names"), "r");
        char *line = malloc(1000);
        while (fgets(line, 1000, tags) != NULL) {
            if (line[strlen(line) - 1] == '\n') {
                line[strlen(line) - 1] = '\0';
            }
            printf("%s\n", line);
        }
        fclose(tags);
        return 1;
    }
    if (argc == 4 && strcmp(argv[2], "show") == 0) {
        char *line = malloc(1000);
        FILE *all_tags = fopen(path_maker(find_source(), ".kiwit/tags"), "r");
        int counter = 0;
        int flag = 0;
        char **tags = malloc(8 * sizeof(char *));
        for (int i = 0; i < 7; ++i) {
            tags[i] = malloc(1000);
        }
        while (fgets(line, 1000, all_tags) != NULL) {
            if (line[strlen(line) - 1] == '\n') {
                line[strlen(line) - 1] = '\0';
            }
            strcpy(tags[counter % 6], line);
            if (counter % 6 == 5) {
                char *tag_name = malloc(1000);
                strcpy(tag_name, tags[0]);
                if (strstr(tag_name, argv[3]) != NULL) {
                    for (int i = 0; i < 6; ++i) {
                        printf("%s\n", tags[i]);
                    }
                    flag = 1;
                }
            }
            counter++;
        }
        fclose(all_tags);
        if (flag == 0) {
            printf(_SGR_REDF "There is no tag with this word.\n"_SGR_RESET);
        }
        return 1;
    }
    bool is_f = false;
    if (strcmp(argv[2], "-a") != 0) {
        printf(_SGR_REDF "invalid command\n"_SGR_RESET);
        return 1;
    }
    if (argc == 5 && strcmp(argv[4], "-f") == 0) {
        is_f = true;
    }
    if (argc == 7 && strcmp(argv[6], "-f") == 0) {
        is_f = true;
    }
    if (argc == 9 && strcmp(argv[8], "-f") == 0) {
        is_f = true;
    }
    if (is_f == false) {
        FILE *tag_names = fopen(path_maker(find_source(), ".kiwit/tag_names"), "r");
        char *line = malloc(1000);
        while (fgets(line, 1000, tag_names) != NULL) {
            if (line[strlen(line) - 1] == '\n') {
                line[strlen(line) - 1] = '\0';
            }
            if (strcmp(line, argv[3]) == 0) {
                printf(_SGR_REDF "This tag name is already exist.\n"_SGR_RESET);
                return 1;
            }
        }
        fclose(tag_names);
    }
    FILE *last_unique_commit_id = fopen(path_maker(find_source(), ".kiwit/commit_ID"), "r");
    char *last_commit_id = malloc(1000);
    fgets(last_commit_id, 1000, last_unique_commit_id);
    if (last_commit_id[strlen(last_commit_id) - 1] == '\n') {
        last_commit_id[strlen(last_commit_id) - 1] = '\0';
    }
    int last_commit_id_int = atoi(last_commit_id);
    last_commit_id_int--;
    fclose(last_unique_commit_id);
    if (argc == 4) {
        FILE *tag_names;
        char *line = malloc(1000);
        tag_names = fopen(path_maker(find_source(), ".kiwit/tag_names"), "r");
        int counter = 0;
        char *all_tags[1000];
        while (fgets(line, 1000, tag_names) != NULL) {
            if (line[strlen(line) - 1] == '\n') {
                line[strlen(line) - 1] = '\0';
            }
            all_tags[counter] = malloc(1000);
            strcpy(all_tags[counter], line);
            counter++;
        }
        fclose(tag_names);
        all_tags[counter] = malloc(1000);
        strcpy(all_tags[counter], argv[3]);
        counter++;
        if (counter != 0) {
            qsort(all_tags, counter, sizeof(const char *), compare);
            tag_names = fopen(path_maker(find_source(), ".kiwit/tag_names"), "w");
            for (int i = 0; i < counter; ++i) {
                fprintf(tag_names, "%s\n", all_tags[i]);
            }
            fclose(tag_names);
        } else {
            tag_names = fopen(path_maker(find_source(), ".kiwit/tag_names"), "w");
            fprintf(tag_names, "%s\n", argv[3]);
            fclose(tag_names);
        }
        FILE *tags_2 = fopen(path_maker(find_source(), ".kiwit/tags_2"), "w");
        fprintf(tags_2, "tag %s\n", argv[3]);
        fprintf(tags_2, "commit %d\n", last_commit_id_int);
        FILE *config = fopen(path_maker(find_source(), ".kiwit/config"), "r");
        char *line_2 = malloc(1000);
        fgets(line_2, 1000, config);
        if (line_2[strlen(line_2) - 1] == '\n') {
            line_2[strlen(line_2) - 1] = '\0';
        }
        fprintf(tags_2, "Author: %s ", line_2);
        fgets(line_2, 1000, config);
        if (line_2[strlen(line_2) - 1] == '\n') {
            line_2[strlen(line_2) - 1] = '\0';
        }
        fprintf(tags_2, "<%s>\n", line_2);
        fclose(config);
        time_t t = time(NULL);
        struct tm tm = *localtime(&t);
        fprintf(tags_2, "Date: %d-%02d-%02d %02d:%02d:%02d\n", tm.tm_year + 1900, tm.tm_mon + 1,
                tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
        fprintf(tags_2, "Message:\n");
        fprintf(tags_2, "*-*\n");
        fclose(tags_2);
        FILE *tags = fopen(path_maker(find_source(), ".kiwit/tags"), "r");
        tags_2 = fopen(path_maker(find_source(), ".kiwit/tags_2"), "a");
        while (fgets(line, 1000, tags) != NULL) {
            if (line[strlen(line) - 1] == '\n') {
                line[strlen(line) - 1] = '\0';
            }
            fprintf(tags_2, "%s\n", line);
        }
        fclose(tags);
        fclose(tags_2);
        remove(path_maker(find_source(), ".kiwit/tags"));
        rename(path_maker(find_source(), ".kiwit/tags_2"), path_maker(find_source(), ".kiwit/tags"));
        printf(_SGR_GREENF "The tag has been added successfully.\n"_SGR_RESET);
    }
    if (argc == 5 && strcmp(argv[4], "-f") == 0) {
        char *line = malloc(1000);
        FILE *all_tags = fopen(path_maker(find_source(), ".kiwit/tags"), "r");
        FILE *tags_2 = fopen(path_maker(find_source(), ".kiwit/tags_2"), "w");
        int counter = 0;
        int flag = 0;
        char **tags = malloc(8 * sizeof(char *));
        for (int i = 0; i < 7; ++i) {
            tags[i] = malloc(1000);
        }
        while (fgets(line, 1000, all_tags) != NULL) {
            if (line[strlen(line) - 1] == '\n') {
                line[strlen(line) - 1] = '\0';
            }
            strcpy(tags[counter % 6], line);
            if (counter % 6 == 5) {
                char *tag_name = malloc(1000);
                strcpy(tag_name, tags[0]);
                if (strstr(tag_name, argv[3]) != NULL) {
                    fprintf(tags_2, "%s\n", tags[0]);
                    fprintf(tags_2, "commit %d\n", last_commit_id_int);
                    FILE *config = fopen(path_maker(find_source(), ".kiwit/config"), "r");
                    char *line_2 = malloc(1000);
                    fgets(line_2, 1000, config);
                    if (line_2[strlen(line_2) - 1] == '\n') {
                        line_2[strlen(line_2) - 1] = '\0';
                    }
                    fprintf(tags_2, "Author: %s ", line_2);
                    fgets(line_2, 1000, config);
                    if (line_2[strlen(line_2) - 1] == '\n') {
                        line_2[strlen(line_2) - 1] = '\0';
                    }
                    fprintf(tags_2, "<%s>\n", line_2);
                    fclose(config);
                    time_t t = time(NULL);
                    struct tm tm = *localtime(&t);
                    fprintf(tags_2, "Date: %d-%02d-%02d %02d:%02d:%02d\n", tm.tm_year + 1900, tm.tm_mon + 1,
                            tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
                    fprintf(tags_2, "%s\n", tags[4]);
                    fprintf(tags_2, "*-*\n");
                    flag = 1;
                } else {
                    for (int i = 0; i < 6; ++i) {
                        fprintf(tags_2, "%s\n", tags[i]);
                    }
                }
            }
            counter++;
        }
        fclose(all_tags);
        fclose(tags_2);
        remove(path_maker(find_source(), ".kiwit/tags"));
        rename(path_maker(find_source(), ".kiwit/tags_2"), path_maker(find_source(), ".kiwit/tags"));
        if (flag == 0) {
            printf(_SGR_REDF "There is no tag with this word.\n"_SGR_RESET);
        }
    }
    if (argc == 7 && strcmp(argv[6], "-f") == 0) {
        if (strcmp(argv[4], "-m") == 0) {
            char *line = malloc(1000);
            FILE *all_tags = fopen(path_maker(find_source(), ".kiwit/tags"), "r");
            FILE *tags_2 = fopen(path_maker(find_source(), ".kiwit/tags_2"), "w");
            int counter = 0;
            int flag = 0;
            char **tags = malloc(8 * sizeof(char *));
            for (int i = 0; i < 7; ++i) {
                tags[i] = malloc(1000);
            }
            while (fgets(line, 1000, all_tags) != NULL) {
                if (line[strlen(line) - 1] == '\n') {
                    line[strlen(line) - 1] = '\0';
                }
                strcpy(tags[counter % 6], line);
                if (counter % 6 == 5) {
                    char *tag_name = malloc(1000);
                    strcpy(tag_name, tags[0]);
                    if (strstr(tag_name, argv[3]) != NULL) {
                        fprintf(tags_2, "%s\n", tags[0]);
                        fprintf(tags_2, "commit %d\n", last_commit_id_int);
                        FILE *config = fopen(path_maker(find_source(), ".kiwit/config"), "r");
                        char *line_2 = malloc(1000);
                        fgets(line_2, 1000, config);
                        if (line_2[strlen(line_2) - 1] == '\n') {
                            line_2[strlen(line_2) - 1] = '\0';
                        }
                        fprintf(tags_2, "Author: %s ", line_2);
                        fgets(line_2, 1000, config);
                        if (line_2[strlen(line_2) - 1] == '\n') {
                            line_2[strlen(line_2) - 1] = '\0';
                        }
                        fprintf(tags_2, "<%s>\n", line_2);
                        fclose(config);
                        time_t t = time(NULL);
                        struct tm tm = *localtime(&t);
                        fprintf(tags_2, "Date: %d-%02d-%02d %02d:%02d:%02d\n", tm.tm_year + 1900, tm.tm_mon + 1,
                                tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
                        fprintf(tags_2, "Message: %s\n", argv[5]);
                        fprintf(tags_2, "*-*\n");
                        flag = 1;
                    } else {
                        for (int i = 0; i < 6; ++i) {
                            fprintf(tags_2, "%s\n", tags[i]);
                        }
                    }
                }
                counter++;
            }
            fclose(all_tags);
            fclose(tags_2);
            remove(path_maker(find_source(), ".kiwit/tags"));
            rename(path_maker(find_source(), ".kiwit/tags_2"), path_maker(find_source(), ".kiwit/tags"));
            if (flag == 0) {
                printf(_SGR_REDF "There is no tag with this word.\n"_SGR_RESET);
            }

        }
        if (strcmp(argv[4], "-c") == 0) {
            char *line = malloc(1000);
            FILE *all_tags = fopen(path_maker(find_source(), ".kiwit/tags"), "r");
            FILE *tags_2 = fopen(path_maker(find_source(), ".kiwit/tags_2"), "w");
            int counter = 0;
            int flag = 0;
            char **tags = malloc(8 * sizeof(char *));
            for (int i = 0; i < 7; ++i) {
                tags[i] = malloc(1000);
            }
            while (fgets(line, 1000, all_tags) != NULL) {
                if (line[strlen(line) - 1] == '\n') {
                    line[strlen(line) - 1] = '\0';
                }
                strcpy(tags[counter % 6], line);
                if (counter % 6 == 5) {
                    char *tag_name = malloc(1000);
                    strcpy(tag_name, tags[0]);
                    if (strstr(tag_name, argv[3]) != NULL) {
                        fprintf(tags_2, "%s\n", tags[0]);
                        fprintf(tags_2, "commit %s\n", argv[5]);
                        FILE *config = fopen(path_maker(find_source(), ".kiwit/config"), "r");
                        char *line_2 = malloc(1000);
                        fgets(line_2, 1000, config);
                        if (line_2[strlen(line_2) - 1] == '\n') {
                            line_2[strlen(line_2) - 1] = '\0';
                        }
                        fprintf(tags_2, "Author: %s ", line_2);
                        fgets(line_2, 1000, config);
                        if (line_2[strlen(line_2) - 1] == '\n') {
                            line_2[strlen(line_2) - 1] = '\0';
                        }
                        fprintf(tags_2, "<%s>\n", line_2);
                        fclose(config);
                        time_t t = time(NULL);
                        struct tm tm = *localtime(&t);
                        fprintf(tags_2, "Date: %d-%02d-%02d %02d:%02d:%02d\n", tm.tm_year + 1900, tm.tm_mon + 1,
                                tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
                        fprintf(tags_2, "%s\n", tags[4]);
                        fprintf(tags_2, "*-*\n");
                        flag = 1;
                    } else {
                        for (int i = 0; i < 6; ++i) {
                            fprintf(tags_2, "%s\n", tags[i]);
                        }
                    }
                }
                counter++;
            }
            fclose(all_tags);
            fclose(tags_2);
            remove(path_maker(find_source(), ".kiwit/tags"));
            rename(path_maker(find_source(), ".kiwit/tags_2"), path_maker(find_source(), ".kiwit/tags"));
            if (flag == 0) {
                printf(_SGR_REDF "There is no tag with this word.\n"_SGR_RESET);
            }

        }
    }
    if (argc == 9 && strcmp(argv[8], "-f") == 0) {
        if (strcmp(argv[4], "-m") == 0 && strcmp(argv[6], "-c") == 0) {
            char *line = malloc(1000);
            FILE *all_tags = fopen(path_maker(find_source(), ".kiwit/tags"), "r");
            FILE *tags_2 = fopen(path_maker(find_source(), ".kiwit/tags_2"), "w");
            int counter = 0;
            int flag = 0;
            char **tags = malloc(8 * sizeof(char *));
            for (int i = 0; i < 7; ++i) {
                tags[i] = malloc(1000);
            }
            while (fgets(line, 1000, all_tags) != NULL) {
                if (line[strlen(line) - 1] == '\n') {
                    line[strlen(line) - 1] = '\0';
                }
                strcpy(tags[counter % 6], line);
                if (counter % 6 == 5) {
                    char *tag_name = malloc(1000);
                    strcpy(tag_name, tags[0]);
                    if (strstr(tag_name, argv[3]) != NULL) {
                        fprintf(tags_2, "%s\n", tags[0]);
                        fprintf(tags_2, "commit %s\n", argv[7]);
                        FILE *config = fopen(path_maker(find_source(), ".kiwit/config"), "r");
                        char *line_2 = malloc(1000);
                        fgets(line_2, 1000, config);
                        if (line_2[strlen(line_2) - 1] == '\n') {
                            line_2[strlen(line_2) - 1] = '\0';
                        }
                        fprintf(tags_2, "Author: %s ", line_2);
                        fgets(line_2, 1000, config);
                        if (line_2[strlen(line_2) - 1] == '\n') {
                            line_2[strlen(line_2) - 1] = '\0';
                        }
                        fprintf(tags_2, "<%s>\n", line_2);
                        fclose(config);
                        time_t t = time(NULL);
                        struct tm tm = *localtime(&t);
                        fprintf(tags_2, "Date: %d-%02d-%02d %02d:%02d:%02d\n", tm.tm_year + 1900, tm.tm_mon + 1,
                                tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
                        fprintf(tags_2, "Message: %s\n", argv[5]);
                        fprintf(tags_2, "*-*\n");
                        flag = 1;
                    } else {
                        for (int i = 0; i < 6; ++i) {
                            fprintf(tags_2, "%s\n", tags[i]);
                        }
                    }
                }
                counter++;
            }
            fclose(all_tags);
            fclose(tags_2);
            remove(path_maker(find_source(), ".kiwit/tags"));
            rename(path_maker(find_source(), ".kiwit/tags_2"), path_maker(find_source(), ".kiwit/tags"));
            if (flag == 0) {
                printf(_SGR_REDF "There is no tag with this word.\n"_SGR_RESET);
            }

        }
    }
    if (argc == 6) {
        if (strcmp(argv[4], "-m") == 0) {
            FILE *tag_names;
            char *line = malloc(1000);
            tag_names = fopen(path_maker(find_source(), ".kiwit/tag_names"), "r");
            int counter = 0;
            char *all_tags[1000];
            while (fgets(line, 1000, tag_names) != NULL) {
                if (line[strlen(line) - 1] == '\n') {
                    line[strlen(line) - 1] = '\0';
                }
                all_tags[counter] = malloc(1000);
                strcpy(all_tags[counter], line);
                counter++;
            }
            fclose(tag_names);
            all_tags[counter] = malloc(1000);
            strcpy(all_tags[counter], argv[3]);
            counter++;
            if (counter != 0) {
                qsort(all_tags, counter, sizeof(const char *), compare);
                tag_names = fopen(path_maker(find_source(), ".kiwit/tag_names"), "w");
                for (int i = 0; i < counter; ++i) {
                    fprintf(tag_names, "%s\n", all_tags[i]);
                }
                fclose(tag_names);
            } else {
                tag_names = fopen(path_maker(find_source(), ".kiwit/tag_names"), "w");
                fprintf(tag_names, "%s\n", argv[3]);
                fclose(tag_names);
            }
            FILE *tags_2 = fopen(path_maker(find_source(), ".kiwit/tags_2"), "w");
            fprintf(tags_2, "tag %s\n", argv[3]);
            fprintf(tags_2, "commit %d\n", last_commit_id_int);
            FILE *config = fopen(path_maker(find_source(), ".kiwit/config"), "r");
            char *line_2 = malloc(1000);
            fgets(line_2, 1000, config);
            if (line_2[strlen(line_2) - 1] == '\n') {
                line_2[strlen(line_2) - 1] = '\0';
            }
            fprintf(tags_2, "Author: %s ", line_2);
            fgets(line_2, 1000, config);
            if (line_2[strlen(line_2) - 1] == '\n') {
                line_2[strlen(line_2) - 1] = '\0';
            }
            fprintf(tags_2, "<%s>\n", line_2);
            fclose(config);
            time_t t = time(NULL);
            struct tm tm = *localtime(&t);
            fprintf(tags_2, "Date: %d-%02d-%02d %02d:%02d:%02d\n", tm.tm_year + 1900, tm.tm_mon + 1,
                    tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
            fprintf(tags_2, "Message: %s\n", argv[5]);
            fprintf(tags_2, "*-*\n");
            fclose(tags_2);
            FILE *tags = fopen(path_maker(find_source(), ".kiwit/tags"), "r");
            tags_2 = fopen(path_maker(find_source(), ".kiwit/tags_2"), "a");
            while (fgets(line, 1000, tags) != NULL) {
                if (line[strlen(line) - 1] == '\n') {
                    line[strlen(line) - 1] = '\0';
                }
                fprintf(tags_2, "%s\n", line);
            }
            fclose(tags);
            fclose(tags_2);
            remove(path_maker(find_source(), ".kiwit/tags"));
            rename(path_maker(find_source(), ".kiwit/tags_2"), path_maker(find_source(), ".kiwit/tags"));
            printf(_SGR_GREENF "The tag has been added successfully.\n"_SGR_RESET);

        }
        if (strcmp(argv[4], "-c") == 0) {
            FILE *tag_names;
            char *line = malloc(1000);
            tag_names = fopen(path_maker(find_source(), ".kiwit/tag_names"), "r");
            int counter = 0;
            char *all_tags[1000];
            while (fgets(line, 1000, tag_names) != NULL) {
                if (line[strlen(line) - 1] == '\n') {
                    line[strlen(line) - 1] = '\0';
                }
                all_tags[counter] = malloc(1000);
                strcpy(all_tags[counter], line);
                counter++;
            }
            fclose(tag_names);
            all_tags[counter] = malloc(1000);
            strcpy(all_tags[counter], argv[3]);
            counter++;
            if (counter != 0) {
                qsort(all_tags, counter, sizeof(const char *), compare);
                tag_names = fopen(path_maker(find_source(), ".kiwit/tag_names"), "w");
                for (int i = 0; i < counter; ++i) {
                    fprintf(tag_names, "%s\n", all_tags[i]);
                }
                fclose(tag_names);
            } else {
                tag_names = fopen(path_maker(find_source(), ".kiwit/tag_names"), "w");
                fprintf(tag_names, "%s\n", argv[3]);
                fclose(tag_names);
            }
            FILE *tags_2 = fopen(path_maker(find_source(), ".kiwit/tags_2"), "w");
            fprintf(tags_2, "tag %s\n", argv[3]);
            fprintf(tags_2, "commit %s\n", argv[5]);
            FILE *config = fopen(path_maker(find_source(), ".kiwit/config"), "r");
            char *line_2 = malloc(1000);
            fgets(line_2, 1000, config);
            if (line_2[strlen(line_2) - 1] == '\n') {
                line_2[strlen(line_2) - 1] = '\0';
            }
            fprintf(tags_2, "Author: %s ", line_2);
            fgets(line_2, 1000, config);
            if (line_2[strlen(line_2) - 1] == '\n') {
                line_2[strlen(line_2) - 1] = '\0';
            }
            fprintf(tags_2, "<%s>\n", line_2);
            fclose(config);
            time_t t = time(NULL);
            struct tm tm = *localtime(&t);
            fprintf(tags_2, "Date: %d-%02d-%02d %02d:%02d:%02d\n", tm.tm_year + 1900, tm.tm_mon + 1,
                    tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
            fprintf(tags_2, "Message:\n");
            fprintf(tags_2, "*-*\n");
            fclose(tags_2);
            FILE *tags = fopen(path_maker(find_source(), ".kiwit/tags"), "r");
            tags_2 = fopen(path_maker(find_source(), ".kiwit/tags_2"), "a");
            while (fgets(line, 1000, tags) != NULL) {
                if (line[strlen(line) - 1] == '\n') {
                    line[strlen(line) - 1] = '\0';
                }
                fprintf(tags_2, "%s\n", line);
            }
            fclose(tags);
            fclose(tags_2);
            remove(path_maker(find_source(), ".kiwit/tags"));
            rename(path_maker(find_source(), ".kiwit/tags_2"), path_maker(find_source(), ".kiwit/tags"));
            printf(_SGR_GREENF "The tag has been added successfully.\n"_SGR_RESET);
        }
    }
    if (argc == 8) {
        if (strcmp(argv[4], "-m") == 0 && strcmp(argv[6], "-c") == 0) {
            FILE *tag_names;
            char *line = malloc(1000);
            tag_names = fopen(path_maker(find_source(), ".kiwit/tag_names"), "r");
            int counter = 0;
            char *all_tags[1000];
            while (fgets(line, 1000, tag_names) != NULL) {
                if (line[strlen(line) - 1] == '\n') {
                    line[strlen(line) - 1] = '\0';
                }
                all_tags[counter] = malloc(1000);
                strcpy(all_tags[counter], line);
                counter++;
            }
            fclose(tag_names);
            all_tags[counter] = malloc(1000);
            strcpy(all_tags[counter], argv[3]);
            counter++;
            if (counter != 0) {
                qsort(all_tags, counter, sizeof(const char *), compare);
                tag_names = fopen(path_maker(find_source(), ".kiwit/tag_names"), "w");
                for (int i = 0; i < counter; ++i) {
                    fprintf(tag_names, "%s\n", all_tags[i]);
                }
                fclose(tag_names);
            } else {
                tag_names = fopen(path_maker(find_source(), ".kiwit/tag_names"), "w");
                fprintf(tag_names, "%s\n", argv[3]);
                fclose(tag_names);
            }
            FILE *tags_2 = fopen(path_maker(find_source(), ".kiwit/tags_2"), "w");
            fprintf(tags_2, "tag %s\n", argv[3]);
            fprintf(tags_2, "commit %s\n", argv[7]);
            FILE *config = fopen(path_maker(find_source(), ".kiwit/config"), "r");
            char *line_2 = malloc(1000);
            fgets(line_2, 1000, config);
            if (line_2[strlen(line_2) - 1] == '\n') {
                line_2[strlen(line_2) - 1] = '\0';
            }
            fprintf(tags_2, "Author: %s ", line_2);
            fgets(line_2, 1000, config);
            if (line_2[strlen(line_2) - 1] == '\n') {
                line_2[strlen(line_2) - 1] = '\0';
            }
            fprintf(tags_2, "<%s>\n", line_2);
            fclose(config);
            time_t t = time(NULL);
            struct tm tm = *localtime(&t);
            fprintf(tags_2, "Date: %d-%02d-%02d %02d:%02d:%02d\n", tm.tm_year + 1900, tm.tm_mon + 1,
                    tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
            fprintf(tags_2, "Message: %s\n", argv[5]);
            fprintf(tags_2, "*-*\n");
            fclose(tags_2);
            FILE *tags = fopen(path_maker(find_source(), ".kiwit/tags"), "r");
            tags_2 = fopen(path_maker(find_source(), ".kiwit/tags_2"), "a");
            while (fgets(line, 1000, tags) != NULL) {
                if (line[strlen(line) - 1] == '\n') {
                    line[strlen(line) - 1] = '\0';
                }
                fprintf(tags_2, "%s\n", line);
            }
            fclose(tags);
            fclose(tags_2);
            remove(path_maker(find_source(), ".kiwit/tags"));
            rename(path_maker(find_source(), ".kiwit/tags_2"), path_maker(find_source(), ".kiwit/tags"));
            printf(_SGR_GREENF "The tag has been added successfully.\n"_SGR_RESET);
        }
    }
}

int todo_check(char *file_path) {
    FILE *formats = fopen(path_maker(find_source(), ".kiwit/formats"), "r");
    int flag = 0;
    char *format = malloc(1000);
    while (fgets(format, 1000, formats) != NULL) {
        if (format[strlen(format) - 1] == '\n') {
            format[strlen(format) - 1] = '\0';
        }
        if (strcmp(format, ".c") == 0 || strcmp(format, ".cpp") == 0 || strcmp(format, ".txt") == 0) {
            if (strstr(file_path, format) != NULL) {
                flag = 1;
                break;
            }
        }
    }
    if (flag == 0) {
        return 2;
    }
    FILE *file = fopen(file_path, "r");
    char *line = malloc(1000);
    while (fgets(line, 1000, file) != NULL) {
        if (line[strlen(line) - 1] == '\n') {
            line[strlen(line) - 1] = '\0';
        }
        if (strstr(line, "TODO") != NULL) {
            return 0;
        }
    }
    return 1;
}

int eof_blank_space(char *file_path) {
    FILE *formats = fopen(path_maker(find_source(), ".kiwit/formats"), "r");
    int flag = 0;
    char *format = malloc(1000);
    while (fgets(format, 1000, formats) != NULL) {
        if (format[strlen(format) - 1] == '\n') {
            format[strlen(format) - 1] = '\0';
        }

        if (strcmp(format, ".c") == 0 || strcmp(format, ".cpp") == 0 || strcmp(format, ".txt") == 0) {
            if (strstr(file_path, format) != NULL) {
                flag = 1;
                break;
            }
        }
    }
    if (flag == 0) {
        return 2;
    }
    FILE *file = fopen(file_path, "r");
    fseek(file, -1, SEEK_END);
    char lastChar = fgetc(file);
    fclose(file);
    if (lastChar == ' ' || lastChar == '\n' || lastChar == '\t') {
        return 0;
    }
    return 1;
}

int format_check(char *file_path) {
    FILE *formats = fopen(path_maker(find_source(), ".kiwit/formats"), "r");
    int flag = 0;
    char *format = malloc(1000);
    while (fgets(format, 1000, formats) != NULL) {
        if (format[strlen(format) - 1] == '\n') {
            format[strlen(format) - 1] = '\0';
        }
        if (strstr(file_path, format) != NULL) {
            return 1;
        }
    }
    return 0;
}

int balance_braces(char *file_path) {
    FILE *formats = fopen(path_maker(find_source(), ".kiwit/formats"), "r");
    int flag = 0;
    char *format = malloc(1000);
    while (fgets(format, 1000, formats) != NULL) {
        if (format[strlen(format) - 1] == '\n') {
            format[strlen(format) - 1] = '\0';
        }

        if (strcmp(format, ".c") == 0 || strcmp(format, ".cpp") == 0 || strcmp(format, ".txt") == 0) {
            if (strstr(file_path, format) != NULL) {
                flag = 1;
                break;
            }
        }
    }
    if (flag == 0) {
        return 2;
    }
    FILE *file = fopen(file_path, "r");
    int counterb1o = 0;
    int counterb1c = 0;
    int counterb2o = 0;
    int counterb2c = 0;
    int counterb3o = 0;
    int counterb3c = 0;
    char *line = malloc(1000);
    while (fgets(line, 1000, file) != NULL) {
        if (line[strlen(line) - 1] == '\n') {
            line[strlen(line) - 1] = '\0';
        }
        for (int i = 0; i < strlen(line); ++i) {
            if (line[i] == '{') {
                counterb1o++;
            }
            if (line[i] == '}') {
                counterb1c++;
            }
            if (line[i] == '(') {
                counterb2o++;
            }
            if (line[i] == ')') {
                counterb2c++;
            }
            if (line[i] == '[') {
                counterb3o++;
            }
            if (line[i] == ']') {
                counterb3c++;
            }
        }
    }
    fclose(file);
    if (counterb1o != counterb1c || counterb2o != counterb2c || counterb3o != counterb3c) {
        return 0;
    }
    return 1;
}

int file_size_check(char *file_path) {
    struct stat st;
    if (stat(file_path, &st) != 0) {
        return -1; // Error occurred, could not get file size
    }
    long int file_size = st.st_size;
    if (file_size > 5120000) {
        return 0;
    }
    return 1;
}

int character_limit(char *file_path) {
    FILE *formats = fopen(path_maker(find_source(), ".kiwit/formats"), "r");
    int flag = 0;
    char *format = malloc(1000);
    while (fgets(format, 1000, formats) != NULL) {
        if (format[strlen(format) - 1] == '\n') {
            format[strlen(format) - 1] = '\0';
        }

        if (strcmp(format, ".c") == 0 || strcmp(format, ".cpp") == 0 || strcmp(format, ".txt") == 0) {
            if (strstr(file_path, format) != NULL) {
                flag = 1;
                break;
            }
        }
    }
    if (flag == 0) {
        return 2;
    }
    FILE *file = fopen(file_path, "r");
    int counter = 0;
    char *line = malloc(1000);
    while (fgets(line, 1000, file) != NULL) {
        counter += strlen(line);
    }
    fclose(file);
    if (counter > 20000) {
        return 0;
    }
    return 1;
}

int static_error_check(char *file_path) {
    FILE *formats = fopen(path_maker(find_source(), ".kiwit/formats"), "r");
    int flag = 0;
    char *format = malloc(1000);
    while (fgets(format, 1000, formats) != NULL) {
        if (format[strlen(format) - 1] == '\n') {
            format[strlen(format) - 1] = '\0';
        }

        if (strcmp(format, ".c") == 0 || strcmp(format, ".cpp") == 0) {
            if (strstr(file_path, format) != NULL) {
                flag = 1;
                break;
            }
        }
    }
    if (flag == 0) {
        return 2;
    }
    char *command = malloc(1000);
    strcpy(command, "gcc ");
    strcat(command, file_path);
    strcat(command, " -o jsgflhsadgflajswgfelgfsj");
    int status = system(command);
    if (status != 0) {
        return 0;
    }
    return 1;
}

//int check_duration(const char* filename) {
//    SF_INFO info;
//    SNDFILE *sf = sf_open(filename, SFM_READ, &info);
//    if (sf == NULL) {
//        printf("Failed to open the file.\n");
//        return -1;
//    }
//
//    // Calculate duration in seconds
//    double duration = (double)info.frames / info.samplerate;
//
//    sf_close(sf);
//
//    // Check if duration is more than 10 minutes
//    if (duration > 600) {
//        return 0;
//    } else {
//        return 1;
//    }
//}
//

int run_pre_commit(int argc, const char *argv[]) {
    if (argc < 2) {
        printf(_SGR_REDF "invalid command\n"_SGR_RESET);
        return 1;
    }
    if (argc == 4 && strcmp(argv[2], "hooks") == 0 && strcmp(argv[3], "list") == 0) {
        FILE *all_hooks = fopen(path_maker(find_source(), ".kiwit/all_hooks"), "r");
        char *line = malloc(1000);
        while (fgets(line, 1000, all_hooks) != NULL) {
            if (line[strlen(line) - 1] == '\n') {
                line[strlen(line) - 1] = '\0';
            }
            printf("%s\n", line);
        }
        fclose(all_hooks);
        return 1;
    }
    if (argc == 4 && strcmp(argv[2], "applied") == 0 && strcmp(argv[3], "hooks") == 0) {
        FILE *applied_hooks = fopen(path_maker(find_source(), ".kiwit/applied_hooks"), "r");
        char *line = malloc(1000);
        while (fgets(line, 1000, applied_hooks) != NULL) {
            if (line[strlen(line) - 1] == '\n') {
                line[strlen(line) - 1] = '\0';
            }
            printf("%s\n", line);
        }
        fclose(applied_hooks);
        return 1;
    }
    if (argc == 5 && strcmp(argv[2], "add") == 0 && strcmp(argv[3], "hook") == 0) {
        FILE *all_hooks = fopen(path_maker(find_source(), ".kiwit/all_hooks"), "r");
        char *line = malloc(1000);
        int flag = 0;
        while (fgets(line, 1000, all_hooks) != NULL) {
            if (strstr(line, argv[4]) != NULL) {
                flag = 1;
                break;
            }
        }
        fclose(all_hooks);
        if (flag == 0) {
            printf(_SGR_REDF "This hook ID is not valied.\n"_SGR_RESET);
            return 1;
        }
        FILE *applied_hooks = fopen(path_maker(find_source(), ".kiwit/applied_hooks"), "r");
        while (fgets(line, 1000, applied_hooks) != NULL) {
            if (strstr(line, argv[4]) != NULL) {
                printf(_SGR_REDF "This hook is already exist.\n"_SGR_RESET);
                return 1;
            }
        }
        fclose(applied_hooks);
        applied_hooks = fopen(path_maker(find_source(), ".kiwit/applied_hooks"), "a");
        fprintf(applied_hooks, "%s\n", argv[4]);
        fclose(applied_hooks);
        printf(_SGR_GREENF "The hook has been added successfully.\n"_SGR_RESET);
        return 1;
    }
    if (argc == 5 && strcmp(argv[2], "remove") == 0 && strcmp(argv[3], "hook") == 0) {
        FILE *applied_hooks = fopen(path_maker(find_source(), ".kiwit/applied_hooks"), "r");
        char *line = malloc(1000);
        int flag = 0;
        while (fgets(line, 1000, applied_hooks) != NULL) {
            if (strstr(line, argv[4]) != NULL) {
                flag = 1;
                break;
            }
        }
        fclose(applied_hooks);
        if (flag == 0) {
            printf(_SGR_REDF "This hook ID is not added before.\n"_SGR_RESET);
            return 1;
        }
        applied_hooks = fopen(path_maker(find_source(), ".kiwit/applied_hooks"), "r");
        FILE *applied_hooks_2 = fopen(path_maker(find_source(), ".kiwit/applied_hooks_2"), "w");
        while (fgets(line, 1000, applied_hooks) != NULL) {
            if (strstr(line, argv[4]) == NULL) {
                fprintf(applied_hooks_2, "%s", line);
            }
        }
        fclose(applied_hooks);
        fclose(applied_hooks_2);
        remove(path_maker(find_source(), ".kiwit/applied_hooks"));
        rename(path_maker(find_source(), ".kiwit/applied_hooks_2"),
               path_maker(find_source(), ".kiwit/applied_hooks"));
        printf(_SGR_GREENF "The hook has been removed successfully.\n"_SGR_RESET);
        return 1;
    }
    if (argc == 2) {
        FILE *applied_hooks = fopen(path_maker(find_source(), ".kiwit/applied_hooks"), "r");
        char *line = malloc(1000);
        FILE *stage = fopen(path_maker(find_source(), ".kiwit/staging"), "r");
        char *file_path = malloc(2000);
        char *file_name = malloc(1000);
        while (fgets(file_path, 1000, stage) != NULL) {
            if (file_path[strlen(file_path) - 1] == '\n') {
                file_path[strlen(file_path) - 1] = '\0';
            }
            strcpy(file_name, file_name_maker(file_path));
            printf(_C_L_PURPLE"%s:\n"_SGR_RESET, file_name);
            rewind(applied_hooks);
            while (fgets(line, 1000, applied_hooks) != NULL) {
                if (line[strlen(line) - 1] == '\n') {
                    line[strlen(line) - 1] = '\0';
                }
                if (strcmp(line, "todo-check") == 0) {
                    int checker = todo_check(file_path);
                    if (checker == 0) {
                        printf(_SGR_REDF "%s....................FAILED\n"_SGR_RESET, line);
                    } else if (checker == 1) {
                        printf(_SGR_GREENF "%s....................PASSED\n"_SGR_RESET, line);
                    } else if (checker == 2) {
                        printf(_SGR_YELLOWF "%s....................SKIPPED\n"_SGR_RESET, line);
                    }
                }
                if (strcmp(line, "eof-blank-space") == 0) {
                    int checker = eof_blank_space(file_path);
                    if (checker == 0) {
                        printf(_SGR_REDF "%s...............FAILED\n"_SGR_RESET, line);
                    } else if (checker == 1) {
                        printf(_SGR_GREENF "%s...............PASSED\n"_SGR_RESET, line);
                    } else if (checker == 2) {
                        printf(_SGR_YELLOWF "%s...............SKIPPED\n"_SGR_RESET, line);
                    }
                }
                if (strcmp(line, "format-check") == 0) {
                    int checker = format_check(file_path);
                    if (checker == 0) {
                        printf(_SGR_REDF "%s..................FAILED\n"_SGR_RESET, line);
                    } else if (checker == 1) {
                        printf(_SGR_GREENF "%s..................PASSED\n"_SGR_RESET, line);
                    }
                }
                if (strcmp(line, "balance-braces") == 0) {
                    int checker = balance_braces(file_path);
                    if (checker == 0) {
                        printf(_SGR_REDF "%s................FAILED\n"_SGR_RESET, line);
                    } else if (checker == 1) {
                        printf(_SGR_GREENF "%s................PASSED\n"_SGR_RESET, line);
                    } else if (checker == 2) {
                        printf(_SGR_YELLOWF "%s................SKIPPED\n"_SGR_RESET, line);
                    }
                }
                if (strcmp(line, "file-size-check") == 0) {
                    int checker = file_size_check(file_path);
                    if (checker == 0) {
                        printf(_SGR_REDF "%s...............FAILED\n"_SGR_RESET, line);
                    } else if (checker == 1) {
                        printf(_SGR_GREENF "%s...............PASSED\n"_SGR_RESET, line);
                    }
                }
                if (strcmp(line, "character-limit") == 0) {
                    int checker = character_limit(file_path);
                    if (checker == 0) {
                        printf(_SGR_REDF "%s...............FAILED\n"_SGR_RESET, line);
                    } else if (checker == 1) {
                        printf(_SGR_GREENF "%s...............PASSED\n"_SGR_RESET, line);
                    } else if (checker == 2) {
                        printf(_SGR_YELLOWF "%s...............SKIPPED\n"_SGR_RESET, line);
                    }
                }
                if (strcmp(line, "static-error-check") == 0) {
                    int checker = static_error_check(file_path);
                    if (checker == 0) {
                        printf(_SGR_REDF "%s............FAILED\n"_SGR_RESET, line);
                    } else if (checker == 1) {
                        printf(_SGR_GREENF "%s............PASSED\n"_SGR_RESET, line);
                    } else if (checker == 2) {
                        printf(_SGR_YELLOWF "%s............SKIPPED\n"_SGR_RESET, line);
                    }
                }
                if (strcmp(line, "time-limit") == 0) {
                    int checker = static_error_check(file_path);
                    if (checker == 0) {
                        printf(_SGR_REDF "%s............FAILED\n"_SGR_RESET, line);
                    } else if (checker == 1) {
                        printf(_SGR_GREENF "%s............PASSED\n"_SGR_RESET, line);
                    } else if (checker == 2) {
                        printf(_SGR_YELLOWF "%s............SKIPPED\n"_SGR_RESET, line);
                    }
                }
            }
        }
        fclose(applied_hooks);
        fclose(stage);
    }
}

int run_grep(int argc, const char *argv[]) {
    if (strcmp(argv[2], "-f") != 0) {
        printf("Invalid command.\n");
        return 1;
    }
    if (strcmp(argv[4], "-p") != 0) {
        printf("Invalid command.\n");
        return 1;
    }
    char *filePath = malloc(2000);
    strcpy(filePath, path_maker(find_source(), argv[3]));
    char *word = malloc(1000);
    FILE *file = fopen(filePath, "r");
    if (file == NULL) {
        printf("Unable to open file.\n");
        return 1;
    }
    strcpy(word, argv[5]);
    char line[1024];
    int lineNumber = 1;
    if (argc == 6) {
        while (fgets(line, sizeof(line), file) != NULL) {
            if (strstr(line, word) != NULL) {
                for (int i = 0; i < strlen(line); i++) {
                    if (line[i] == word[0]) {
                        int flag = 1;
                        for (int j = 0; j < strlen(word); j++) {
                            if (line[i + j] != word[j]) {
                                flag = 0;
                                break;
                            }
                        }
                        if (flag == 1) {
                            printf(_SGR_GREENF);
                            for (int j = 0; j < strlen(word); j++) {
                                printf("%c", line[i + j]);
                            }
                            printf(_SGR_RESET);
                            i += strlen(word) - 1;
                        } else {
                            printf("%c", line[i]);
                        }
                    } else {
                        printf("%c", line[i]);
                    }
                }
            }
            lineNumber++;
        }
    } else if (strcmp(argv[6], "-n") == 0 && argc == 7) {
        while (fgets(line, sizeof(line), file) != NULL) {
            if (strstr(line, word) != NULL) {
                //printf("%d: %s", lineNumber, line);
                printf(_C_L_PURPLE"%d: "_SGR_RESET, lineNumber);
                for (int i = 0; i < strlen(line); i++) {
                    if (line[i] == word[0]) {
                        int flag = 1;
                        for (int j = 0; j < strlen(word); j++) {
                            if (line[i + j] != word[j]) {
                                flag = 0;
                                break;
                            }
                        }
                        if (flag == 1) {
                            printf(_SGR_GREENF);
                            for (int j = 0; j < strlen(word); j++) {
                                printf("%c", line[i + j]);
                            }
                            printf(_SGR_RESET);
                            i += strlen(word) - 1;
                        } else {
                            printf("%c", line[i]);
                        }
                    } else {
                        printf("%c", line[i]);
                    }
                }
            }
            lineNumber++;
        }
    }
    fclose(file);
}

int run_diff(int argc, const char *argv[]) {
    if (argc == 5) {
        char *file_path_1 = path_maker(find_source(), argv[3]);
        char *file_path_2 = path_maker(find_source(), argv[4]);
        FILE *file1 = fopen(file_path_1, "r");
        FILE *file2 = fopen(file_path_2, "r");

        if (file1 == NULL || file2 == NULL) {
            printf("Unable to open file.\n");
            return 1;
        }

        char line1[1024], line2[1024];
        int lineNumber1 = 1, lineNumber2 = 1;

        while (fgets(line1, sizeof(line1), file1) != NULL && fgets(line2, sizeof(line2), file2) != NULL) {
            if (strcmp(line1, "\n") != 0 && strcmp(line2, "\n") != 0) {
                if (strcmp(line1, line2) != 0) {
                    if (line1[strlen(line1) - 1] == '\n') {
                        line1[strlen(line1) - 1] = '\0';
                    }
                    if (line2[strlen(line2) - 1] == '\n') {
                        line2[strlen(line2) - 1] = '\0';
                    }
                    printf("«««««\n");
                    printf("%s-%d\n", file_name_maker(file_path_1), lineNumber1);
                    printf(_C_L_PURPLE"%s\n"_SGR_RESET, line1);
                    printf("%s-%d\n", file_name_maker(file_path_2), lineNumber2);
                    printf(_SGR_YELLOWF"%s\n"_SGR_RESET, line2);
                    printf("»»»»»\n");
                }
            }
            lineNumber1++;
            lineNumber2++;
        }

        fclose(file1);
        fclose(file2);
    } else if (argc == 11) {
        char *file_path_1 = path_maker(find_source(), argv[3]);
        char *file_path_2 = path_maker(find_source(), argv[4]);
        int begin1 = atoi(argv[6]);
        int end1 = atoi(argv[7]);
        int begin2 = atoi(argv[9]);
        int end2 = atoi(argv[10]);
        FILE *file1 = fopen(file_path_1, "r");
        FILE *file2 = fopen(file_path_2, "r");

        if (file1 == NULL || file2 == NULL) {
            printf("Unable to open file.\n");
            return 1;
        }

        char line1[1024], line2[1024];
        int lineNumber1 = 1, lineNumber2 = 1;

        while (fgets(line1, sizeof(line1), file1) != NULL && fgets(line2, sizeof(line2), file2) != NULL) {
            if (lineNumber1 >= begin1 && lineNumber1 <= end1 && lineNumber2 >= begin2 && lineNumber2 <= end2) {
                if (strcmp(line1, "\n") != 0 && strcmp(line2, "\n") != 0) {
                    if (strcmp(line1, line2) != 0) {
                        if (line2[strlen(line2) - 1] == '\n') {
                            line2[strlen(line2) - 1] = '\0';
                        }
                        if (line1[strlen(line1) - 1] == '\n') {
                            line1[strlen(line1) - 1] = '\0';
                        }
                        printf("«««««\n");
                        printf("%s-%d\n", file_name_maker(file_path_1), lineNumber1);
                        printf(_C_L_PURPLE"%s\n"_SGR_RESET, line1);
                        printf("%s-%d\n", file_name_maker(file_path_2), lineNumber2);
                        printf(_SGR_YELLOWF"%s\n"_SGR_RESET, line2);
                        printf("»»»»»\n");
                    }
                }
            }
            lineNumber1++;
            lineNumber2++;
        }

        fclose(file1);
        fclose(file2);
    }
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
            } else if (strcmp(argv[2], "-before") == 0) {
                return run_log_before(argc, argv);
            } else if (strcmp(argv[2], "-search") == 0) {
                return run_log_search(argc, argv);
            }
        }
    } else if (strcmp(command, "branch") == 0 && argc < 4) {
        return run_branch(argc, argv);
    } else if (strcmp(command, "checkout") == 0 && argc == 3) {
        return run_checkout(argc, argv);
    } else if (strcmp(command, "status") == 0 && argc == 2) {
        return run_status(argc, argv);
    } else if (strcmp(command, "revert") == 0 && (argc == 3 || argc == 5)) {
        return run_revert(argc, argv);
    } else if (strcmp(command, "tag") == 0 && argc >= 2) {
        return run_tag(argc, argv);
    } else if (strcmp(command, "pre-commit") == 0) {
        return run_pre_commit(argc, argv);
    } else if (strcmp(command, "grep") == 0 && argc >= 6) {
        return run_grep(argc, argv);
    } else if (strcmp(command, "diff") == 0 && argc >= 5) {
        return run_diff(argc, argv);
    } else {
        printf(_SGR_REDB "invalid command\n"_SGR_RESET);
    }
    return 0;
}





