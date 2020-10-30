#!/bin/sh
#
# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
#
# Thin wrapper to execute configurations with the CA. 
# Might be called from athena.py
#


#otherargs is set if we are sourced from inside athena.py
#if not yet set, resort of reguar CL arguments 
if [ -z ${otherargs+x} ]
    then
    otherargs=${@}
fi


#Separate the top-level python script (ending with .py) from 
#arguments passed to that script 
scriptargs=""
for a in $otherargs
do
    case "$a" in
	*.py) topscriptfile=$a;;
	*) scriptargs="$scriptargs $a";;
    esac
done

#Check if we got a top-level script 
#Improve: Check if there is exactly one!
if [ -z "$topscriptfile" ]
    then
    echo "ERROR: No top-level python script given"
    exit 1
fi

#Assemble the search path for top-level jobOption files.
#Search the local directoy, a possible WorkDir and the installed Athena
#Note: We could also search $PYTHONPATH but that's a much longer list
topScriptSearchPath="."

if [ ! -z "${WorkDir_DIR}" ]
 then
    topScriptSearchPath="$topScriptSearchPath $WorkDir_DIR_DIR/python" 
fi

if [ ! -z "${Athena_DIR}" ]
 then
    topScriptSearchPath="$topScriptSearchPath $Athena_DIR/python" 
fi

#Now search for the top-level script:
for pp in $topScriptSearchPath
do
    if [ -f "$pp/$topscriptfile" ]
	then
	topscript="$pp/$topscriptfile"
	break
    fi
done


if [ -z "$topscript" ];
    then
    echo "Could not find python script $topscriptfile"
    exit 1
fi

#Finally: Execute it! 
python $topscript $scriptargs
status=$?
if [ ! $status -eq 0 ]; then
    echo "leaving with code $status: \"failure\""
    exit $status
else
    echo "leaving with code 0: \"successful run\""
fi
