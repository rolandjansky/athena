#!/bin/sh -xv
# Cleaning
test=${1}
/bin/rm -f ${test}.log ${test}.py
#
if [ "${test}" = "AthenaPoolExample_Write" ]
then
	#echo "## clean up"
	#echo $LD_LIBRARY_PATH
	/bin/rm -f *
elif [ "${test}" = "AthenaPoolExample_Concat" ]
then
	#echo "## clean up ii"
	/bin/rm -f SimplePoolFile[13].root
elif [ "${test}" = "AthenaPoolExample_WriteFast" ]
then
	#echo "## clean up iii"
	/bin/rm -f SimplePool*[1-4].root #*.xml*
#	cat ../share/AthenaPoolExample_WriteJobOptions.py | \
#	sed -e 's/SimplePoolFile2.root/SimplePoolFileA.root/g' > ${test}.py
#elif [ "${test}" = "AthenaPoolExample_AppendFast" ]
#then
#	cat ../share/AthenaPoolExample_AppendJobOptions.py | \
#	sed -e 's/Stream1/Stream2/g' | \
#	sed -e 's/SimplePoolFile2.root/SimplePoolFileB.root/g' > ${test}.py
#elif [ "${test}" = "AthenaPoolExample_ReWriteTagFast" ]
#then
#	cat ../share/AthenaPoolExample_RWJobOptions.py | \
#	sed -e 's/SimplePoolFile1.root/SimplePoolFile2.root/g' | \
#	sed -e 's/SimplePoolFile3.root/SimplePoolFile4.root/g' | \
#	sed -e 's/SimplePoolCollection3.root/SimplePoolCollection4.root/g' | \
#	sed -e 's/^RegStream1.WriteInputDataHeader = False/RegStream1.WriteInputDataHeader = True/g' | \
#	sed -e 's/DataHeader#Stream1/DataHeader#\*/g' > ${test}.py
#elif [ "${test}" = "AthenaPoolExample_ReadTagFast" ]
#then
#	cat ../share/AthenaPoolExample_ReadJobOptions.py | \
#	sed -e 's/SimplePoolCollection2.root/SimplePoolCollection4.root/g' | \
#	sed -e 's/^svcMgr.EventSelector.SkipEvent/#svcMgr.EventSelector.SkipEvent/g' | \
#	sed -e 's/^#svcMgr.EventSelector.InputCollections/svcMgr.EventSelector.InputCollections/g' | \
#	sed -e 's/^#svcMgr.EventSelector.CollectionType/svcMgr.EventSelector.CollectionType/g' | \
#	sed -e 's/^#svcMgr.EventSelector.HelperTools/svcMgr.EventSelector.HelperTools/g' | \
#	sed -e 's/^#Switch Off for TAG - start/\"\"\"/g' | \
#	sed -e 's/^#Switch Off for TAG - end/\"\"\"/g' > ${test}.py
elif [ "${test}" = "AthenaPoolExample_WMeta" ]
then
	#echo "## clean up iii"
	/bin/rm -f Catalog1.xml* SimplePool*1.root SimplePool*2.root SimplePool*3.root SimplePool*4.root
	/bin/rm -f *.root *.xml* *.h5
elif [ "${test}" == "AthenaPoolExample_ReWriteAgain" ]
then
	rm SimplePool*3.root
#	cat ../share/AthenaPoolExample_RWJobOptions.py | \
#	sed -e 's/SimplePoolFile1.root/SimplePoolReplica1.root/g' > ${test}.py
#elif [ "${test}" == "AthenaPoolExample_ReWriteNext" ]
#then
#	cat ../share/AthenaPoolExample_RWJobOptions.py | \
#	#sed -e 's/#Stream1.WritingTool.ProvenanceTags/Stream1.WritingTool.ProvenanceTags/g' | \
#	sed -e 's/3.root/4.root/g' | \
#	sed -e 's/1.root/3.root/g' > ${test}.py
elif [ "${test}" == "AthenaPoolExample_ReWriteSkip" ]
then
	/bin/rm -f TruncatedPoolFile.root SimplePool*3.root SimplePool*4.root Catalog1.xml*
	pool_insertFileToCatalog -u xmlcatalog_file:Catalog1.xml *File*.root
	cat ../share/AthenaPoolExample_RWJobOptions.py | \
	sed -e 's/^#Stream1.ExtendProvenanceRecord = FALSE/Stream1.ExtendProvenanceRecord = FALSE/g' > ${test}.py
elif [ "${test}" == "AthenaPoolExample_ReadAgain" ]
then
	FCdeletePFN -u xmlcatalog_file:Catalog1.xml -p EmptyPoolFile.root
	FCaddReplica -u xmlcatalog_file:Catalog1.xml -r NotFoundPoolFile.root -p SimplePoolFile3.root
	FCaddReplica -u xmlcatalog_file:Catalog1.xml -r TruncatedPoolFile.root -p SimplePoolFile4.root
	FCdeletePFN -u xmlcatalog_file:Catalog1.xml -p SimplePoolFile4.root
#	cat ../share/AthenaPoolExample_ReadJobOptions.py | \
#	sed -e 's/^#svcMgr.PoolSvc.AttemptCatalogPatch/svcMgr.PoolSvc.AttemptCatalogPatch/g' | \
#	sed -e 's/SimplePoolFile1.root/SimplePoolReplica1.root/g' | \
#	sed -e 's/, "SimplePoolFile3.root"/, "NotTherePoolFile.root", "NotFoundPoolFile.root", "TruncatedPoolFile.root", "SimplePoolFile4.root"/g' > ${test}.py
elif [ "${test}" == "AthenaPoolExample_ReadBsTag" ]
then
	#catalogBytestreamFiles.sh /afs/cern.ch/atlas/offline/test/daq.m4_combined.0020720.extract.L1TT-b00000010._0001.data
	FCregisterPFN -g "7AB2B62D-7276-DC11-9AB7-0018FE6D438B" -t "BYTE_STREAM" -p "/afs/cern.ch/atlas/offline/test/daq.m4_combined.0020720.extract.L1TT-b00000010._0001.data" # For now, as catalogBytestreamFiles.sh fails on gcc4.8
	rm SimplePoolFromRaw.root
	cat ../share/AthenaPoolExample_ReadBsJobOptions.py | \
	sed -e 's/^#svcMgr.EventSelector.InputCollections/svcMgr.EventSelector.InputCollections/g' | \
	sed -e 's/^#svcMgr.EventSelector.CollectionType/svcMgr.EventSelector.CollectionType/g' | \
	sed -e 's/^#svcMgr.EventSelector.RefName/svcMgr.EventSelector.RefName/g' | \
	sed -e 's/^#svcMgr.EventSelector.Query/svcMgr.EventSelector.Query/g' | \
	grep -v "^\"\"\"" > ${test}.py
elif [ "${test}" == "AthenaPoolExample_RWcBs" ]
then
	cat ../share/AthenaPoolExample_RWBsJobOptions.py | \
	sed -e 's/^#svcMgr.ByteStreamInputSvc.FullFileName/svcMgr.ByteStreamInputSvc.FullFileName/g' > ${test}.py
	AtlCopyBSEvent.exe -d -e 14350,14356,14382 -o test_defl.data /afs/cern.ch/atlas/offline/test/daq.m4_combined.0020720.extract.L1TT-b00000010._0001.data
	#catalogBytestreamFiles.sh test_defl.data
        guid=`strings ../run/test_defl.data | grep "GUID=" | cut -d= -f2`
	FCregisterPFN -g ${guid} -t "BYTE_STREAM" -p "test_defl.data" # For now, as catalogBytestreamFiles.sh fails on gcc4.8
elif [ "${test}" == "AthenaPoolExample_ReadBNzip" ]
then
	zip -n root ArchiveFile EmptyPoolFile.root SimplePoolFile*
	FCaddReplica -u xmlcatalog_file:Catalog1.xml -r ArchiveFile.zip#EmptyPoolFile.root -p EmptyPoolFile.root
	FCaddReplica -u xmlcatalog_file:Catalog1.xml -r ArchiveFile.zip#SimplePoolFile1.root -p SimplePoolFile1.root
	FCdeletePFN -u xmlcatalog_file:Catalog1.xml -p EmptyPoolFile.root
	FCdeletePFN -u xmlcatalog_file:Catalog1.xml -p SimplePoolFile1.root
	FCdeletePFN -u xmlcatalog_file:Catalog1.xml -p SimplePoolFile2.root
	FCdeletePFN -u xmlcatalog_file:Catalog1.xml -p SimplePoolFile3.root
	/bin/rm -f EmptyPoolFile.root SimplePoolFile*
	pool_insertFileToCatalog -u xmlcatalog_file:Catalog1.xml ArchiveFile.zip#SimplePoolFile2.root ArchiveFile.zip#SimplePoolFile3.root
	cat ../share/AthenaPoolExample_ReadJobOptions.py | \
	sed -e 's/^svcMgr.AthenaPoolAddressProviderSvc.BackNavigation = TRUE/svcMgr.AthenaPoolAddressProviderSvc.BackNavigation = FALSE/g' | \
	sed -e 's/^#svcMgr.AthenaPoolAddressProviderSvc.BackNavigationScope/svcMgr.AthenaPoolAddressProviderSvc.BackNavigationScope/g' | \
	sed -e 's/EmptyPoolFile/ArchiveFile.zip#EmptyPoolFile/g' | \
	sed -e 's/SimplePoolFile/ArchiveFile.zip#SimplePoolFile/g' > ${test}.py
elif [ "${test}" == "AthenaPoolExample_ReadBNzipAgain" ]
then
	cat ./AthenaPoolExample_ReadBNzip.py | \
	sed -e 's/ArchiveFile/root:\/\/castoratlas\/\/castor\/cern\.ch\/user\/g\/gemmeren\/ArchiveFile/g' > ${test}.py
	xrdcp ArchiveFile.zip root://castoratlas//castor/cern.ch/user/g/gemmeren/ArchiveFile.zip
	rm Catalog1.xml ArchiveFile.zip
	pool_insertFileToCatalog -u xmlcatalog_file:Catalog1.xml root://castoratlas//castor/cern.ch/user/g/gemmeren/ArchiveFile.zip#EmptyPoolFile.root root://castoratlas//castor/cern.ch/user/g/gemmeren/ArchiveFile.zip#SimplePoolFile1.root root://castoratlas//castor/cern.ch/user/g/gemmeren/ArchiveFile.zip#SimplePoolFile2.root root://castoratlas//castor/cern.ch/user/g/gemmeren/ArchiveFile.zip#SimplePoolFile3.root
fi
# Turn off pool verbose printing
export POOL_OUTMSG_LEVEL=4
