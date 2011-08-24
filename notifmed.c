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

int main(void) {
        /* Our process ID and Session ID */
        pid_t pid, sid;

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


        /* Create a new SID for the child process */
        sid = setsid();
        if (sid < 0) {
                syslog (LOG_ERR, "Cannot create a new SID.");
                exit(EXIT_FAILURE);
        }



        /* Change the current working directory */
        if ((chdir("/")) < 0) {
                syslog (LOG_ERR, "Cannot create a new SID.");
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
                notify_notification_set_timeout(n, 5000); //3 seconds
                if (!notify_notification_show (n, NULL)) {
                        g_error("Failed to send notification.\n");
                        return 1;
                }
                g_object_unref(G_OBJECT(n));

                sleep(3); /* wait 3 seconds */
        }
        exit(EXIT_SUCCESS);
}
