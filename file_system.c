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