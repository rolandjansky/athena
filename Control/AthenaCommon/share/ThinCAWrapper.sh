#!/bin/sh
#
# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
#
# Demonstator for a thin wrapper to execute configurations with the CA
#

topScriptSearchPath="."

if [ ! -z "${WorkDir_DIR}" ]
 then
    topScriptSearchPath="$topScriptSearchPath $WorkDir_DIR_DIR/python" 
fi

if [ ! -z "${Athena_DIR}" ]
 then
    topScriptSearchPath="$topScriptSearchPath $Athena_DIR/python" 
fi

topscriptfile=`echo $otherargs | cut -d " " -f 1`
scriptargs=`echo $otherargs | cut -d " " -f 2-`


for pp in $topScriptSearchPath
do
    if [ -f "$pp/$topscriptfile" ]
	then
	topscript="$pp/$topscriptfile"
	break
    fi
done


if [[ -z $topscript  ]];
    then
    echo "Could not find python script $topscriptfile"
    exit 1
fi

python $topscript $scriptargs



