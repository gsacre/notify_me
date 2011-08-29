AC_INIT([notifmed], [0.1], [gregory.sacre@gmail.com])
AC_CONFIG_AUX_DIR([build-aux])
AM_INIT_AUTOMAKE([-Wall -Werror])
AC_PROG_CC
AC_CONFIG_SRCDIR([src/notifmed.c])
AC_CHECK_LIB([notify],[notify_notification_new],[NOTIFY=-lnotify])
AC_SUBST([NOTIFY])
AC_CHECK_LIB([iniparser],[iniparser_load],[INIPARSER=-liniparser])
AC_SUBST([INIPARSER])
AC_CHECK_LIB([glib-2.0],[main],[GLIB=-lglib-2.0])
AC_SUBST([GLIB])
AC_CHECK_LIB([gdk_pixbuf-2.0],[gdk_pixbuf_version],[PIXBUF=-lgdk_pixbuf-2.0])
AC_SUBST([PIXBUF])

AC_CHECK_HEADERS([glib.h unistd.h sys/types.h sys.stat.h stdlib.h stdio.h fnctl.h errno.h syslog.h string.h getopt.h sys/socket.h netinet/in.h dirent.h])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_FILES([Makefile src/Makefile])
AC_OUTPUT
