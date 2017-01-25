#!/bin/bash

MACRO="$ROOTCOREBIN/data/EventLoopGrid/loadrungridjob.C"

if [ ! $# -eq 1 ] ; then
    echo "$0 : Error: This script expects exactly one argument."
    exit 1
fi

if [ ! -f "$MACRO" ] ; then
    echo "$0 : Error: $MACRO does not exist."
    exit 1    
fi

command -v root > /dev/null 2>&1 || { 
    echo >&2 "$0 : Error: Root not set up." 
    exit 1 
}

unset ROOT_TTREECACHE_SIZE

echo Executing root -l -b -q $ROOTCOREDIR/scripts/load_packages.C $MACRO\(\"$1\"\) 
date

root -l -b -q $ROOTCOREDIR/scripts/load_packages.C $MACRO\(\"$1\"\)
exitcode=$?

echo Finished executing root
date

exit $exitcode
