#!/bin/sh -xv
#/** @file post.sh
# @brief sh script that check the return code of an executable and compares
# its output with a reference (if available).
# @param test_name
#
# @author Paolo Calafiura <pcalafiura@lbl.gov> - ATLAS Collaboration.
# $Id: post_check.sh,v 1.1 2009-04-10 21:47:45 gemmeren Exp $
# **/
status=${?}
test=${1}
ref=${2:-${test}}
if [ -z "${status}" ]
    then
    echo "[93;1m post.sh> Warning: athena exit status is not available [m"
else
    # check exit status
    joblog=${test}.log
    cat ${joblog} |\
	# For now drop some configurable warnings, they are not mine...
	grep -v "^Py:PropertyProxy " |\
	grep -v "^removing: " |\
	grep -v "^Preloading tcmalloc[_a-z]*.so" |\
	grep -v "TEnvRec::ChangeValue:0: RuntimeWarning: duplicate entry" |\
	grep -v "Note: (file \"(tmpfile)\", line 2) " |\
	grep -v "Warning in <TEnvRec::ChangeValue>:" |\
	grep -v "Warning in <TInterpreter::ReadRootmapFile>: class" |\
	grep -v "Welcome to ApplicationMgr " |\
	grep -v "using release " |\
	grep -v "running on " |\
	grep -v "Skipping" |\
	# For some reason, this gives problems, but they are not mine...
	grep -v "AthenaCommon/Execution.py" |\
	# Locally this produces strange characters
	grep -v "ApplicationMgr       INFO Updating Gaudi::PluginService::SetDebug(level) to level=0" |\
	# Py:Athena will be Athena
	sed -e 's/^Py:Athena/Athena   /g' |\
	# Pointer addresses (mark 0)
	sed -e 's/0x0\{8\}/0x!!!!/g' |\
	sed -e 's/0x[0-9a-f]\{8\}/0x????/g' |\
	sed -e 's/0x0\{7\}/0x!!!!/g' |\
	sed -e 's/0x[0-9a-f]\{7\}/0x????/g' |\
	# POOL id
	sed -e 's/0\{8\}-0\{4\}-0\{4\}-0\{4\}-0\{12\}/!!!!/g' |\
	sed -e 's/[0-9A-F]\{8\}-[0-9A-F]\{4\}-[0-9A-F]\{4\}-[0-9A-F]\{4\}-[0-9A-F]\{12\}/????/g' |\
	# Collection MetaData
	sed -e 's/Metadata TTree 0/Metadata TTree !!!/g' |\
	sed -e 's/Metadata TTree [0-9]\{3\}/Metadata TTree ???/g' |\
	grep -v "^PluginMgr            INFO loaded plugin info for [0-9]* components" |\
	grep -v "^Py:PropertyProxy     INFO Read module info for [0-9]* configurables from [0-9]* rootmap files" |\
	grep -v "^DllClassManager      INFO readPluginInfo: loaded plugin info for" |\
	grep -v "^Py:ConfigurableDb " |\
	grep -v "^DetDescrCnvSvc       INFO " |\
	grep -v "^EventPersistenc...   INFO Added successfully Conversion service:DetDescrCnvSvc" |\
	# for 15.6.x and 15.7
	grep -v "^AthenaRootStrea...  DEBUG Service base class initialized successfully" |\
	grep -v "^AthenaRootStrea...   INFO POOL/ROOT class loader initialized" |\
	grep -v "^DbSession Info" |\
	grep -v "^XMLFileCatalog Debug" |\
	grep -v "^PoolXMLFileCatalog Verbose" |\
	sed -e 's/OutputLevel is $/OutputLevel is 2/g' |\
	# DEBUG only messages
	grep -v "^ROBDataProviderSvc  DEBUG" |\
	grep -v "^InputMetaDataStore  DEBUG Recorded object" |\
	grep -v "^InputMetaDataStore  DEBUG Retrieved const handle to object ByteStreamMetadata" |\
	grep -v "^StoreGateSvc        DEBUG Recorded object" |\
	grep -v "^ in DataObject @0x????" |\
	grep -v "^ object modifiable when retrieved" |\
	grep -v "^StoreGateSvc        DEBUG retrieve(default): Retrieved const handle to default object" |\
	grep -v "^ of type EventInfo(CLID 2101)" |\
	# HistorySvc
	grep -v "^HistorySvc" |\
        grep -v "^DBReplicaSvc  *INFO " |\
	grep -v "^DBReplicaSvc  *DEBUG  *HOSTNAME" |\
	grep -v "^DBReplicaSvc  *DEBUG Candidate server" |\
	sed -e "s/ 'DetDescrCnvSvc' ,//g" |\
	# lib maps
	grep -v "^map " |\
        grep -v "^value " |\
	# ClassIDSvc and ServiceManager messages
	grep -v "^ClassIDSvc" |\
	grep -v "^ServiceManager" |\
	grep -v "^CoreDumpSvc          INFO Handling signals" |\
	grep -v "DeprecationWarning: object.__new__" |\
	# Empty lines
	grep -v '^$' \
	> ${joblog}.tmp
    mv ${joblog}.tmp ${joblog}

    if [ "${status}" = 0 ]
	then
	echo "[92;1m post.sh> OK: ${test} exited normally. Output is in ${joblog} [m"
	reflog=../share/${ref}.ref
	if [ -r ${reflog} ]
	    then
#	    echo " post.sh> Now comparing output with reference"
	    diff -a -b -B  ${joblog} ${reflog} |\
                # ignore diff annotations
	        egrep -a -v '^---|^[[:digit:]]+[acd,][[:digit:]]+' |\
                # ignore package names e.g. Package-00-00-00
		egrep -a -v '\w+-[[:digit:]]{2}-[[:digit:]]{2}-[[:digit:]]{2}' |\
		egrep -a -v '\w+-r[[:digit:]]{6}' |\
		# for now ignore IProxyDict dictionary warning
		egrep -a -v 'Warning in <TClass::TClass>: no dictionary for class IProxyDict is available' |\
		# ignore cpu usage printouts
		egrep -a -v 'ChronoStatSvc +INFO Time' |\
		egrep -a -v 'Time left.+ Seconds' |\
		egrep -a -v 'Timeleft.+ sec' |\
		egrep -a -v 'INFO Time User' |\
		# ignore date
		egrep -a -v '[Mon|Tue|Wed|Thu|Fri|Sat|Sun] [[:alpha:]]{3} +[[:digit:]]+ [[:digit:]]{2}:[[:digit:]]{2}:[[:digit:]]{2}' |\
		# ignore clid db file name
		egrep -a -v 'from CLIDDB file'  |\
		# ignore slug machine printout
		egrep -a -v ' Machine: .* System and Processor Info'  |\
		egrep -a -v ' Jobname = .* Machine =' |\
		# ignore slug pid printout
		egrep -a -v 'Atlas Detector Simulation, Reconstruction and Analysis Running on'  |\
		egrep -a -v 'Program:  Slug-Dice-Arecon .+ pid +[[:digit:]]+'  |\
		#ignore DllClassManager DEBUG messages
		egrep -a -v 'DllClassManager     DEBUG' |\
		# ignore slug Library printout
		egrep -a -v 'Library of +[[:digit:]]+ at +[[:digit:]]+'  |\
		egrep -a -v 'Library compiled on +[[:digit:]]'  |\
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
		# ignore DataHeader GUID changes
		egrep -a -v '\[CNT=POOLContainer_DataHeader\]'  |\
		# ignore Appmgr msgs
		egrep -a -v 'ApplicationMgr       INFO Successfully loaded modules' |\
		egrep -a -v 'EventPersistenc...   INFO "CnvServices"'  |\
		egrep -a -v 'ClassIDSvc' |\
		egrep -a -v 'bmagatlas' |\
		egrep -a -v 'GeoModel' |\
		egrep -a -v 'LArNumberHelper'

	    diffStatus=${?}
	    if [ ${diffStatus} = 0 ]
		then
		echo "[97;101;1m post.sh> ERROR: ${joblog} and ${reflog} differ [m"
#		exit 1
	    else
		echo "[92;1m post.sh> OK: ${joblog} and ${reflog} identical [m"
	    fi
	else
	    tail ${joblog}
	    echo "[93;1m post.sh> WARNING: reference output ${reflog} not available [m"
	    echo  " post.sh> Please check ${PWD}/${joblog}"
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
rm -f *.data

exit ${status}
