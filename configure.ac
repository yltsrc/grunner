AC_INIT([grunner],0.1.1)
AC_CONFIG_HEADERS(config.h)
AM_INIT_AUTOMAKE
AM_SILENT_RULES([yes])
AC_CONFIG_FILES([Makefile])
AC_PROG_CC

CFLAGS="$CFLAGS -ggdb3 -Wall -rdynamic -O0"

PKG_CHECK_MODULES([glib2], [glib-2.0 >= 2.14])
AC_OUTPUT

