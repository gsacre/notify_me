AC_INIT([notifmed], [0.1], [gregory.sacre@gmail.com])
AC_CONFIG_AUX_DIR([build-aux])
AM_INIT_AUTOMAKE([-Wall -Werror])
AC_PROG_CC
AC_CONFIG_SRCDIR([src/notifmed.c])
PKG_CHECK_MODULES([notify],[libnotify])
PKG_CHECK_MODULES([iniparser],[iniparser],[],
                  [AC_CHECK_LIB([iniparser],[iniparser_load])]
                  )
PKG_CHECK_MODULES([glib],[glib-2.0])
PKG_CHECK_MODULES([gdk_pixbuf],[gdk-pixbuf-2.0])

AC_CHECK_HEADERS([glib.h unistd.h sys/types.h sys.stat.h stdlib.h stdio.h fnctl.h errno.h syslog.h string.h getopt.h sys/socket.h netinet/in.h dirent.h])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_FILES([Makefile src/Makefile])
AC_OUTPUT
