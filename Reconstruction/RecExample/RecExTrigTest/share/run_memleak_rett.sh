#!/bin/bash

echo "running esdTrig_memleak with CMTCONFIG $CMTCONFIG"
if [[ $CMTCONFIG = x86_64-* ]]; then
    echo "64 bit : igprof"
    export LD_LIBRARY_PATH=${LD_LIBRARY_PATH}:/afs/cern.ch/atlas/offline/external/igprof/5.9.2/${CMTCONFIG}/lib
    export PATH=${PATH}:/afs/cern.ch/atlas/offline/external/igprof/5.9.2/${CMTCONFIG}/bin
    igprof -mp -z -t python athena.py -c "jp.AthenaCommonFlags.EvtMax=-1; rec.doCBNT=False ;jp.AthenaCommonFlags.FilesInput=['/afs/cern.ch/atlas/project/rig/referencefiles/dataStreams_high_mu/high_mu-data11_7TeV.00179725.physics_JetTauEtmiss.merge.RAW._lb0021.data'] " RecExTrigTest/RecExTrigTest_RTT_esdtrigger.py --stdcmalloc
    mkdir -p web/data
    cp /afs/cern.ch/atlas/offline/external/igprof/5.9.2/${CMTCONFIG}/bin/igprof-navigator web
    igprof-analyse --sqlite -d -v -g -r MEM_LIVE igprof.*.gz | sqlite3 web/data/igreport_total.sql3
else
    echo "32 bit leak check execute"
    athena.py -c "jp.AthenaCommonFlags.EvtMax=-1; rec.doCBNT=False ;jp.AthenaCommonFlags.FilesInput=['/afs/cern.ch/atlas/project/rig/referencefiles/dataStreams_high_mu/high_mu-data11_7TeV.00179725.physics_JetTauEtmiss.merge.RAW._lb0021.data'] " RecExTrigTest/RecExTrigTest_RTT_esdtrigger.py --stdcmalloc
fi
