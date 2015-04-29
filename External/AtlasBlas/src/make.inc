####################################################################
#  BLAS make include file.                                         #
#  March 2007                                                      #
####################################################################
#
SHELL = /bin/sh
#
#  The machine (platform) identifier to append to the library names
#
PLAT = _LINUX
#  
#  Modify the FORTRAN and OPTS definitions to refer to the
#  compiler and desired compiler options for your machine.  NOOPT
#  refers to the compiler options desired when NO OPTIMIZATION is
#  selected.  Define LOADER and LOADOPTS to refer to the loader and 
#  desired load options for your machine.
#
FORTRAN  = $(FC) -fPIC
OPTS     = $(FCFLAGS)
DRVOPTS  = $(OPTS)
NOOPT    = $(OPTS/-O?/-O0)
LOADER   = $(FORTRAN)
LOADOPTS = $(LDFLAGS)
#
#  The archiver and the flag(s) to use when building archive (library)
#  If you system has no ranlib, set RANLIB = echo.
#
ARCH     = $(FORTRAN)
ARCHFLAGS= -shared $(OPTS) -o
RANLIB   = echo
#
#  The location and name of the Reference BLAS library.
#
BLASLIB      = libblas.so.3.0.3
