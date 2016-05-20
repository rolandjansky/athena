#!/bin/bash
#
# $Id: mergeFiles.sh 711151 2015-11-27 12:00:55Z krasznaa $
#
# Script used to merge text files during the build procedure.
#
# Usage: mergeFiles <output> <input1> [input2...]
#

# Propagate errors:
set -e

# Get the output file name:
ofile=$1
shift

# Loop over the files that need to be merged:
firstFile=1
for ifile in $*; do
    # Only consider files that really exist:
    if [ -f ${ifile} ]; then
        # Overwrite, or append the file:
        if [ ${firstFile} = 1 ]; then
            cat ${ifile} > ${ofile}
        else
            cat ${ifile} >> ${ofile}
        fi
        firstFile=0
    fi
done
