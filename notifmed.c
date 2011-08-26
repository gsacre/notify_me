#include <libnotify/notify.h>
#include <glib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <syslog.h>
#include <string.h>
#include <iniparser.h>
#include <getopt.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <dirent.h>

int file_exists (char * fileName) {
        struct stat buf;
        int i = stat ( fileName, &buf );
        /* File found */
        if ( i == 0 ) {
                return 1;
        }
        return 0;
}

unsigned int getProcessID(char *p_processname) {
        DIR *dir_p;
        struct dirent *dir_entry_p;
        char dir_name[40];
        char target_name[252];
        int target_result;
        char exe_link[252];
        int errorcount;
        int result;

        errorcount=0;
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
                                if (strstr(target_name, p_processname) != NULL) {
                                        result = atoi(dir_entry_p->d_name);
                                        printf("getProcessID(&#37;s) :Found. id = %d\n", p_processname, result);
                                        closedir(dir_p);
                                        return result;
                                }
                        }
                }
        }
        closedir(dir_p);
        return result;
}


int main(int argc, char **argv) {
        int port = 5586,
            notification_timeout = 5,
            config_preceeds = 1,
            sockfd,
            newsockfd,
            c;

        socklen_t clilen;
        char buffer[256];
        struct sockaddr_in serv_addr, cli_addr;

        while (1) {
                static struct option long_options[] =
                {
                        /* These options don't set a flag.
                         * We distinguish them by their indices. */
                        {"port",    required_argument, 0, 'p'},
                        {"timeout", required_argument, 0, 't'},
                        {"help",    no_argument,       0, 'h'},
                        {0, 0, 0, 0}
                };
                /* getopt_long stores the option index here. */
                int option_index = 0;

                c = getopt_long (argc, argv, "hp:t:",
                                long_options, &option_index);

                /* Detect the end of the options. */
                if (c == -1)
                        break;

                switch (c)
                {
                        case 0:
                                /* If this option set a flag, do nothing else now. */
                                if (long_options[option_index].flag != 0)
                                        break;
                                printf ("option %s", long_options[option_index].name);
                                if (optarg)
                                        printf (" with arg %s", optarg);
                                printf ("\n");
                                break;

                        case 'p':
                                port = atoi(optarg);
                                config_preceeds = 0;
                                break;

                        case 't':
                                notification_timeout = atoi(optarg);
                                config_preceeds = 0;
                                break;

                        case 'h':
                                printf ("Usage:\n");
                                printf ("%s [-h] [-p port_number] [-t notification_timeout]\n", argv[0]);
                                exit(EXIT_SUCCESS);

                        case '?':
                                /* getopt_long already printed an error message. */
                                break;

                        default:
                                printf("Unkown parameter. Exiting.\n");
                                exit(EXIT_FAILURE);
                }
        }

        /* Our process ID and Session ID */
        pid_t pid, sid;

        dictionary *dict;

        /* Fork off the parent process */
        pid = fork();
        if (pid < 0) {
                printf("Cannot fork the daemon. Exiting...\n");
                exit(EXIT_FAILURE);
        }
        /* If we got a good PID, then
         *            we can exit the parent process. */
        if (pid > 0) {
                exit(EXIT_SUCCESS);
        }

        /* Change the file mode mask */
        umask(0);

        openlog ("notifmed", LOG_CONS | LOG_PID | LOG_NDELAY, LOG_LOCAL0);
        syslog (LOG_INFO, "Program started by user %d", getuid ());

        if (config_preceeds) {
                char * home = getenv("HOME");
                char * home_path_config = malloc(snprintf(NULL, 0, "%s/%s", home, ".notifmedrc") + 1);
                sprintf(home_path_config, "%s/%s", home, ".notifmedrc");

                if (file_exists(home_path_config)) {
                        dict = iniparser_load(home_path_config);
                } else if (file_exists("/etc/notifmed.rc")) {
                        dict = iniparser_load("/etc/notifmed.rc");
                        /* a config file could also be given as argument
                           } else if (file_exists()) {
                           dict = iniparser_load();*/
                } else {
                        syslog (LOG_INFO, "No configuration file found.");
                        syslog (LOG_INFO, "Using defaults:");
                        syslog (LOG_INFO, "    port = 5586 ; notification_timeout = 5");
                }

                if (!dict) {
                        syslog (LOG_ERR, "Dictionary configuration file problem.");
                        closelog();
                        exit(EXIT_FAILURE);
                }

                int i;
                unsigned int hh=dictionary_hash("server");
                for ( i=0 ; (i<dict->n) && (hh!=dict->hash[i]) ; i++);
                // No "server" section found
                if( i == dict->n ) {
                        syslog (LOG_INFO, "No server section found.");
                        syslog (LOG_INFO, "Using defaults:");
                        syslog (LOG_INFO, "    port = 5586 ; notification_timeout = 5");
                }

                for ( i++ ; ( i < dict->n ) && strncmp(dict->key[i],"server:",6) == 0 ; i++ ) {
                        if (strcmp(dict->key[i],"server:port") == 0) {
                                port = atoi(dict->val[i]);
                        } else if (strcmp(dict->key[i],"server:notification_timeout") == 0) {
                                notification_timeout = atoi(dict->val[i]);
                        }
                }
        }
        syslog (LOG_INFO, "Config found: port=%i - notification_timeout=%i", port, notification_timeout);

        /* Create a new SID for the child process */
        sid = setsid();
        if (sid < 0) {
                syslog (LOG_ERR, "Cannot create a new SID.");
                closelog();
                exit(EXIT_FAILURE);
        }

        // Preparing the network part
        sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if (sockfd < 0) {
                syslog (LOG_ERR, "Cannot open the socket.");
                closelog();
                exit(EXIT_FAILURE);
        }

        bzero((char *) &serv_addr, sizeof(serv_addr));
        serv_addr.sin_family = AF_INET;
        serv_addr.sin_addr.s_addr = INADDR_ANY;
        serv_addr.sin_port = htons(port);

        if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
                syslog (LOG_ERR, "Cannot bind the socket.");
                closelog();
                exit(EXIT_FAILURE);
        }

        listen(sockfd,5);
        clilen = sizeof(cli_addr);

        /* Change the current working directory */
        if ((chdir("/")) < 0) {
                syslog (LOG_ERR, "Cannot create a new SID.");
                closelog();
                exit(EXIT_FAILURE);
        }

        /* Close out the standard file descriptors */
        close(STDIN_FILENO);
        close(STDOUT_FILENO);
        close(STDERR_FILENO);

        /* Daemon-specific initialization goes here */

        /* The Big Loop */
        while (1) {
                NotifyNotification *n;
                notify_init("Test");
                n = notify_notification_new ("Title","Body", NULL);
                notify_notification_set_timeout(n, notification_timeout * 1000); //3 seconds
                if (!notify_notification_show (n, NULL)) {
                        g_error("Failed to send notification.\n");
                        return 1;
                }
                g_object_unref(G_OBJECT(n));

                newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
                if (newsockfd < 0) {
                        syslog (LOG_ERR, "Cannot accept messages.");
                        closelog();
                        close(newsockfd);
                        close(sockfd);
                        exit(EXIT_FAILURE);
                }

                bzero(buffer,256);

                n = read(newsockfd,buffer,255);
                if (n < 0) {
                        syslog (LOG_ERR, "Error reading from the socket.");
                        closelog();
                        close(newsockfd);
                        close(sockfd);
                        exit(EXIT_FAILURE);
                }

                syslog (LOG_INFO, "Here is the message: %s\n",buffer);
                n = write(newsockfd,"I got your message",18);

                if (n < 0) {
                        syslog (LOG_ERR, "Error writing to the socket.");
                        closelog();
                        close(newsockfd);
                        close(sockfd);
                        exit(EXIT_FAILURE);
                }

                //sleep(3); /* wait 3 seconds */
        }

        close(newsockfd);
        close(sockfd);
        closelog();
        exit(EXIT_SUCCESS);
}
