#!/bin/bash
#
# $Id: mergeSelections.sh 711151 2015-11-27 12:00:55Z krasznaa $
#
# Script used to merge selection.xml files during the build procedure.
#
# Usage: mergeFiles <output> <input1> [input2...]
#

# Propagate errors:
set -e

# Get the output file name:
ofile=$1
shift

# Start the output file with a header:
echo "<lcgdict>" > $ofile

# Merge the specified XML files into it, excluding their possible
# headers and footers:
while [[ $# > 0 ]]
do
    grep -v -e "<lcgdict>" -e "</lcgdict>" $1 >> $ofile
    shift
done

# Put a footer at the end of the file:
echo "</lcgdict>" >> $ofile
