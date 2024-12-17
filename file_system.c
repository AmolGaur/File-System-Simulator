#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <direct.h>     
#include <unistd.h>      

#define MAX_NAME_LENGTH 100
#define MAX_FILES 100
#define MAX_DIRS 100

#define RESET_COLOR "\033[0m"
#define SUCCESS_COLOR "\033[0;32m"
#define ERROR_COLOR "\033[0;31m"
#define PROMPT_COLOR "\033[0;36m"
#define SAVE_FILE "filesystem_data.txt"

// File structure
typedef struct {
    char name[MAX_NAME_LENGTH];
    char *data;
    int size;
} File;

// Directory structure
typedef struct Directory {
    char name[MAX_NAME_LENGTH];
    struct Directory *parent;
    struct Directory *subdirs[MAX_DIRS];
    File *files[MAX_FILES];
    int num_subdirs;
    int num_files;
} Directory;

// FileSystem structure
typedef struct {
    Directory *root;
    Directory *current_dir;
} FileSystem;

// Function to get current path
void build_path(Directory *dir, char *path) {
    if (dir->parent == NULL) {  
        strcpy(path, "/");
        return;
    }

    // Recuresion
    build_path(dir->parent, path);

    if (strcmp(path, "/") != 0) {  
        strcat(path, "/");
    }
    strcat(path, dir->name);
}

// Function to get file path from local subsystem
void generate_file_path(Directory *dir, const char *filename, char *path, size_t path_size) {
    if (dir->parent == NULL) {
        snprintf(path, path_size, "./%s", filename);
    } else {
        char parent_path[MAX_NAME_LENGTH * 2] = "";
        generate_file_path(dir->parent, dir->name, parent_path, sizeof(parent_path));
        snprintf(path, path_size, "%s/%s", parent_path, filename);
    }
}

// Recursive Function to save data in filesystem_data.txt
void save_directory(FILE *file, Directory *dir) {

    if (dir->parent == NULL) {
        fprintf(file, "DIR /\n");
    } else {
        fprintf(file, "DIR %s %d\n", dir->name, dir->num_files);
    }

    for (int i = 0; i < dir->num_files; i++) {
        fprintf(file, "FILE %s %d %s\n", dir->files[i]->name, dir->files[i]->size, dir->files[i]->data);
    }
    for (int i = 0; i < dir->num_subdirs; i++) {
        save_directory(file, dir->subdirs[i]);
    }
    fprintf(file, "ENDDIR\n");
}

// Function to save file structure
void save_filesystem(FileSystem *fs) {
    FILE *file = fopen(SAVE_FILE, "w");
    if (!file) {
        printf(ERROR_COLOR "Error saving filesystem data.\n" RESET_COLOR);
        return;
    }
    save_directory(file, fs->root);
    fclose(file);
}

// Function to read filesystem_data.txt
Directory *load_directory(FILE *file, Directory *parent) {
    Directory *dir = (Directory *)malloc(sizeof(Directory));
    dir->parent = parent;
    dir->num_files = 0;
    dir->num_subdirs = 0;

    fscanf(file, " %s", dir->name);

    while (1) {
        char type[10];
        fscanf(file, " %s", type);

        if (strcmp(type, "FILE") == 0) {
            File *new_file = (File *)malloc(sizeof(File));
            fscanf(file, "%s %d", new_file->name, &new_file->size);
            new_file->data = (char *)malloc(new_file->size + 1);
            fscanf(file, " %[^\n]", new_file->data);
            dir->files[dir->num_files++] = new_file;
        } else if (strcmp(type, "DIR") == 0) {
            Directory *subdir = load_directory(file, dir);
            dir->subdirs[dir->num_subdirs++] = subdir;
        } else if (strcmp(type, "ENDDIR") == 0) {
            break;
        }
    }

    return dir;
}

// Function to load the file structure
void load_filesystem(FileSystem *fs) {
    FILE *file = fopen(SAVE_FILE, "r");
    if (!file) {
        printf(PROMPT_COLOR "No previous filesystem data found. Starting fresh.\n" RESET_COLOR);
        return;
    }
    fs->root = load_directory(file, NULL);
    fs->current_dir = fs->root;
    fclose(file);
}

// Function to create a new directory
void create_directory(FileSystem *fs, const char *dirname) {
    Directory *current = fs->current_dir;
    if (current->num_subdirs >= MAX_DIRS) {
        printf(ERROR_COLOR "Max subdirectories limit reached.\n" RESET_COLOR);
        return;
    }
    for (int i = 0; i < current->num_subdirs; i++) {
        if (strcmp(current->subdirs[i]->name, dirname) == 0) {
            printf(ERROR_COLOR "Directory already exists.\n" RESET_COLOR);
            return;
        }
    }

    Directory *new_dir = (Directory *)malloc(sizeof(Directory));
    strncpy(new_dir->name, dirname, MAX_NAME_LENGTH);
    new_dir->parent = current;
    new_dir->num_subdirs = 0;
    new_dir->num_files = 0;
    current->subdirs[current->num_subdirs++] = new_dir;

    // Create the directory on the system
    char path[MAX_NAME_LENGTH * 2] = "";
    if (current->parent == NULL) {
        // Root directory case
        snprintf(path, sizeof(path), "./%s", dirname);
    } else {
        snprintf(path, sizeof(path), "./%s/%s", current->name, dirname);
    }

    if (_mkdir(path) == 0) { // Use _mkdir() on Windows
        printf(SUCCESS_COLOR "Directory %s created successfully on system.\n" RESET_COLOR, path);
    } else {
        printf(ERROR_COLOR "Error creating directory on system" RESET_COLOR);
    }
}

// Function to delete contents of directory (subdirectories and files) and deallocate the space
void delete_directory(Directory *dir) {
    char path[MAX_NAME_LENGTH * 2] = "";
    generate_file_path(dir->parent, dir->name, path, sizeof(path));

    for (int i = 0; i < dir->num_files; i++) {
        char file_path[MAX_NAME_LENGTH * 3] = "";
        snprintf(file_path, sizeof(file_path), "%s/%s", path, dir->files[i]->name);

        free(dir->files[i]->data);
        free(dir->files[i]);

        if (remove(file_path) == 0) {
            printf(SUCCESS_COLOR "Deleted file %s from system.\n" RESET_COLOR, file_path);
        } else {
            printf(ERROR_COLOR "Error deleting file %s from system.\n" RESET_COLOR, file_path);
        }
    }

    for (int i = 0; i < dir->num_subdirs; i++) {
        delete_directory(dir->subdirs[i]);
    }

    if (rmdir(path) == 0) {
        printf(SUCCESS_COLOR "Deleted directory %s from system.\n" RESET_COLOR, path);
    } else {
        printf(ERROR_COLOR "Error deleting directory %s from system.\n" RESET_COLOR, path);
    }

    free(dir); 
}

// Function to delete a directory from current directory based on commands
void remove_directory(FileSystem *fs, const char *dirname) {
    Directory *current = fs->current_dir;
    for (int i = 0; i < current->num_subdirs; i++) {
        if (strcmp(current->subdirs[i]->name, dirname) == 0) {
            delete_directory(current->subdirs[i]);

            for (int j = i; j < current->num_subdirs - 1; j++) {
                current->subdirs[j] = current->subdirs[j + 1];
            }
            current->num_subdirs--;

            return;
        }
    }
    printf(ERROR_COLOR "Directory not found.\n" RESET_COLOR);
}

// Function to create a file
void create_file(FileSystem *fs, const char *filename) {
    Directory *current = fs->current_dir;
    if (current->num_files >= MAX_FILES) {
        printf(ERROR_COLOR "Directory is full!\n" RESET_COLOR);
        return;
    }
    for (int i = 0; i < current->num_files; i++) {
        if (strcmp(current->files[i]->name, filename) == 0) {
            printf(ERROR_COLOR "File already exists.\n" RESET_COLOR);
            return;
        }
    }
    File *new_file = (File *)malloc(sizeof(File));
    strncpy(new_file->name, filename, MAX_NAME_LENGTH);
    new_file->data = NULL;
    new_file->size = 0;
    current->files[current->num_files++] = new_file;

    // Generate the full path for the file
    char path[MAX_NAME_LENGTH * 2] = "";
    generate_file_path(current, filename, path, sizeof(path));

    // Write an empty file to the actual filesystem
    FILE *fp = fopen(path, "w");
    if (fp) {
        fclose(fp);
        printf(SUCCESS_COLOR "File %s created successfully on disk.\n" RESET_COLOR, filename);
    } else {
        printf(ERROR_COLOR "Error creating file on disk.\n" RESET_COLOR);
    }
}

// Function to delete a file
void delete_file(FileSystem *fs, const char *filename) {
    Directory *current = fs->current_dir;
    for (int i = 0; i < current->num_files; i++) {
        if (strcmp(current->files[i]->name, filename) == 0) {
            free(current->files[i]->data);
            free(current->files[i]);

            // Shift remaining files
            for (int j = i; j < current->num_files - 1; j++) {
                current->files[j] = current->files[j + 1];
            }
            current->num_files--;

            // Generate the full path for the file
            char path[MAX_NAME_LENGTH * 2] = "";
            generate_file_path(current, filename, path, sizeof(path));

            // Remove the file from the actual filesystem
            if (remove(path) == 0) {
                printf(SUCCESS_COLOR "File %s deleted successfully from system and memory.\n" RESET_COLOR, path);
            } else {
                printf(ERROR_COLOR "Error deleting file from system.\n" RESET_COLOR);
            }
            return;

        }
    }
    printf(ERROR_COLOR "File not found.\n" RESET_COLOR);
}


// Function to read a file
void read_file(FileSystem *fs, const char *filename) {
    Directory *current = fs->current_dir;
    for (int i = 0; i < current->num_files; i++) {
        if (strcmp(current->files[i]->name, filename) == 0) {
            // Read data from the actual file on disk

            char path[MAX_NAME_LENGTH * 2] = "";
            generate_file_path(current, filename, path, sizeof(path));

            FILE *fp = fopen(path, "r");
            if (fp) {
                printf("Reading file %s:\n", filename);
                char ch;
                while ((ch = fgetc(fp)) != EOF) {
                    putchar(ch);
                }
                fclose(fp);
                printf("\n");
            } else {
                printf(ERROR_COLOR "Error reading file on disk.\n" RESET_COLOR);
            }
            return;
        }
    }
    printf(ERROR_COLOR "File not found.\n" RESET_COLOR);
}


// Function to write data to a file
void write_file(FileSystem *fs, const char *filename, const char *data) {
    Directory *current = fs->current_dir;
    for (int i = 0; i < current->num_files; i++) {
        if (strcmp(current->files[i]->name, filename) == 0) {
            // Save data in memory
            current->files[i]->data = (char *)malloc(strlen(data) + 1);
            strcpy(current->files[i]->data, data);
            current->files[i]->size = strlen(data);

            char path[MAX_NAME_LENGTH * 2] = "";
            generate_file_path(current, filename, path, sizeof(path));

            // Save data to actual file on disk
            FILE *fp = fopen(path, "w");
            if (fp) {
                fprintf(fp, "%s", data);
                fclose(fp);
                printf(SUCCESS_COLOR "Data written to %s on disk.\n" RESET_COLOR, filename);
            } else {
                printf(ERROR_COLOR "Error writing data to file on disk.\n" RESET_COLOR);
            }
            return;
        }
    }
    printf(ERROR_COLOR "File not found.\n" RESET_COLOR);
}


// Function to change directory
void change_directory(FileSystem *fs, const char *dirname) {
    Directory *current = fs->current_dir;
    if (strcmp(dirname, "..") == 0 && current->parent != NULL) {
        fs->current_dir = current->parent;
        printf(SUCCESS_COLOR "Moved to parent directory.\n" RESET_COLOR);
    } else {
        for (int i = 0; i < current->num_subdirs; i++) {
            if (strcmp(current->subdirs[i]->name, dirname) == 0) {
                fs->current_dir = current->subdirs[i];
                printf(SUCCESS_COLOR "Moved to directory %s.\n" RESET_COLOR, dirname);
                return;
            }
        }
        printf(ERROR_COLOR "Directory not found.\n" RESET_COLOR);
    }
}

// Function to list directory contents
void list_directory(FileSystem *fs) {
    Directory *current = fs->current_dir;
    printf("Listing contents of %s:\n", current->name);
    for (int i = 0; i < current->num_subdirs; i++) {
        printf("  [DIR] %s\n", current->subdirs[i]->name);
    }
    for (int i = 0; i < current->num_files; i++) {
        printf("  %s\n", current->files[i]->name);
    }
}

// Function to print directory structure as a tree
void print_tree(Directory *dir, int depth) {
    for (int i = 0; i < depth; i++) {
        printf("  ");
    }
    printf("%s/\n", dir->name);

    for (int i = 0; i < dir->num_files; i++) {
        for (int j = 0; j < depth + 1; j++) {
            printf("  ");
        }
        printf("%s\n", dir->files[i]->name);
    }

    for (int i = 0; i < dir->num_subdirs; i++) {
        print_tree(dir->subdirs[i], depth + 1);
    }
}

// Function to print available commands
void print_help() {
    printf("Available Commands:\n");
    printf(SUCCESS_COLOR " - create <filename>       : Create a new file\n" RESET_COLOR);
    printf(SUCCESS_COLOR " - delete <filename>       : Delete an existing file\n" RESET_COLOR);
    printf(SUCCESS_COLOR " - read <filename>         : Read data from a file\n" RESET_COLOR);
    printf(SUCCESS_COLOR " - write <filename> <data> : Write data to a file\n" RESET_COLOR);
    printf(SUCCESS_COLOR " - mkdir <dirname>         : Create a new directory\n" RESET_COLOR);
    printf(SUCCESS_COLOR " - rmdir <dirname>         : remove an existing directory\n" RESET_COLOR);
    printf(SUCCESS_COLOR " - ls                      : List contents of the current directory\n" RESET_COLOR);
    printf(SUCCESS_COLOR " - cd <dirname>            : Change directory\n" RESET_COLOR);
    printf(SUCCESS_COLOR " - cd ..                   : Go to parent directory\n" RESET_COLOR);
    printf(SUCCESS_COLOR " - tree                    : Display directory structure\n" RESET_COLOR);
    printf(SUCCESS_COLOR " - exit                    : Exit the program\n" RESET_COLOR);
}

// Command-Line Interface (CLI)
void run_cli(FileSystem *fs) {
    char command[MAX_NAME_LENGTH];
    char arg1[MAX_NAME_LENGTH];
    char arg2[MAX_NAME_LENGTH];
    char current_path[MAX_NAME_LENGTH * 10];

    while (1) {
        build_path(fs->current_dir, current_path);
        printf(PROMPT_COLOR "%s> " RESET_COLOR, current_path);
        fgets(command, sizeof(command), stdin);
        command[strcspn(command, "\n")] = 0;

        if (sscanf(command, "create %s", arg1) == 1) {
            create_file(fs, arg1);
        } else if (sscanf(command, "delete %s", arg1) == 1) {
            delete_file(fs, arg1);
        } else if (sscanf(command, "read %s", arg1) == 1) {
            read_file(fs, arg1);
        } else if (sscanf(command, "write %s %[^\n]", arg1, arg2) == 2) {
            write_file(fs, arg1, arg2);
        } else if (sscanf(command, "mkdir %s", arg1) == 1) {
            create_directory(fs, arg1);
        } else if (sscanf(command, "rmdir %s", arg1) == 1) {
            remove_directory(fs, arg1);
        } else if (strcmp(command, "ls") == 0) {
            list_directory(fs);
        } else if (sscanf(command, "cd %s", arg1) == 1) {
            change_directory(fs, arg1);
        } else if (strcmp(command, "tree") == 0) {
            print_tree(fs->root, 0);
        } else if (strcmp(command, "help") == 0) {
            print_help();
        } else if (strcmp(command, "exit") == 0) {
            save_filesystem(fs);
            break;
        } else {
            printf(ERROR_COLOR "Unknown command.\n" RESET_COLOR);
            printf("Type 'help' for a list of commands.\n");
        }
    }
}

// Main function to initialize file system and start CLI
int main() {
    FileSystem *fs = (FileSystem *)malloc(sizeof(FileSystem));
    fs->root = (Directory *)malloc(sizeof(Directory));
    strncpy(fs->root->name, "/", MAX_NAME_LENGTH);
    fs->root->parent = NULL;
    fs->root->num_subdirs = 0;
    fs->root->num_files = 0;

    fs->current_dir = fs->root;
    load_filesystem(fs);

    run_cli(fs);

    return 0;
}