#include "processes.h"

unsigned int getProcessID(char *p_processname) {
        DIR *dir_p;
        struct dirent *dir_entry_p;
        char dir_name[40];
        char target_name[252];
        int target_result;
        char exe_link[252];
        int result;
        char *process_trimmed;

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
                                        //printf("getProcessID(&#37;s) :Found. id = %d\n", p_processname, result);
                                        closedir(dir_p);
                                        return result;
                                }
                        }
                }
        }
        closedir(dir_p);
        return result;
}

