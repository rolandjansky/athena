#!/bin/sh
#+
#	Script to setup gfortran base directory
#
#	Author:-
#	David Quarrie
#-

s_gcc=`which gcc`
if [ -L $s_gcc ]; then
	s_gcc=`readlink -f $s_gcc`
fi
s_bin=`dirname $s_gcc`
s_base=`dirname $s_bin`
echo $s_base
s_gcc=
s_bin=
s_base=

