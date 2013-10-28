#!/bin/sh -xv
#/** @file post.sh
# @brief sh script that check the return code of an executable and compares 
# its output with a reference (if available).
# @param test_name 
#
# @author Paolo Calafiura <pcalafiura@lbl.gov> - ATLAS Collaboration.
# $Id: post_check.sh,v 1.4 2005-11-08 20:42:31 kkarr Exp $
# **/
test=$1
pattern=$2
#if [ -z "$status" ]
#    then
#    echo "[93;1m post.sh> Warning: athena exit status is not available [m"
#else 
    # check exit status
    joblog=${test}_test.log
    egrep -f ../test/${pattern}_pattern.txt ${test}.log > $joblog
#    cat joblog
#    if [ "$status" = 0 ]
#	then 
	echo "[92;1m post.sh> OK: ${test} exited normally. Output is in $joblog [m"
	reflog=../test/${test}_test.ref
	if [ -r $reflog ]
	    then
#	    echo " post.sh> Now comparing output with reference"
            diff -a -b -B $joblog $reflog
	    diffStatus=$?
	    if [ $diffStatus = 0 ]
		then
		echo "[92;1m post.sh> OK: $joblog and $reflog identical [m"
	    else
		echo "[97;101;1m post.sh> ERROR: $joblog and $reflog differ [m"
#		exit 1
	    fi
	else
	    tail $joblog
	    echo "[93;1m post.sh> WARNING: reference output $reflog not available [m"
	    echo  " post.sh> Please check ${PWD}/$joblog"
	fi
#    else
#	tail $joblog
#	echo  "[97;101;1m post.sh> ERROR: Athena exited abnormally! Exit code: $status [m"
#	echo  " post.sh> Please check ${PWD}/$joblog"
#    fi
#fi

# Check output for ERROR/FATAL
joblog=${test}.log
echo 

exit $status

