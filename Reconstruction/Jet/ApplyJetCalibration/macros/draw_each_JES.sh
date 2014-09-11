#!/bin/sh
####
#
#   Simple macro that draws the JES and offset correction of the available
#   jet calibrations.
#
#   Dag Gillberg, Dec 3, 2011
# 
####

exec=draw_each_JES.exe
code=DrawJES.C

flagsNlibs="$($ROOTSYS/bin/root-config --cflags --glibs) -lTreePlayer"
# -lXMLParser"

echo 
echo "Will now compile $code" ; echo 
echo "Compilation flags:"
echo "  $flagsNlibs" 
echo

rm -f $exec
g++ $flagsNlibs -I../ -o $exec $code && {

    echo "Compilation successful!" ; echo
    echo "Will now loop over the available jet calibrations and make plots" ; echo

    JESs=$(ls ../data/CalibrationConfigs/*config)
    for JES in $JESs ; do
	./$exec $JES
    done

} || {
    echo "Compilation failed :(" ; echo
}
