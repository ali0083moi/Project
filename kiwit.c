// hints for me:
// line 0 = username
// line 1 = email

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ansi_color.h"
#include "logo_print.c"
#include <dirent.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>

char *root_email;
char *root_username;
char *root_path = "/Users/ali/Documents/Daaneshgah/Term1/FOP/Project/data/";

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

    file = fopen(".kiwit/staging", "w");
    fclose(file);

    file = fopen(".kiwit/tracks", "w");
    fclose(file);

    file = fopen(".kiwit/staging", "w");
    fclose(file);

    file = fopen(".kiwit/deleted", "w");
    fclose(file);

    file = fopen(".kiwit/number", "w");
    fprintf(file, "0\n");
    fclose(file);

    copy_file(path_maker(root_path, "root_alias"), ".kiwit/alias");

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
        return 1;
    } else if (root_email == NULL || (int) root_email[0] == 0) {
        printf(_SGR_REDF _SGR_BOLD "You did not set user.email\n"_SGR_RESET);
        printf(_SGR_GREENF "use kiwit config -global user.email <email>\n"_SGR_RESET);
        printf(_SGR_YELLOWF "then you can use kiwit init\n"_SGR_RESET);
        return 1;
    } else {
        printf(_SGR_REDF _SGR_BOLD "You did not set user.name and user.email\n"_SGR_RESET);
        printf(_SGR_GREENF "use kiwit config -global user.name <username>\nuse kiwit config -global user.email <email>\n"_SGR_RESET);
        printf(_SGR_YELLOWF "then you can use kiwit init\n"_SGR_RESET);
        return 1;
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
            printf(_SGR_REDF _SGR_BOLD "invalid command\n"_SGR_RESET);
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
    } else {
        if (argv[3] == NULL || argv[4] == NULL) {
            printf(_SGR_REDF _SGR_BOLD "invalid command\n"_SGR_RESET);
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
            printf(_SGR_REDF _SGR_BOLD "invalid command\n"_SGR_RESET);
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
    }
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
    for (int i = 2 + is_f; i < argc; ++i) {
        //printf("%s\n", argv[i]);
        char *file_path = find_source();
        strcat(file_path, argv[i]);
        if (is_dir(file_path) == -1) {
            printf(_SGR_REDF "There is no file or directory with this name!\n" _SGR_RESET);
            //return 1;
        } else if (is_dir(file_path) == 0) {
            char copied_file_address[1000];
            strcpy(copied_file_address, ".kiwit/staging_files/");
            strcat(copied_file_address, argv[i]);
            char *file_data = malloc(1000000);
            char *file_data_checker = malloc(1000000);
            //printf("%s\n", path_maker(find_source(), ".kiwit/staging"));
            FILE *src_file = fopen(path_maker(find_source(), ".kiwit/staging"), "r");
            FILE *file1;
            FILE *file2 = fopen(file_path, "r");
            fscanf(file2, "%[^\0]s", file_data);
            fclose(file2);

            char *line = malloc(2024);
            //printf("%s\n", copied_src);
            printf("%s\n", argv[i]);
//            while (fgets(line, 2024, src_file) != NULL) {
//                //printf("%s\n", line);
////                flag3 = 1;
//                flag2 = 0; // for checking if a file address is already added
//                flag = 0; //for deleted files
//                if (line[strlen(line) - 1] == '\n') {
//                    line[strlen(line) - 1] = '\0';
//                }
//                if (strcmp(line, path_maker(find_source(), copied_src)) == 0) {
//                    flag2 = 1;
//                }
//                //printf("%s\n", line);
//                // for checking if a file deleted
//                file1 = fopen(line, "r");
//                if (file1 == NULL) {
//                    flag = 1;
//                }
//                if (flag == 0) {
//                    fscanf(file1, "%[^\0]s", file_data_checker);
//                    //printf("%s\n", file_data_checker);
//                    unsigned long int file_hash = hash(file_data_checker);
//                    //printf("%lu\n", file_hash);
//                    if (file_hash == not_added_file_hash) {
//                        printf(_SGR_REDF "%s is already added\n", argv[i]);
//                        printf(_SGR_RESET);
//                        flag2 = 1;
//                    } else {
//                        copy_file(file_path, path_maker(find_source(), copied_src));
//                    }
//                }
//            }
//            fseek(src_file, 0, SEEK_SET);
//            if (flag2 == 0) {
//                copy_file(file_path, path_maker(find_source(), copied_src));
//                FILE *src_file2 = fopen(path_maker(find_source(), ".kiwit/staging"), "a");
//                fprintf(src_file2, "%s\n", path_maker(find_source(), copied_src));
//            }
        } else if (is_dir(file_path) == 1) {
            printf(_SGR_REDF "This is a directory\n" _SGR_RESET);
            //return 1;
        }
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
    if (strcmp(command, "init") == 0) {
        return run_init(argc, argv);
    } else if (strcmp(command, "config") == 0 && argc >= 3) {
        if (strcmp(argv[3], "alias") == 0 || strcmp(argv[2], "alias") == 0) {
            config_alias(argc, argv);
        } else {
            config_root(argc, argv);
        }
    } else if (strcmp(argv[1], "--start") == 0) {
        logo_print();
    } else if (strcmp(argv[1], "add") == 0) {
        return run_add(argc, argv);
    } else {
        printf(_SGR_REDB "invalid command\n"_SGR_RESET);
    }
    return 0;
}

