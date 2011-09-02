#include <stdlib.h>
#include <unistd.h>
#include "processes.h"
#include "strings.h"

unsigned int* getProcessID(char *p_processname) {
        DIR *dir_p;
        struct dirent *dir_entry_p;
        char dir_name[40];
        char target_name[252];
        int target_result;
        char exe_link[252];
        int result;
        char *process_trimmed;
        int *list_of_pids = (int*) malloc(255 * sizeof(int));
        int index = 0;

        process_trimmed = trim(p_processname);

        result=-1;

        // Open /proc/ directory
        dir_p = opendir("/proc/");

        // Reading /proc/ entries
        while(NULL != (dir_entry_p = readdir(dir_p))) {
                // Checking for numbered directories
                if (strspn(dir_entry_p->d_name, "0123456789") == strlen(dir_entry_p->d_name)) {
                        strcpy(dir_name, "/proc/");
                        strcat(dir_name, dir_entry_p->d_name);
                        // Obtaining the full-path eg: /proc/24657/
                        strcat(dir_name, "/");
                        exe_link[0] = 0;
                        strcat(exe_link, dir_name);
                        // Getting the full-path of that exe link
                        strcat(exe_link, "exe");
                        // Getting the target of the exe ie to which binary it points to
                        target_result = readlink(exe_link, target_name, sizeof(target_name)-1);
                        if (target_result > 0) {
                                target_name[target_result] = 0;
                                // Searching for process name in the target name -- ??? could be a better search !!!
                                if (strstr(target_name, process_trimmed) != NULL) {
                                        result = atoi(dir_entry_p->d_name);
                                        list_of_pids[index] = result;
                                        index++;
                                }
                        }
                }
        }
        closedir(dir_p);
        return &(list_of_pids[0]);
}
