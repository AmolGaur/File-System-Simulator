# File System Simulator  

This **File System Simulator** is a C-based project that mimics a basic file system's operations, supporting a range of commands to manage directories and files. Below are the supported commands and their practical applications.  

---

## **Supported Commands and Their Applications**  

### **Directory Commands**  
| Command                  | Description                          | Example Usage                         |  
|--------------------------|--------------------------------------|---------------------------------------|  
| `mkdir <directory_name>` | Creates a new directory.             | `mkdir Documents`                     |  
| `rmdir <directory_name>` | Removes an empty directory.          | `rmdir Documents`                     |  
| `cd <directory_name>`    | Changes the current directory.       | `cd Documents`                        |  
| `cd ..`                  | Moves to the parent directory.       | `cd ..`                               |  
| `ls`                     | Lists all files and directories.     | `ls`                                  |  

### **File Commands**  
| Command                     | Description                          | Example Usage                         |  
|-----------------------------|--------------------------------------|---------------------------------------|  
| `create <file_name>`        | Creates a new file.                  | `create report.txt`                   |  
| `write <file_name>`         | Writes data into a file.             | `write report.txt`                    |  
| `read <file_name>`          | Reads and displays file content.     | `read report.txt`                     |  
| `delete <file_name>`        | Deletes a specified file.            | `delete report.txt`                   |  

### **System Commands**  
| Command               | Description                          | Example Usage                         |  
|-----------------------|--------------------------------------|---------------------------------------|  
| `save`                | Saves the current file system state. | `save`                                |  
| `load`                | Loads the saved file system state.   | `load`                                |  
| `exit`                | Exits the simulator.                 | `exit`                                |  

---

## **Applications**  
1. **Simulating File System Operations**: Test and understand file system behavior without accessing actual hardware.  
2. **Educational Tool**: Ideal for learning file system concepts in a safe environment.  
3. **Prototyping and Debugging**: Prototype file management systems and debug logic.  

---

## **How to Run**  
1. **Compile the Program**:  
   ```bash  
   gcc -o filesystem_simulator filesystem_simulator.c  
