#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_NAME_LENGTH 100
#define MAX_FILES 100
#define MAX_DIRS 100

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

// 

int main() {

    return 0;
}
