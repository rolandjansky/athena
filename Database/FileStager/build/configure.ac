AC_INIT(Makefile.am)

AC_CONFIG_MACRO_DIR([m4])

AC_CANONICAL_HOST
AC_CANONICAL_TARGET

AM_INIT_AUTOMAKE(FileStager,0.1.0)

AC_PROG_CC
AC_PROG_CXX
AC_LANG(C++)
AC_PROG_LIBTOOL
AM_PROG_LIBTOOL
AC_PROG_INSTALL

# Check for ROOT
#
AC_CHECK_PROG(ROOT_BIN,[root-config],[yes])
if test -z "$ROOT_BIN" ; then
  AC_MSG_ERROR([root-config not found in PATH])
fi
if root-config --dummy-nonexistent-param >/dev/null 2>&1; then
  AC_MSG_ERROR([root-config script does not return error on bad parameters. Please update root.])
fi
if ! root-config --cflags >/dev/null 2>&1; then
  AC_MSG_ERROR([root-config script does not parse --cflags correctly])
fi
if ! root-config --ldflags >/dev/null 2>&1; then
  AC_MSG_ERROR([root-config script does not parse --ldflags correctly])
fi
if ! root-config --libs >/dev/null 2>&1; then
  AC_MSG_ERROR([root-config script does not parse --libs correctly])
fi	

ROOT_CFLAGS="`root-config --cflags`"
ROOT_LDFLAGS="`root-config --ldflags`"
ROOT_LIBS="`root-config --libs`"

AC_SUBST(ROOT_CFLAGS)
AC_SUBST(ROOT_LDFLAGS)
AC_SUBST(ROOT_LIBS)

AC_CONFIG_FILES([Makefile])

AC_OUTPUT
