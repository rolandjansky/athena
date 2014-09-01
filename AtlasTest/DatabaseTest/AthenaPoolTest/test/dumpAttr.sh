#!/bin/sh -xv
#/** @file dumAttr.sh
# @Dump out attributes from NewEventCollection1 to output log and run post.sh
#
# @author RD Schaffer <R.D.Schaffer@cern.ch> - ATLAS Collaboration.
# $Id: dumpAttr.sh,v 1.1 2004-08-06 10:12:53 schaffer Exp $
# **/
test=$1
if [ -z "$status" ]
    then
    echo "[93;1m post.sh> Warning: athena exit status is not available [m"
else 
    # check exit status
    joblog=${test}.log
    if [ "$status" = 0 ]
	then 
        CollListAttrib.exe -src NewEventCollection1:RootCollection >> ${joblog}
	source ../test/post_check.sh ${test}
        status=$?
    fi
fi

exit $status

