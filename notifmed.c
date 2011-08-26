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

int file_exists (char * fileName) {
        struct stat buf;
        int i = stat ( fileName, &buf );
        /* File found */
        if ( i == 0 ) {
                return 1;
        }
        return 0;
}

int main(int argc, char **argv) {
        int port = 5586,
            notification_timeout = 5;

        int config_preceeds = 1;
        int c;

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

                sleep(3); /* wait 3 seconds */
        }
        closelog();
        exit(EXIT_SUCCESS);
}
