#!/bin/sh
processIdent="$$"

##Arguments for script should be new file name and folder where individual files are stored. New file will be created in directory where originals are stored.

python haddMaker.py $1 $2 $processIdent

echo "Merging: Depending on the number of files and their sizes, this could take for-fracking-ever"

runner="susyAdd$$.C"

root -l $runner 

rm $runner
echo "Merged"

exit 0
