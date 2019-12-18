#!/bin/bash

# art-description: Test of Trigger FTK RDO->RDO_FTK 
# art-type: grid
# art-include: 21.3/Athena
# art-include: 21.0/Athena
# art-include: master/Athena
# art-output: *check*
# art-output: *.log
# art-output: log.RAW*
# art-output: log.RDO*
# art-output: *.root
# art-output: *.pmon.gz
# art-output: TotalEventsProcessed.txt


export NAME="mc_rdo_to_rdoftk_grid"
export JOB_LOG="athena.log"
export TEST="TrigAnalysisTest"
export NEVT=100
#export DS='/eos/atlas/atlascerngroupdisk/data-art/grid-input/TriggerTest/valid1.410000.PowhegPythiaEvtGen_P2012_ttbar_hdamp172p5_nonallhad.merge.RDO.e4993_s3214_r11234/RDO.17071942._000179.pool.root.1'
export DS='/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/TriggerTest/valid1.410000.PowhegPythiaEvtGen_P2012_ttbar_hdamp172p5_nonallhad.merge.RDO.e4993_s3214_r11234/RDO.17071942._000179.pool.root.1'

TrigFTKTM64SM1Un_tf.py --inputRDOFile=${DS} --runNumber="410000"  --outputRDO_FTKFile="RDO_FTK.pool.root" --CachedBank="True" --Clustering="False" --SctClustering="0" --ConstantsDir="/cvmfs/atlas.cern.ch/repo/sw/database/GroupData/FTK" --FTKSetupTag="64Tower2018.v1.ECFix" --FitConstantsVersion="ftk.64tower.2016.mc16.v2" --PatternsVersion="ftk.64tower.2016.mc16.v2"  --maxEvents=${NEVT} --postExec "RDOFTKCreator:from AthenaCommon.AlgSequence import AlgSequence;AlgSequence().FTK_RDO_CreatorAlgo.TrainingBeamspotX=-0.5;AlgSequence().FTK_RDO_CreatorAlgo.TrainingBeamspotY=-0.5;AlgSequence().FTK_RDO_CreatorAlgo.ReverseIBLlocX=True" --preExec "RAWtoESD:rec.doCalo=False;rec.doInDet=True;rec.doMuon=False;rec.doJetMissingETTag=False;rec.doEgamma=False;rec.doMuonCombined=False;rec.doTau=False;rec.doTrigger=False;rec.UserAlgs=[\"FastTrackSimWrap/FastTrackSimRegionalWrap_64TowersIBL3D_HWModeID2_jobOptions.py\"]" --skipEvents="0" > ${JOB_LOG}

echo "art-result: $? athena"


echo $(date "+%FT%H:%M %Z")"     Running checklog"
timeout 5m check_log.py --errors --config checklogTriggerTest.conf --showexcludestats ${JOB_LOG} 2>&1 | tee checklog.log

echo "art-result: ${PIPESTATUS[0]} CheckLog"

