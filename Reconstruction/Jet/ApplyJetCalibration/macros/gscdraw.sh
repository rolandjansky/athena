#!/bin/sh
####
#
#   Simple macro that tests the GSC implementation
#   jet calibrations.
#
#   Joe Taenzer, November 1st 2012
# 
####

exec=_gscdraw.exe
code=DrawGSC.C

flagsNlibs="$($ROOTSYS/bin/root-config --cflags --glibs) -lTreePlayer"
# -lXMLParser"

echo 
echo "Will now compile $code" ; echo 
echo "Compilation flags:"
echo "  $flagsNlibs" 
echo

rm -f $exec
g++ -I../ $flagsNlibs -o $exec $code && {

    echo "Compilation successful!" ; echo
    GSCs=$(ls ../data/CalibrationFactors/GSC*)

    for GSC in $GSCs ; do
        ./$exec $GSC
    done

} || {
    echo "Compilation failed :(" ; echo
}
