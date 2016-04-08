#!/bin/sh -xv
#
echo "Starting pre_check.sh"
# Cleaning
test=${1}
/bin/rm -f ${test}.log ${test}.py
#
if [ "${test}" = "ESD_base" ]
then
	pool_insertFileToCatalog /afs/cern.ch/atlas/maxidisk/d33/referencefiles/esd/ESD-1[56]*.pool.root &> /dev/null
	cp ../share/esdtoesd_base.py esdtoesd_base.py
	for esdfile in `ls /afs/cern.ch/atlas/maxidisk/d33/referencefiles/esd/ESD-1[56]*.pool.root`
	do
		echo "svcMgr.EventSelector.InputCollections += [\"${esdfile}\"]" >> esdtoesd_base.py
	done
elif [ "${test}" = "AOD_base" ]
then
	pool_insertFileToCatalog /afs/cern.ch/atlas/maxidisk/d33/referencefiles/aod/*/AOD-1[56]*-full.pool.root &> /dev/null
	cp ../share/aodtoaod_base.py aodtoaod_base.py
	for aodfile in `ls /afs/cern.ch/atlas/maxidisk/d33/referencefiles/aod/*/AOD-1[56]*-full.pool.root`
	do
		echo "svcMgr.EventSelector.InputCollections += [\"${aodfile}\"]" >> aodtoaod_base.py
	done
fi
echo "## setting up environment "
echo "## ready to run"
echo $CMTPATH
echo $JOBOPTSEARCHPATH
# Turn off pool verbose printing
export POOL_OUTMSG_LEVEL=4
