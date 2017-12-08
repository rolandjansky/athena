#!/bin/sh 
#/** @file post.sh
# @brief sh script that check the return code of an executable and compares 
# its output with a reference (if available).
# @params  
#    first  arg is test file prefix name
#    second arg is the selection string, e.g. Alg name printed out
#
# @author Paolo Calafiura <pcalafiura@lbl.gov> - ATLAS Collaboration.
# $Id: post_check_with_select.sh,v 1.8 2006-03-13 18:09:59 schaffer Exp $
# **/
test=$1
select=$2
#echo "args 1> $1 2> $2 "
#if [ -z "$status" ]
#    then
#    echo "[93;1m post.sh> Warning: athena exit status is not available [m"
#else 
    # check exit status
    joblog=${test}.log
#    if [ "$status" = 0 ]
#	then 
	#echo "[92;1m post.sh> OK: ${test} exited normally. Output is in $joblog [m"
	reflog=../share/${test}.ref

        # If we can't find the reference file, maybe it's located outside
        # the repo.  With the switch to git, we have to fall back
        # to handling the versioning manually.
        # ATLAS_REFERENCE_TAG should be a string of the form PACKAGE/VERSION.
        # We first look for it in DATAPATH.  If we don't find it,
        # we then look under ATLAS_REFERENCE_DATA, which falls back
        # to an afs path if it's not found.
        if [ \( ! -r $reflog \) -a "$ATLAS_REFERENCE_TAG" != "" ]; then
            # Look for the file in DATAPATH.
            # We have to look for the directory, not the file itself,
            # since get_files is hardcoded not to look more than two
            # levels down.
            get_files -data -symlink $ATLAS_REFERENCE_TAG > /dev/null
            reflog=`basename $ATLAS_REFERENCE_TAG`/${test}.ref
            if [ ! -r $reflog ]; then
                testdata=$ATLAS_REFERENCE_DATA
                if [ "$testdata" = "" ]; then
                    testdata=/afs/cern.ch/atlas/maxidisk/d33/referencefiles
                fi
                reflog=$testdata/$ATLAS_REFERENCE_TAG/${test}.ref
            fi
        fi

	if [ -r $reflog ]
	    then
            # If select string is non-zero, use it for the comparison,
            # otherwise do standard diff with exclusions
	    if [ -n "${select}" ]
		then
		#echo "Selecting on: ${select}"
		diff  -a -b -B  $joblog $reflog >> xlog
		diff  -a -b -B  $joblog $reflog |\
		    # select only the differing lines
	        egrep -a '^[<>] ' |\
	        egrep -a ${select} |\
		    # ignore file name after Id - needed for EventInfo tests:
	        egrep -a -v '\$Id:' |\
		    # Ignore line with 'Release' in it
	        egrep -a -v 'Release' |\
	        egrep -a -v 'release' |\
		    # ignore tokens
		egrep -a -v '\[DB=' |\
		    # ignore "package version"
		egrep -a -v 'package version' |\
		    # spurious warning for EventSelector
		egrep -a -v 'Service already offline' |\
		egrep -a -v 'Property update|input handles|output handles|Data Deps|in queryInterface|Default to ConversionSvc|entering handle' |\
		    # Must remove excess print for CaloShowerContainer
		egrep -a -v 'CaloShowerContainer'

	    else 
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
	    fi

	    diffStatus=$?
	    if [ $diffStatus = 0 ] 
		then
		echo "[97;101;1m post.sh> ERROR: $joblog and $reflog differ [m"
		exit 1
	    else
                true
		#echo "[92;1m post.sh> OK: $joblog and $reflog identical [m"
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
#echo 

exit $status

