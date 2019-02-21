#!/bin/sh
#/** @file post_check_co.sh
# @brief sh script that check the return code of an executable and compares
# its output with a reference (if available). Modified to restrict checks
# to output from AthenaPoolMultiTest CheckOutput test.
# @param test_name
#
# @author Jack Cranshaw (Jack.Cranshaw@cern.ch), Paolo Calafiura (pcalafiura@lbl.gov).
# $Id: post_check_co.sh,v 1.4 2006-04-27 21:03:11 cranshaw Exp $
# @param test_name 
#
# @author Paolo Calafiura <pcalafiura@lbl.gov> - ATLAS Collaboration.
# $Id: post_check_co.sh,v 1.4 2006-04-27 21:03:11 cranshaw Exp $
# **/
test=$1
status=${?}
if [ -z "$status" ]
    then
    echo "[93;1m post_check_co.sh> Warning: athena exit status is not available [m"
else 
    # check exit status
    joblog=${test}.log
    if [ "$status" = 0 ]
	then 
	#echo "[92;1m post_check_co.sh> OK: ${test} exited normally. Output is in $joblog [m"
	reflog=../share/${test}.ref
        grep -e 'ReadData' \
             -e 'EventSelector' \
             -e 'rimary' \
             -e 'ERROR' \
             -e 'Error' \
             ${joblog} |\
             # Rounding error etc
	     sed -e 's/\.\([0-9]\{1\}\)[0-9]*/.\1/g' |\
	     sed -e 's/ nan / inf /g' |\
             grep -v 'EventSelectorAthenaPool' | \
             grep -v 'EventBookkeeper' | \
             grep -v 'POOLCollectionID' \
             > ${joblog}.small
	joblog=${joblog}.small
	if [ -r $reflog ]
	    then
#	    echo " post_check_co.sh> Now comparing output with reference"
	    diff -a -b -B  $joblog $reflog |\
                # ignore diff annotations
	        egrep -a -v '^---|^[[:digit:]]+[acd,][[:digit:]]+' |\
                # ignore hex addresses
		egrep -a -v ' 0x\w{4,}' |\
                # ignore package names e.g. Package-00-00-00
		egrep -a -v '\w+-[[:digit:]]{2}-[[:digit:]]{2}-[[:digit:]]{2}' |\
		# ignore cpu usage printouts
		egrep -a -v 'ChronoStatSvc +INFO Time' |\
		egrep -a -v 'Time left.+ Seconds' |\
		egrep -a -v 'Timeleft.+ sec' |\
		egrep -a -v 'INFO Time User' |\
                egrep -a -v 'UnixTimestamp' | \
		# ignore clid db file name
		grep -a -v 'from CLIDDB file'  |\
		# ignore slug machine printout
		egrep -a -v ' Machine: .* System and Processor Info'  |\
		egrep -a -v ' Jobname = .* Machine =' |\
		# ignore slug pid printout
		grep -a -v 'Atlas Detector Simulation, Reconstruction and Analysis Running on'  |\
		egrep -a -v 'Program:  Slug-Dice-Arecon .+ pid +[[:digit:]]+'  |\
		#ignore DllClassManager DEBUG messages
		egrep -a -v 'DllClassManager     DEBUG' |\
		# ignore slug Library printout
		egrep -a -v 'Library of +[[:digit:]]+ at +[[:digit:]]+'  |\
		egrep -a -v 'Library compiled on +[[:digit:]]'  |\
		# ignore ClassIDSvc "in memory db" printouts
		egrep -a -v 'CLID: .* - type name:' |\
		# ignore ClassIDSvc "already set" printouts
		egrep -a -v 'ClassIDSvc .* setTypeNameForID: .* already set for' |\
		# ignore ClassIDSvc finalize output
		egrep -a -v 'ClassIDSvc * DEBUG finalize: wrote .*'   |\
		# ignore rcs version comments
		egrep -a -v 'Id: .+ Exp \$'  |\
		# ignore listings
		egrep -a -v 'athena.*listing'  |\
		# ignore ptr values for LArCell
		egrep -a -v 'Found elem'  |\
                # ignore Dict issues
                egrep -a -v 'Dict.so' |\
		# ignore file mgr
		egrep -a -v 'FileMgr'  |\
		# ignore file names 
		egrep -a -v 'Reading file'  |\
		# ignore root collect key 
		egrep -a -v 'NewEventCollection.root, recovered' |\
                # EventSelector sourceID
		egrep -a -v 'Disconnecting input sourceID'

	    diffStatus=$?
	    if [ $diffStatus -ne 1 ] 
		then
		echo "[97;101;1m post_check_co.sh> ERROR: $joblog and $reflog differ [m"
		exit 1
	    else
                true
		#echo "[92;1m post_check_co.sh> OK: $joblog and $reflog identical [m"
	    fi
	else
	    tail $joblog
	    echo "[93;1m post_check_co.sh> WARNING: reference output $reflog not available [m"
	    echo  " post_check_co.sh> Please check ${PWD}/$joblog"
	fi
    else
	tail $joblog
	echo  "[97;101;1m post_check_co.sh> ERROR: Athena exited abnormally! Exit code: $status [m"
	echo  " post_check_co.sh> Please check ${PWD}/$joblog"
        exit 1
    fi
fi

# Check output for ERROR/FATAL
joblog=${test}.log
#echo 

exit $status

