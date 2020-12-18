#!/bin/sh
# art-description: ART test job HITS to AOD
# art-type: grid
# art-include: master/Athena
# art-memory: 4096
# art-output: *.root
# art-output: dcube
# art-html: dcube

ART_PATH="/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/Tier0ChainTests/"
ART_FILE="mc16_13TeV.410470.PhPy8EG_A14_ttbar_hdamp258p75_nonallhad.simul.HITS.e6337_s3126/HITS.12860054._032508.pool.root.1"
HITSFile="${ART_PATH}${ART_FILE}"
echo "Input HITS file for    : ${HITSFile}"

Nevents=3000
echo "Number of test events  : ${Nevents}"

ART_AOD="nightly_met.AOD.pool.root"
echo "Output AOD file        : ${ART_AOD}"

ART_Validation="nightly_met.PHYSVAL.root"
echo "Output Validation file : ${ART_Validation}"

echo "Submitting Reconstruction ..."

Reco_tf.py \
    --maxEvents -1 \
    --inputHITSFile=${HITSFile} \
    --outputAODFile=${ART_AOD} \
    --outputNTUP_PHYSVALFile ${ART_Validation} \
    --valid=True \
    --validationFlags 'doInDet,doMET' \
    --autoConfiguration everything \
    --preExec 'from RecExConfig.RecFlags import rec;rec.doTrigger=False'

rc=$?
echo "art-result: $rc Reco"

rc2=-9999
if [ ${rc} -eq 0 ]
then
  # Histogram comparison with DCube
  $ATLAS_LOCAL_ROOT/dcube/current/DCubeClient/python/dcube.py \
     -p -x dcube \
     -c /cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/JetValidation/DCUBE/met.xml \
     -r /cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/JetValidation/reference/nightly_met.PHYSVAL.root \
     nightly_met.PHYSVAL.root
  rc2=$?
fi
echo "art-result: ${rc2} plot"
