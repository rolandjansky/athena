#!/bin/bash

if [ ! $# -eq 1 ] ; then
    echo "$0 : Error: This script expects exactly one argument."
    exit 1
fi

command -v root > /dev/null 2>&1 || { 
    echo >&2 "$0 : Error: Root not set up." 
    exit 1 
}

unset ROOT_TTREECACHE_SIZE

echo Executing eventloop_run_grid_job \"$1\"
date

eventloop_run_grid_job $1
exitcode=$?

echo Finished executing root
date

exit $exitcode
