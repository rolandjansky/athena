#!/bin/sh -xv
#/** @file post.sh
# @brief sh script that check the return code of an executable and compares
# its output with a reference (if available).
# @param test_name
#
# @author Paolo Calafiura <pcalafiura@lbl.gov> - ATLAS Collaboration.
# $Id: post_check.sh,v 1.43 2009-05-20 18:02:25 gemmeren Exp $
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
    while [ ! -s ${joblog}.tmp ]; do
    cat ${joblog} |\
	# For now drop some configurable warnings, they are not mine...
	grep -v "^Py:PropertyProxy " |\
	grep -v "^removing: " |\
	grep -v "using release" |\
	grep -v "Warning in <TEnvRec::ChangeValue>:" |\
	grep -v "Error in <TCint::AutoLoad>: failure loading library " |\
	grep -v "Error in <TCint::AutoLoadCallback>: failure loading library " |\
	grep -v "Welcome to ApplicationMgr " |\
	grep -v "running on " |\
	grep -v "Skipping" |\
	# Py:Athena will be Athena
	sed -e 's/^.*Py:Athena /Athena    /g' |\
	# Pointer addresses (mark 0)
	sed -e 's/0x0\{8\}/0x!!!!/g' |\
	sed -e 's/0x[0-9a-f]\{8\}/0x????/g' |\
	sed -e 's/0x0\{7\}/0x!!!!/g' |\
	sed -e 's/0x[0-9a-f]\{7\}/0x????/g' |\
	# Rounding error etc
	sed -e 's/\.\([0-9]\{2\}\)[0-9]*/.\1/g' |\
	sed -e 's/ nan / inf /g' |\
	# POOL id
	sed -e 's/0\{8\}-0\{4\}-0\{4\}-0\{4\}-0\{12\}/!!!!/g' |\
	sed -e 's/[0-9A-F]\{8\}-[0-9A-F]\{4\}-[0-9A-F]\{4\}-[0-9A-F]\{4\}-[0-9A-F]\{12\}/????/g' |\
	sed -e 's/TTree [0-9]\{3\}[0-9]*/TTree ????/g' |\
	sed -e 's/Bytes: [0-9]\{3\}[0-9]*/Bytes: ????/g' |\
	# 64 bit offsets
	sed -e 's/fffffffff/f/g' |\
	sed -e 's/000000000/0/g' |\
	# Collection MetaData
	sed -e 's/Metadata TTree 0/Metadata TTree !!!/g' |\
	sed -e 's/Metadata TTree [0-9]\{3\}/Metadata TTree ???/g' |\
	# Remove Archive Prefix
	sed -e 's/root:\/\/castoratlas\/\/castor\/cern.ch\/user\/g\/gemmeren\/ArchiveFile.zip#EmptyPoolFile/EmptyPoolFile/g' |\
	sed -e 's/root:\/\/castoratlas\/\/castor\/cern.ch\/user\/g\/gemmeren\/ArchiveFile.zip#SimplePoolFile/SimplePoolFile/g' |\
	sed -e 's/ArchiveFile.zip#EmptyPoolFile/EmptyPoolFile/g' |\
	sed -e 's/ArchiveFile.zip#SimplePoolFile/SimplePoolFile/g' |\
	# Stream wildcard
	sed -e 's/INFO CLID = 222376821, key = Stream[12]/INFO CLID = 222376821, key = StreamX/g' |\
	grep -v "^Stream[1-9]             DEBUG Wild card in key: begining part -  ending part -" |\
	grep -v "^PluginMgr            INFO loaded plugin info for [0-9]* components" |\
	grep -v "^Py:PropertyProxy     INFO Read module info for [0-9]* configurables from [0-9]* rootmap files" |\
	grep -v "^DllClassManager      INFO readPluginInfo: loaded plugin info for" |\
        grep -v "^Py:ConfigurableDb    INFO Read module info for" |\
	grep -v "^DetDescrCnvSvc       INFO " |\
	grep -v "^EventPersistenc...   INFO Added successfully Conversion service:DetDescrCnvSvc" |\
	sed -e "s/ 'DetDescrCnvSvc' ,//g" |\
	# HistorySvc
	grep -v "^HistorySvc" |\
	# ExampleHit Streamer for new object.
	grep -v "Found unknown streamer checksum" |\
	# CLHEP Streamer.
	grep -v "AthenaRootStrea...   INFO   - Streamer name:" |\
	grep -v "AthenaRootStrea...   INFO ROOT Streamer for" |\
	grep -v "AthenaRootStrea...   INFO Adopted streamer for class" |\
        grep -v "DBReplicaSvc         INFO " |\
	# lib maps
	grep -v "^map " |\
        grep -v "^value " |\
	# ROOT file version
	grep -v "Root file version:" |\
	grep -v "File version:" |\
	# ClassIDSvc and ServiceManager messages
	grep -v "^ClassIDSvc" |\
	grep -v "^ServiceManager" |\
	grep -v "^CoreDumpSvc          INFO Handling signals" |\
	# Strange Chars from ApplicationMgr
	sed -e "s/^..*ApplicationMgr/ApplicationMgr/" |\
	# Custom jobOptions
	sed -e "s/.\/${test}.py/AthenaPoolExampleAlgorithms\/${ref}JobOptions.py/" |\
	sed -e "s/${test}.py/AthenaPoolExampleAlgorithms\/${ref}JobOptions.py/" |\
	sed -e "s/.\/AthenaPoolExample_ReadBsTag.py/AthenaPoolExampleAlgorithms\/AthenaPoolExample_ReadBsTagJobOptions.py/" |\
	sed -e "s/AthenaPoolExample_ReWriteJobOptions.py/AthenaPoolExample_RWJobOptions.py/" |\
	sed -e "s/AthenaPoolExample_ReadSkipJobOptions.py/AthenaPoolExample_ReadJobOptions.py/" |\
	# Empty lines
	grep -v '^$' \
	> ${joblog}.tmp
       if [ ! -s ${joblog}.tmp ]; then
         # ??? Sometimes the filtered file is empty.  Not sure why.
         #     It's not reproducible; running the filter again usually works.
         sleep 1
         echo Retry ${joblog}.tmp
       fi
    done
    #cp ${joblog} ${joblog}-orig
    mv ${joblog}.tmp ${joblog}
    if [ "${test}" == "AthenaPoolExample_WriteFast" ]
    then
        cat ${joblog} |\
        sed -e 's/SimplePoolFileA.root/SimplePoolFile2.root/g' > ${joblog}.tmp
        mv ${joblog}.tmp ${joblog}
    elif [ "${test}" == "AthenaPoolExample_ReadTagFast" ]
    then
        cat ${joblog} |\
	sed -e 's/SimplePoolFile4.root/SimplePoolFile2.root/g' |\
	sed -e 's/SimplePoolCollection4.root/SimplePoolCollection2.root/g' > ${joblog}.tmp
        mv ${joblog}.tmp ${joblog}
    elif [ "${test}" == "AthenaPoolExample_ReWriteAgain" ]
    then
        cat ${joblog} |\
	sed -e 's/SimplePoolReplica1.root/SimplePoolFile1.root/g' > ${joblog}.tmp
        mv ${joblog}.tmp ${joblog}
    elif [ "${test}" == "AthenaPoolExample_ReWriteNext" ]
    then
        cat ${joblog} |\
	sed -e 's/SimplePoolReplica1.root/SimplePoolFile1.root/g' > ${joblog}.tmp
        mv ${joblog}.tmp ${joblog}
    elif [ "${test}" == "AthenaPoolExample_ReadAgain" ]
    then
        cat ${joblog} |\
        sed -e 's/4.root/3.root/g' |\
	sed -e 's/SimplePoolReplica1.root/SimplePoolFile1.root/g' > ${joblog}.tmp
        mv ${joblog}.tmp ${joblog}
    elif [ "${test}" == "AthenaPoolExample_ReadBNzip" -o "${test}" == "AthenaPoolExample_ReadBNzipAgain" ]
    then
        cat ${joblog} |\
	sed '/SimplePoolFile3.root/,/[)0]/ s/Stream2/Stream1/g' > ${joblog}.tmp
        mv ${joblog}.tmp ${joblog}
    elif [ "${test}" == "AthenaPoolExample_ReadConcat" ]
    then
        cat ${joblog} |\
	sed 's/AthenaPoolExample_ReadJobOptions/AthenaPoolExample_ReadConcatJobOptions/g' |\
	sed '/SimplePoolFile3.root/,/[)0]/ s/Stream2/Stream1/g' > ${joblog}.tmp
        mv ${joblog}.tmp ${joblog}
    fi

    if [ -e ../test/${test}.pattern ]
    then
        cat ../test/empty.pattern ../test/${test}.pattern > ../run/${test}.pattern
	pattern=../run/${test}.pattern
    else
	pattern=../test/empty.pattern
    fi
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
		egrep -a -v '\w+-r[[:digit:]]{6}' |\
		egrep -a -v '\w+-[[:digit:]]{2}-[[:digit:]]{2}-[[:digit:]]{2}' |\
                # ignore ROOT version details
		egrep -a -v 'File version:53[[:digit:]]{3}' |\
		# for now ignore IProxyDict dictionary warning
		egrep -a -v 'Warning in <TClass::TClass>: no dictionary for class IProxyDict is available' |\
		# ignore cpu usage printouts
		egrep -a -v 'ChronoStatSvc +INFO Time' |\
		egrep -a -v 'Time left.+ Seconds' |\
		egrep -a -v 'Timeleft.+ sec' |\
		egrep -a -v 'INFO Time User' |\
		# ignore date and release
		egrep -a -v '[Mon|Tue|Wed|Thu|Fri|Sat|Sun] [[:alpha:]]{3} +[[:digit:]]+ [[:digit:]]{2}:[[:digit:]]{2}:[[:digit:]]{2}' |\
		egrep -a -v 'Athena               INFO using release' |\
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
		#egrep -a -v 'Found elem'  |\
		# ignore file names
		egrep -a -v 'Reading file'  |\
		# ignore AthenaSealSvc checks
		egrep -a -v 'AthenaSealSvc'  |\
		# ignore DataHeader GUID changes
		egrep -a -v '\[CNT=POOLContainer_DataHeader\]'  |\
		egrep -a -v '\[CNT=POOLContainer\(DataHeader\)\]'  |\
		# ignore Appmgr msgs
		egrep -a -v 'ApplicationMgr       INFO Successfully loaded modules' |\
		egrep -a -v 'EventPersistenc...   INFO "CnvServices"'  |\
		egrep -a -v 'ClassIDSvc' |\
		egrep -a -v 'bmagatlas' |\
		egrep -a -v 'GeoModel' |\
		egrep -a -v 'LArNumberHelper' |\
		egrep -a -v 'including file' |\
		egrep -a -v 'Store +INFO' |\
		egrep -a -v 'StoreGateSvc +INFO' |\
		egrep -a -v 'Py:ConfigurableDb WARNING' |\
		egrep -v -f ${pattern} \
	    > ${joblog}.tmp
	    grep "^< " ${joblog}.tmp | cut -d' ' -f2-99 > ${joblog}.tmp1
	    grep "^> " ${joblog}.tmp | cut -d' ' -f2-99 > ${joblog}.tmp2
	    diff ${joblog}.tmp1 ${joblog}.tmp2
	    diffStatus=${?}
	    rm ${joblog}.tmp*
	    if [ ${diffStatus} != 0 ]
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
	exit 1
    fi
    if [ "${test}" == "AthenaPoolExample_AppendFast" ]
    then
	mergePOOL.exe -o SimplePoolFile2.root -i SimplePoolFileA.root -i SimplePoolFileB.root
	mv Catalog1.xml Catalog1.xml.old
	rm SimplePoolFileA.root SimplePoolFileB.root
	pool_insertFileToCatalog -u xmlcatalog_file:Catalog1.xml EmptyPoolFile.root SimplePoolFile1.root SimplePoolFile2.root
    fi
fi

# Check output for ERROR/FATAL
joblog=${test}.log
echo

exit ${status}
