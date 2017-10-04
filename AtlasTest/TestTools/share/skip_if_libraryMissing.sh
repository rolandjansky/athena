#!/bin/sh
# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration.
SCRIPTNAME=$(basename $0)
libroot=$1
shift
echo "$SCRIPTNAME:  command provided: \"$@\""

for p in `echo $LD_LIBRARY_PATH | tr : ' ' `; do
  if [ -r $p/lib${libroot}.so ]; then
    echo "$SCRIPTNAME:  Library $libroot is present; will execute provided command."
    $@
    # take return value from executed command
    exit $?
  fi
done


echo "$SCRIPTNAME:  Library $libroot is not present , *not* executing provided command."

