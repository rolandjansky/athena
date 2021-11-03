#!/bin/sh
# Cleaning
test=${1}
/bin/rm -f ${test}.log ${test}.py
#
if [ "${test}" = "AthenaPoolExample_Write" ]
then
	/bin/rm -f *.root Catalog1.xml
	FCregisterPFN -p SimplePoolFile1.root -u xmlcatalog_file:Catalog1.xml -t ROOT_All -g F2313678-C96A-964F-89E8-08BAB39F2DA7
	FCregisterPFN -p SimplePoolFile2.root -u xmlcatalog_file:Catalog1.xml -t ROOT_All -g 73F77177-D136-9344-B64D-29693F68D6C2
	FCregisterPFN -p SimplePoolFile3.root -u xmlcatalog_file:Catalog1.xml -t ROOT_All -g A811B014-0297-AD47-AF4C-B75EF418982D
	FCregisterPFN -p SimplePoolFile4.root -u xmlcatalog_file:Catalog1.xml -t ROOT_All -g 298DE14D-49CF-674A-9171-CEBBD9BEC6F8
	FCregisterPFN -p SimplePoolReplica1.root -u xmlcatalog_file:Catalog1.xml -t ROOT_All -g 095498FF-E805-B142-9948-BD2D4AC79975
	FCregisterPFN -p EmptyPoolFile.root -u xmlcatalog_file:Catalog1.xml -t ROOT_All -g C7040C30-3363-7D42-B1B7-E3F3B1881030
elif [ "${test}" = "AthenaPoolExample_Concat" ]
then
	/bin/rm -f SimplePoolFile[13].root
elif [ "${test}" = "AthenaPoolExample_WMeta" ]
then
	/bin/rm -f *.root Catalog2.xml
	FCregisterPFN -p SimplePoolFile5.root -u xmlcatalog_file:Catalog2.xml -t ROOT_All -g C949FD2E-3B8E-9343-AAE0-1E2306900C43
fi
# Turn off pool verbose printing
export POOL_OUTMSG_LEVEL=4
