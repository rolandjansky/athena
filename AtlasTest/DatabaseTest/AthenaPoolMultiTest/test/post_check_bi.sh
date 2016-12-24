#!/bin/sh -xv
#/** @file post_check_bi.sh
# @brief sh script that check the return code of an executable and compares 
# its output with a reference (if available). Modified to restrict checks 
# to output from AthenaPoolMultiTest BuildInput test.
# @param test_name 
#
# @author Jack Cranshaw (Jack.Cranshaw@cern.ch), Paolo Calafiura.
# $Id: post_check_bi.sh,v 1.5 2009-02-13 16:29:40 cranshaw Exp $
# **/
test=$1
status=${?}
if [ -z "$status" ]
    then
    echo "[93;1m post_check_bi.sh> Warning: athena exit status is not available [m"
else 
    # check exit status
    joblog=${test}.log
    if [ "$status" = 0 ]
	then 
	echo "[92;1m post_check_bi.sh> OK: ${test} exited normally. Output is in $joblog [m"
	reflog=../share/${test}.ref
        grep -e 'WriteData' \
             -e 'AddTrigMap' \
             -e 'Stream1' \
             -e 'FullColl' \
             -e 'NullColl' \
             -e 'ERROR' \
             -e 'Error' \
             ${joblog} > ${joblog}.middle
        grep -v 'ExampleHit' ${joblog}.middle > ${joblog}.small
	joblog=${joblog}.small
	if [ -r $reflog ]
	    then
	    echo " post_check_bi.sh> Now comparing output with reference"

            # ignore diff annotations
            PP='^---|^[[:digit:]]+[acd,][[:digit:]]+'
            # ignore hex addresses
            PP="$PP"'|0x\w{4,}'
            # ignore package names e.g. Package-00-00-00
            PP="$PP"'|\w+-[[:digit:]]{2}-[[:digit:]]{2}-[[:digit:]]{2}'
            # ignore trunk package names e.g. Package-r123456
            PP="$PP"'|\w+-r[[:digit:]]+'
            # ignore history service
            PP="$PP"'|HistorySvc          DEBUG'
            # ignore cpu usage printouts
            PP="$PP"'|ChronoStatSvc +INFO Time'
            PP="$PP"'|Time left.+ Seconds'
            PP="$PP"'|Timeleft.+ sec'
            PP="$PP"'|INFO Time User'
            # ignore clid db file name
            PP="$PP"'|from CLIDDB file'
            # ignore slug machine printout
            PP="$PP"'| Machine: .* System and Processor Info'
            PP="$PP"'| Jobname = .* Machine ='
            # ignore slug pid printout
            PP="$PP"'|Atlas Detector Simulation, Reconstruction and Analysis Running on'
            PP="$PP"'|Program:  Slug-Dice-Arecon .+ pid +[[:digit:]]+'
            #ignore DllClassManager DEBUG messages
            PP="$PP"'|DllClassManager     DEBUG'
            # ignore slug Library printout
            PP="$PP"'|Library of +[[:digit:]]+ at +[[:digit:]]+'
            PP="$PP"'|Library compiled on +[[:digit:]]'
            # ignore ClassIDSvc "in memory db" printouts
            PP="$PP"'|CLID: .* - type name:'
            # ignore ClassIDSvc "already set" printouts
            PP="$PP"'|ClassIDSvc .* setTypeNameForID: .* already set for'
            # ignore ClassIDSvc finalize output
            PP="$PP"'|ClassIDSvc .* finalize: wrote .*'
            # ignore rcs version comments
            PP="$PP"'|Id: .+ Exp \$'
	    # ignore listings
            PP="$PP"'|athena.*listing'
            # ignore ptr values for LArCell
            PP="$PP"'|Found elem'
            # ignore Dict issues
            PP="$PP"'|Dict.so'
            # ignore file names 
            PP="$PP"'|Reading file'
            # ignore root collect key 
            PP="$PP"'|NewEventCollection.root, recovered'

            jobdiff=`basename ${joblog}`-todiff
            refdiff=`basename ${reflog}`-todiff
            egrep -a -v "$PP" < $joblog > $jobdiff
            egrep -a -v "$PP" < $reflog > $refdiff
            diff -a -b -E -B -u $jobdiff $refdiff

	    diffStatus=$?
	    if [ $diffStatus != 0 ] 
		then
		echo "[97;101;1m post_check_bi.sh> ERROR: $joblog and $reflog differ [m"
		exit 1
	    else
		echo "[92;1m post_check_bi.sh> OK: $joblog and $reflog identical [m"
	    fi
	else
	    tail $joblog
	    echo "[93;1m post_check_bi.sh> WARNING: reference output $reflog not available [m"
	    echo  " post_check_bi.sh> Please check ${PWD}/$joblog"
            exit 1
	fi
    else
	tail $joblog
	echo  "[97;101;1m post_check_bi.sh> ERROR: Athena exited abnormally! Exit code: $status [m"
	echo  " post_check_bi.sh> Please check ${PWD}/$joblog"
    fi
fi

# Check output for ERROR/FATAL
joblog=${test}.log
echo 

exit $status

