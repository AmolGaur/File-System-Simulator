#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
    char *data;       // file data
    int size;         // size of the data
} File;

// Directory structure
typedef struct Directory {
    char name[MAX_NAME_LENGTH];
    struct Directory *parent; // parent directory
    struct Directory *subdirs[MAX_DIRS]; // subdirectories
    File *files[MAX_FILES];    // files in the directory
    int num_subdirs;
    int num_files;
} Directory;

// FileSystem structure
typedef struct {
    Directory *root;         // root directory
    Directory *current_dir;  // current directory the user is in
} FileSystem;

// Function Headers
void save_filesystem(FileSystem *fs);
void load_filesystem(FileSystem *fs);
void create_directory(FileSystem *fs, const char *dirname);
void create_file(FileSystem *fs, const char *filename);
void write_file(FileSystem *fs, const char *filename, const char *data);
void print_tree(Directory *dir, int depth);
void delete_file(FileSystem *fs, const char *filename);
void read_file(FileSystem *fs, const char *filename);
void change_directory(FileSystem *fs, const char *dirname);
void list_directory(FileSystem *fs);
void print_help();

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
    printf(SUCCESS_COLOR "File %s created successfully.\n" RESET_COLOR, filename);
}

// Function to delete a file
void delete_file(FileSystem *fs, const char *filename) {
    Directory *current = fs->current_dir;
    for (int i = 0; i < current->num_files; i++) {
        if (strcmp(current->files[i]->name, filename) == 0) {
            free(current->files[i]->data);
            free(current->files[i]);
            for (int j = i; j < current->num_files - 1; j++) {
                current->files[j] = current->files[j + 1];
            }
            current->num_files--;
            printf(SUCCESS_COLOR "File %s deleted successfully.\n" RESET_COLOR, filename);
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
            if (current->files[i]->data != NULL) {
                printf("Reading file %s:\n%s\n", filename, current->files[i]->data);
            } else {
                printf("File is empty.\n");
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
            current->files[i]->data = (char *)malloc(strlen(data) + 1);
            strcpy(current->files[i]->data, data);
            current->files[i]->size = strlen(data);
            printf(SUCCESS_COLOR "Data written to %s.\n" RESET_COLOR, filename);
            return;
        }
    }
    printf(ERROR_COLOR "File not found.\n" RESET_COLOR);
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