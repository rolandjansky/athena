#!/bin/bash

# art-description: Test of Trigger NTUP_PHYSVAL 
# art-type: grid
# art-include: 21.0/Athena
# art-include: 21.3/Athena
# art-include: master/Athena
# art-output: *.txt
# art-output: *.log
# art-output: log.*
# art-output: *.out
# art-output: *.err
# art-output: *.log.tar.gz
# art-output: *.new
# art-output: *.json
# art-output: *.root
# art-output: *.pmon.gz
# art-output: *perfmon*
# art-output: *.check*
# art-output: HLTconfig*.xml
# art-output: L1Topoconfig*.xml
# art-output: LVL1config*.xml
# art-output: PHYSVAL_WEB
# art-html: PHYSVAL_WEB

unset ATHENA_NPROC_NUM

export NAME="mc_ntup_physval_grid"
export JOB_LOG="athena.log"
export TEST="TrigAnalysisTest"
export NEVT=100
export DS='/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/TrigAnalysisTest/AthenaTrigAOD_master/AOD.pool.root'

Reco_tf.py --inputAODFile=${DS}  --skipEvents="0" --maxEvents=${NEVT} --valid="True"  --jobNumber="1" --validationFlags doTrigEgamma,doTrigBphys,doTrigMET,doTrigJet,doTrigMuon,doTrigHLTResult,doTrigCalo,doTrigMinBias,doTrigTau,doTrigIDtrk,doTrigBjet --outputNTUP_PHYSVALFile="NTUP_PHYSVAL.pool.root" > output.log 

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
#physval_make_web_display.py --ratio --drawopt HISTPE --refdrawopt HIST --reffile Ref:${refdir}/NTUP_PHYSVAL.pool.root --title Test  --outdir PHYSVAL_WEB --startpath run_1/HLT NTUP_PHYSVAL.pool.root


domains="TauMon MuonMon IDMon JetMon BphysMon HLTCaloESD ResultMon BjetMon METMon MinBiasMon Egamma"
for slice in ${domains}
do 
  physval_make_web_display.py --ratio --drawopt HISTPE --refdrawopt HIST --reffile Ref:${refdir}/NTUP_PHYSVAL.pool.root --title Test  --outdir PHYSVAL_WEB/${slice} --startpath run_1/HLT/${slice} NTUP_PHYSVAL.pool.root
done

echo  "art-result: $? web"


echo "ls PHYSVAL_WEB"
ls PHYSVAL_WEB

for slice in ${domains}
do
  grep Red PHYSVAL_WEB/${slice}/index.html > ${slice}_redhists.txt
  SIZE=( $(ls -nl ${slice}_redhists.txt | awk '{print $5}') )
  echo "art-result: ${SIZE}  ${slice}"
done



