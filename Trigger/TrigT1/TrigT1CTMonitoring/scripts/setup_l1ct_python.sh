#!/bin/bash

# source this file

# Required for IS python bindings only,
# echo setup setup tdaq 03
# . /sw/tdaq/setup/setup_tdaq-02-00-03.sh

echo setting up offline
if [ -d /sw/atlas/cmtsite/ ] ; then
    echo 'using point 1 setup script'
    source /sw/atlas/cmtsite/setup.sh -tag=15.5.1,setup
else
    source ~/cmthome/setup.sh -tag=15.5.1,setup
fi

echo setting up python includes 
export PYTHONPATH=$PYTHONPATH:`pwd`
export PATH=$PATH:`pwd`

echo setup of aliases, must contain pointers to the used databases
export CORAL_DBLOOKUP_PATH=/atlas/db/.adm/coral/:"$CORAL_DBLOOKUP_PATH"
export CORAL_AUTH_PATH=/atlas/db/.adm/coral/:"$CORAL_DBLOOKUP_PATH"
    
echo "CORAL_DBLOOKUP_PATH = $CORAL_DBLOOKUP_PATH "
echo "CORAL_AUTH_PATH     = $CORAL_AUTH_PATH     "

