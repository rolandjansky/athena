#!/bin/sh -xv
#/** @file post.sh
# @brief sh script that check the return code of an executable and compares 
# its output with a reference (if available).
# @param test_name 
#
# @author Paolo Calafiura <pcalafiura@lbl.gov> - ATLAS Collaboration.
# $Id: post_check.sh,v 1.6 2004-10-09 14:01:21 schaffer Exp $
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
	echo "[92;1m post.sh> OK: ${test} exited normally. Output is in $joblog [m"
	reflog=../test/${test}.ref
	if [ -r $reflog ]
	    then
#	    echo " post.sh> Now comparing output with reference"
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
		# ignore file names 
		egrep -a -v 'Reading file'  |\
		# ignore AthenaSealSvc checks
		egrep -a -v 'AthenaSealSvc'  |\
		# ignore root collect key 
		egrep -a -v 'NewEventCollection.root, recovered' |
		# ignore muon dict version
		egrep -a -v 'Dictionary MuonSpectrometer' |
		# ignore Appmgr msgs
		egrep -a -v 'ApplicationMgr       INFO Successfully loaded modules' |
		egrep -a -v 'EventPersistenc...   INFO "CnvServices"'  |
		egrep -a -v 'ClassIDSvc' | 
		egrep -a -v 'bmagatlas' | 
		egrep -a -v 'GeoModel' | 
		egrep -a -v 'LArNumberHelper'

	    diffStatus=$?
	    if [ $diffStatus = 0 ] 
		then
		echo "[97;101;1m post.sh> ERROR: $joblog and $reflog differ [m"
#		exit 1
	    else
		echo "[92;1m post.sh> OK: $joblog and $reflog identical [m"
	    fi
	else
	    tail $joblog
	    echo "[93;1m post.sh> WARNING: reference output $reflog not available [m"
	    echo  " post.sh> Please check ${PWD}/$joblog"
	fi
    else
	tail $joblog
	echo  "[97;101;1m post.sh> ERROR: Athena exited abnormally! Exit code: $status [m"
	echo  " post.sh> Please check ${PWD}/$joblog"
    fi
fi

# Check output for ERROR/FATAL
joblog=${test}.log
echo 

exit $status

