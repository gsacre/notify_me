#include <libnotify/notify.h>
#include <glib.h>
#include <unistd.h>
int main(int argc, char** argv)
{
        if(argc == 3)
        {
                NotifyNotification *n;
                notify_init("Test");
                n = notify_notification_new (argv[1],argv[2], NULL);
                notify_notification_set_timeout(n, 3000); //3 seconds
                if (!notify_notification_show (n, NULL)) {
                        g_error("Failed to send notification.\n");
                        return 1;
                }
                g_object_unref(G_OBJECT(n));
        }else{
                g_print("Too few arguments (%d), 2 needed.\n", argc-1);
        }
        return 0;
}
