#!/bin/bash

# art-description: Trigger Event Data Model test on AOD
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

export NAME="mc_NTUP_PHYSVAL_grid"
export JOB_LOG="athena.log"
export TEST="TrigAnalysisTest"
export NEVT=10
export DS='/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/TrigAnalysisTest/AthenaTrigAOD_TrigEDMandTDTCheck_MC_pp_v7_chain/AOD.pool.root'

Reco_tf.py --inputAODFile=${DS}  --preExec "all:from InDetPhysValMonitoring.InDetPhysValJobProperties import InDetPhysValFlags; InDetPhysValFlags.doValidateTightPrimaryTracks.set_Value_and_Lock(True);" --skipEvents="0" --maxEvents=${NEVT} --valid="True"  --jobNumber="1" --validationFlags doExample,doMET,doPFlow,doEgamma,doInDet,doTau,doJet,doBtag,doZee,doMuon,doTrigEgamma,doTrigBphys,doTrigMET,doTrigJet,doTrigMuon,doTrigHLTResult,doTrigCalo,doTrigMinBias,doTrigTau,doTrigIDtrk,doTrigBjet --outputNTUP_PHYSVALFile="NTUP_PHYSVAL.pool.root" > output.log 

echo "art-result: $? PhysVal"



# We need to place a reference file on cvmfs
#physval_make_web_display.py --ratio  --reffile Ref:PHYSVAL_all.root --title Test PHYSVAL_all.root --outdir PHYSVAL_WEB
mkdir PHYSVAL_WEB
#domains="BTag Ele Gam IDTrk Jets Muons MET PFlow TopoClusters"
###domains="BTag Electron IDPerformanceMon Jets MET Muons PFlow Photon Summary Tau TopoClusters"
###doTrigEgamma,doTrigBphys,doTrigMET,doTrigJet,doTrigMuon,doTrigHLTResult,doTrigCalo,doTrigMinBias,doTrigTau,doTrigIDtrk,doTrigBjet
#for slice in ${domains}
#do 
#      physval_make_web_display.py --ratio --reffile Ref:/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/DerivationFrameworkPhysicsValidation/PHYSVAL_all_reference.root --title Test PHYSVAL_all.root --outdir PHYSVAL_WEB/${slice} --startpath ${slice}
      physval_make_web_display.py --ratio --drawopt HISTPE --refdrawopt HIST --reffile Ref:NTUP_PHYSVAL.pool.root --title Test  --outdir PHYSVAL_WEB --startpath run_1/HLT NTUP_PHYSVAL.pool.root
#done
echo  "art-result: $? web"



#ArtPackage=$1
#ArtJobName=$2
#art.py compare grid --entries 10 ${ArtPackage} ${ArtJobName}
#echo "art-result: $? Diff"
