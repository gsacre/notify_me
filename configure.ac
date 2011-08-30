AC_INIT([notifmed], [0.1], [gregory.sacre@gmail.com])
AC_CONFIG_AUX_DIR([build-aux])
AM_INIT_AUTOMAKE([-Wall -Werror])
AC_PROG_CC
AC_CONFIG_SRCDIR([src/notifmed.c])
PKG_CHECK_MODULES([DEPS],[libnotify glib-2.0 gdk-pixbuf-2.0])
AC_ARG_WITH([iniparser-path],
            [AS_HELP_STRING([--with-iniparser-path],
                            [location of the iniparser library path, defaults to /usr/include])],
            [INIPARSER_CFLAGS="-I$withval"],
            [INIPARSER_CFLAGS='-I/usr/include'])
AC_SUBST([INIPARSER_CFLAGS])

PKG_CHECK_MODULES([iniparser],[iniparser],[],
                  [AC_CHECK_LIB([iniparser],[iniparser_load])]
                  )

AC_CHECK_HEADERS([glib.h unistd.h sys/types.h sys.stat.h stdlib.h stdio.h fnctl.h errno.h syslog.h string.h getopt.h sys/socket.h netinet/in.h dirent.h])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_FILES([Makefile src/Makefile])
AC_OUTPUT
