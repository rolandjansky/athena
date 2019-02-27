#!/bin/bash

# art-description: Test of Trigger NTUP_PHYSVAL 
# art-type: grid
# art-include: 21.3/Athena
# art-include: master/Athena
# art-include: master/AthenaP1
# art-output: *check*
# art-output: *.log
# art-output: log.*
# art-output: *.root
# art-output: *.pmon.gz
# art-output: TotalEventsProcessed.txt
# art-output: PHYSVAL_WEB

export NAME="mc_ntup_physval_grid"
export JOB_LOG="athena.log"
export TEST="TrigAnalysisTest"
export NEVT=10
export DS='/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/TrigAnalysisTest/AthenaTrigAOD_TrigEDMandTDTCheck_MC_pp_v7_chain/AOD.pool.root'

Reco_tf.py --inputAODFile=${DS}  --preExec "all:from InDetPhysValMonitoring.InDetPhysValJobProperties import InDetPhysValFlags; InDetPhysValFlags.doValidateTightPrimaryTracks.set_Value_and_Lock(True);" --skipEvents="0" --maxEvents=${NEVT} --valid="True"  --jobNumber="1" --validationFlags doExample,doMET,doPFlow,doEgamma,doInDet,doTau,doJet,doBtag,doZee,doMuon,doTrigEgamma,doTrigBphys,doTrigMET,doTrigJet,doTrigMuon,doTrigHLTResult,doTrigCalo,doTrigMinBias,doTrigTau,doTrigIDtrk,doTrigBjet --outputNTUP_PHYSVALFile="NTUP_PHYSVAL.pool.root" > output.log 

echo "art-result: $? PhysVal"

ArtPackage=$1
ArtJobName=$2
#
echo " ArtPackage = ${ArtPackage}"
echo " ArtJobName = ${ArtJobName}"
#
art.py download ${ArtPackage} ${ArtJobName}
echo "art-result: $? ref-download"

CMD="find . -maxdepth 1 -name "ref-*""
refdir=$(eval "${CMD}")
echo "Reference directory"
echo ${refdir}


# make webdisplay 
mkdir PHYSVAL_WEB
physval_make_web_display.py --ratio --drawopt HISTPE --refdrawopt HIST --reffile Ref:${refdir}/NTUP_PHYSVAL.pool.root --title Test  --outdir PHYSVAL_WEB --startpath run_1/HLT NTUP_PHYSVAL.pool.root

echo  "art-result: $? web"



