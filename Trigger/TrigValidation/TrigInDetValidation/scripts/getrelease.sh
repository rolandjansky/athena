#!/bin/bash

[ $# -lt 1 ] && exit -1

RELEASE=$1

[ -z "$AtlasSetup" ] && exit -1

SUB=$(echo $RELEASE | sed 's|.*\.||g')
STUB=$(echo $RELEASE | sed 's|\.[^\.]*$||g')

((SUB--))

RELEASE=$STUB.$SUB

while [ $SUB -ge 0 ]; do 

    ( source $AtlasSetup/scripts/asetup.sh Athena,$RELEASE --simulate &> /dev/null ) && echo $RELEASE && exit 0

    ((SUB--))
  
    RELEASE=$STUB.$SUB

done

exit -1
