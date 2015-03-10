#!/bin/bash
#
# Check for differences in the trigger setup and menu
#

if [ $# -ne 1 ]; then
   echo "Syntax: `basename $0` <reference directory>"
   echo
   exit 1
fi

refdir=$1
l2setup="l2__setup.txt"
efsetup="ef__setup.txt"
l2menu="l2__outputHLTconfig.xml"
efmenu="ef__outputHLTconfig.xml"

if [[ ! -d ${refdir} ]]; then
    echo "Directory ${refdir} does not exist"
    exit 1
fi

# Test whether L2 or EF was run
if [ -e ./${l2setup} ]; then
    setup=${l2setup}
    menu=${l2menu}
elif [ -e ./${efsetup} ]; then
    setup=${efsetup}
    menu=${efmenu}
else
    echo "Cannot find L2/EF setup files"
    exit 1
fi

echo "======================================================="
echo " Diffing setup ${setup}"
echo " Reference: ${refdir}/${setup}"
echo "======================================================="


# Ignore lines matching this regexp
ignore='^ .*|^Type.*|^Name.*|^Version.*|^Parent.*|^Properties.*|^No subalgorithms.*|.*DF_Pid.*|^-+$|^>>.*|^\].*|^\s*$|.*ExtraTagValuePairs.*rel_[0-6].*'
egrep -v "${ignore}" ${setup} > tmp1
egrep -v "${ignore}" ${refdir}/${setup} > tmp2
diff tmp1 tmp2
diff1=$?
if [ $diff1 -eq 0 ]; then
    echo "No differences found"
fi
rm -f tmp1 tmp2

echo "======================================================="
echo " Diffing menu outputHLTconfig.xml"
echo " Reference: ${refdir}/${menu}"
echo "======================================================="
diff "outputHLTconfig.xml" ${refdir}/${menu}
diff2=$?
if [ $diff2 -eq 0 ]; then
    echo "No differences found"
fi

if [ $diff1 -eq 0 -a $diff2 -eq 0 ]; then
    exit 0
else
    exit 1
fi

#
