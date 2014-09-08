#!/bin/bash

if [ $# -eq 0 ]; then
 echo "stop, no arguments given !"
fi

RUN=0

if [[ $1 == "esd_notrig_memleakAna" ]]; then
    echo "running $1 with CMTCONFIG $CMTCONFIG"
    if [[ $CMTCONFIG = x86_64-* ]]; then
	echo "64 bit : igprof"
	export LD_LIBRARY_PATH=${LD_LIBRARY_PATH}:/afs/cern.ch/atlas/offline/external/igprof/5.9.2/${CMTCONFIG}/lib
	export PATH=${PATH}:/afs/cern.ch/atlas/offline/external/igprof/5.9.2/${CMTCONFIG}/bin
	igprof -mp -z -t python athena.py -c "jp.AthenaCommonFlags.EvtMax=50; rec.doCBNT=False ;jp.AthenaCommonFlags.FilesInput=['/afs/cern.ch/atlas/project/rig/referencefiles/dataStreams_high_mu/high_mu-data11_7TeV.00179725.physics_JetTauEtmiss.merge.RAW._lb0021.data'] " RecExAnaTest/RecExAnaTest_RTT_notrigger.py --stdcmalloc
	mkdir -p web/data
	cp /afs/cern.ch/atlas/offline/external/igprof/5.9.2/${CMTCONFIG}/bin/igprof-navigator web
	igprof-analyse --sqlite -d -v -g -r MEM_LIVE igprof.*.gz | sqlite3 web/data/igreport_total.sql3
    else
	echo "32 bit leak check execute"
	athena.py -c "jp.AthenaCommonFlags.EvtMax=50; rec.doCBNT=False ;jp.AthenaCommonFlags.FilesInput=['/afs/cern.ch/atlas/project/rig/referencefiles/dataStreams_high_mu/high_mu-data11_7TeV.00179725.physics_JetTauEtmiss.merge.RAW._lb0021.data'] " RecExAnaTest/RecExAnaTest_RTT_notrigger.py --leak-check-execute
    fi
    RUN=1
fi

if [[ $1 == "esd_memleakAna" ]]; then
    echo "running $1 with CMTCONFIG $CMTCONFIG"
    if [[ $CMTCONFIG = x86_64-* ]]; then
	echo "64 bit : igprof"
	export LD_LIBRARY_PATH=${LD_LIBRARY_PATH}:/afs/cern.ch/atlas/offline/external/igprof/5.9.2/${CMTCONFIG}/lib
	export PATH=${PATH}:/afs/cern.ch/atlas/offline/external/igprof/5.9.2/${CMTCONFIG}/bin
	igprof -mp -z -t python athena.py -c "jp.AthenaCommonFlags.EvtMax=-1; rec.doCBNT=False ;jp.AthenaCommonFlags.FilesInput=[\"/afs/cern.ch/atlas/project/rig/referencefiles/MC/valid1.105200.T1_McAtNlo_Jimmy.digit.RDO.e835_s1256_s1258_d566_tid464353_00/RDO.464353._000001.pool.root.1\"] " RecExAnaTest/RecExAnaTest_RTT_all.py --stdcmalloc
	mkdir -p web/data
	cp /afs/cern.ch/atlas/offline/external/igprof/5.9.2/${CMTCONFIG}/bin/igprof-navigator web
	igprof-analyse --sqlite -d -v -g -r MEM_LIVE igprof.*.gz | sqlite3 web/data/igreport_total.sql3
    else
	echo "32 bit : leak check execute"
	athena.py -c "jp.AthenaCommonFlags.EvtMax=-1; rec.doCBNT=False ;jp.AthenaCommonFlags.FilesInput=[\"/afs/cern.ch/atlas/project/rig/referencefiles/MC/valid1.105200.T1_McAtNlo_Jimmy.digit.RDO.e835_s1256_s1258_d566_tid464353_00/RDO.464353._000001.pool.root.1\"] " RecExAnaTest/RecExAnaTest_RTT_all.py --leak-check-execute
    fi
    RUN=1
fi

if [ $RUN -eq 0 ]; then
    echo "don't understand the parameter $1"
fi
