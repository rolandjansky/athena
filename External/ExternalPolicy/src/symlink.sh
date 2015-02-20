#! /usr/bin/env sh

#
#  Replace the -f option of ln -s when it does not work
#

ls -l $2 1>/dev/null 2>&1 

if test $? = 0 ; then
  rm -rf $2
fi

ln -s $1 $2

