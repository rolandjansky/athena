#!/bin/sh -xv
#/** @file post.sh
# @brief sh script that check the return code of an executable and compares
# its output with a reference (if available).
# @param test_name
#
# @author Paolo Calafiura <pcalafiura@lbl.gov> - ATLAS Collaboration.
# $Id: post_check.sh,v 1.18 2008-12-12 16:02:16 gemmeren Exp $
# **/
status=${?}
test=${1}
file=${2}
if [ -z "${status}" ]
    then
    echo "[93;1m post.sh> Warning: athena exit status is not available [m"
else
    # check exit status
    joblog=${test}.log
    if [ "${status}" = 0 ]
	then
	echo "[92;1m post.sh> OK: ${test} exited normally. Output is in ${joblog} [m"
	reflog=../test/${test}.ref
	if [ -r ${reflog} ]
	    then
#	    echo " post.sh> Now comparing output with reference"
	    diff -a -b -B  ${joblog} ${reflog} |\
                # ignore diff annotations
	        egrep -a -v '^---|^[[:digit:]]+[acd,][[:digit:]]+' |\
                # ignore package names e.g. Package-00-00-00
		egrep -a -v '\w+-[[:digit:]]{2}-[[:digit:]]{2}-[[:digit:]]{2}' |\
		# for now ignore IProxyDict dictionary warning
		grep -a '^AthenaEventLoopMgr'

	    diffStatus=${?}
	    if [ ${diffStatus} = 0 ]
		then
		echo "[97;101;1m post.sh> WARNING: ${joblog} and ${reflog} differ [m"
#		exit 1
	    else
		echo "[92;1m post.sh> OK: ${joblog} and ${reflog} identical [m"
	    fi
	else
	    tail ${joblog}
	    echo "[93;1m post.sh> WARNING: reference output ${reflog} not available [m"
	    echo  " post.sh> Please check ${PWD}/${joblog}"
	fi
        checkFile.py ${file} | grep "(T)" | sed -e 's/ \+[0-9]\+\.[0-9]\+ \(kb\)*//2g' |sed -e 's/ \+\([0-9]\+\.[0-9]\+ kb\) \+\([0-9]\+\) \+(T) \(.*\)$/\3 \2/' | sort > ${test}.size.log
        if [ -r ../test/${test}.size.ref ]
	    then
            diff -a -b -B  ${test}.size.log ../test/${test}.size.ref |\
                # ignore diff annotations
                egrep -a -v '^---|^[[:digit:]]+[acd,][[:digit:]]+'

	    diffStatus=${?}
	    if [ ${diffStatus} = 0 ]
		then
		echo "[97;101;1m post.sh> WARNING: ${test}.size.log and ../test/${test}.size.ref differ [m"
#		exit 1
	    else
		echo "[92;1m post.sh> OK: ${test}.size.log and ../test/${test}.size.ref identical [m"
	    fi
	fi
    else
	tail ${joblog}
	echo  "[97;101;1m post.sh> ERROR: Athena exited abnormally! Exit code: ${status} [m"
	echo  " post.sh> Please check ${PWD}/${joblog}"
    fi
fi

# Check output for ERROR/FATAL
joblog=${test}.log
echo

exit 0
exit ${status}
