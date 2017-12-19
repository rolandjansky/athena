#!/bin/bash

if [ $# -eq 0 ]; then
 echo "stop, no arguments given !"
fi

RUN=0

if [[ $1 == "mc11_memleak" ]]; then
    echo "running $1 with CMTCONFIG $CMTCONFIG"
    if [[ $CMTCONFIG = x86_64-* ]]; then
	echo "64 bit : igprof"
	Reco_trf.py inputRDOFile=root://eosatlas.cern.ch//eos/atlas/atlascerngroupdisk/phys-rig/mc10_7TeV.105200.T1_McAtNlo_Jimmy.digit.RDO.e598_s933_s946_d366_tid197091_00/RDO.197091._000001.pool.root.1 outputESDFile=T1_McAtNlo_Jimmy.ESD.pool.root outputAODFile=T1_McAtNlo_Jimmy.AOD.pool.root outputHISTFile=T1_McAtNlo_Jimmy.HIST.root maxEvents=500 autoConfiguration=everything --athenaopts="--config-only=job.cfg --keep-configuration"
	export LD_LIBRARY_PATH=${LD_LIBRARY_PATH}:/afs/cern.ch/atlas/offline/external/igprof/5.9.2/${CMTCONFIG}/lib
	export PATH=${PATH}:/afs/cern.ch/atlas/offline/external/igprof/5.9.2/${CMTCONFIG}/bin
	igprof -mp -z -t python athena.py job.cfg.pkl --stdcmalloc
	mkdir -p web/data
	cp /afs/cern.ch/atlas/offline/external/igprof/5.9.2/${CMTCONFIG}/bin/igprof-navigator web
	igprof-analyse --sqlite -d -v -g -r MEM_LIVE igprof.*.gz | sqlite3 web/data/igreport_total.sql3
    else
	echo "32 bit leak check execute"
	Reco_trf.py inputRDOFile=root://eosatlas.cern.ch//eos/atlas/atlascerngroupdisk/phys-rig/mc10_7TeV.105200.T1_McAtNlo_Jimmy.digit.RDO.e598_s933_s946_d366_tid197091_00/RDO.197091._000001.pool.root.1 outputESDFile=T1_McAtNlo_Jimmy.ESD.pool.root outputAODFile=T1_McAtNlo_Jimmy.AOD.pool.root outputHISTFile=T1_McAtNlo_Jimmy.HIST.root maxEvents=500 autoConfiguration=everything --athenaopts="--leak-check-execute"
    fi
    RUN=1
fi

if [[ $1 == "data11_7TeV_memleak" ]]; then
    echo "running $1 with CMTCONFIG $CMTCONFIG"
    if [[ $CMTCONFIG = x86_64-* ]]; then
	echo "64 bit : igprof"
        Reco_trf.py inputBSFile=/afs/cern.ch/atlas/project/rig/referencefiles/dataStreams_high_mu/high_mu-data11_7TeV.00179725.physics_JetTauEtmiss.merge.RAW._lb0021.data maxEvents=3 autoConfiguration=everything outputESDFile=myESD.pool.root outputNTUP_MUONCALIBFile=muonCalib.root outputNTUP_TRKVALIDFile=trkValidNtup.root outputHISTFile=myMergedMonitoring.root outputTAGFile=myTAG.root --athenaopts="--config-only=job.cfg --keep-configuration"
	export LD_LIBRARY_PATH=${LD_LIBRARY_PATH}:/afs/cern.ch/atlas/offline/external/igprof/5.9.2/${CMTCONFIG}/lib
	export PATH=${PATH}:/afs/cern.ch/atlas/offline/external/igprof/5.9.2/${CMTCONFIG}/bin
	igprof -mp -z -t python athena.py job.cfg.pkl --stdcmalloc
	mkdir -p web/data
	cp /afs/cern.ch/atlas/offline/external/igprof/5.9.2/${CMTCONFIG}/bin/igprof-navigator web
	igprof-analyse --sqlite -d -v -g -r MEM_LIVE igprof.*.gz | sqlite3 web/data/igreport_total.sql3
    else
	echo "32 bit : leak check execute"
	Reco_trf.py inputBSFile=/afs/cern.ch/atlas/project/rig/referencefiles/dataStreams_high_mu/high_mu-data11_7TeV.00179725.physics_JetTauEtmiss.merge.RAW._lb0021.data maxEvents=300 autoConfiguration=everything outputESDFile=myESD.pool.root outputNTUP_MUONCALIBFile=muonCalib.root outputNTUP_TRKVALIDFile=trkValidNtup.root outputHISTFile=myMergedMonitoring.root outputTAGFile=myTAG.root --athenaopts="--leak-check-execute"
    fi
    RUN=1
fi

if [[ $1 == "data12_8TeV_memleak" ]]; then
    echo "running $1 with CMTCONFIG $CMTCONFIG"
    if [[ $CMTCONFIG = x86_64-* ]]; then
	echo "64 bit : igprof"
        Reco_trf.py inputBSFile=/afs/cern.ch/atlas/project/rig/referencefiles/dataStreams_high_mu/data12_8TeV/data12_8TeV.00201556.physics_JetTauEtmiss.merge.RAW._lb0423._SFO-1._0001.1 maxEvents=3 autoConfiguration=everything outputESDFile=myESD.pool.root outputNTUP_MUONCALIBFile=muonCalib.root outputNTUP_TRKVALIDFile=trkValidNtup.root outputHISTFile=myMergedMonitoring.root outputTAGFile=myTAG.root --athenaopts="--config-only=job.cfg --keep-configuration"
	export LD_LIBRARY_PATH=${LD_LIBRARY_PATH}:/afs/cern.ch/atlas/offline/external/igprof/5.9.2/${CMTCONFIG}/lib
	export PATH=${PATH}:/afs/cern.ch/atlas/offline/external/igprof/5.9.2/${CMTCONFIG}/bin
	igprof -mp -z -t python athena.py job.cfg.pkl --stdcmalloc
	mkdir -p web/data
	cp /afs/cern.ch/atlas/offline/external/igprof/5.9.2/${CMTCONFIG}/bin/igprof-navigator web
	igprof-analyse --sqlite -d -v -g -r MEM_LIVE igprof.*.gz | sqlite3 web/data/igreport_total.sql3
    else
	echo "32 bit : leak check execute"
	Reco_trf.py inputBSFile=/afs/cern.ch/atlas/project/rig/referencefiles/dataStreams_high_mu/data12_8TeV/data12_8TeV.00201556.physics_JetTauEtmiss.merge.RAW._lb0423._SFO-1._0001.1 maxEvents=300 autoConfiguration=everything outputESDFile=myESD.pool.root outputNTUP_MUONCALIBFile=muonCalib.root outputNTUP_TRKVALIDFile=trkValidNtup.root outputHISTFile=myMergedMonitoring.root outputTAGFile=myTAG.root --athenaopts="--leak-check-execute"
    fi
    RUN=1
fi

if [ $RUN -eq 0 ]; then
    echo "don't understand the parameter $1"
fi
