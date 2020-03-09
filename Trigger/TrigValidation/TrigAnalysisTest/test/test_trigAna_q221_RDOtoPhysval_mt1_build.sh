#!/bin/bash

# art-description: Test of transform RDO->RDO_TRIG->ESD->AOD with threads=1 followed by AOD->NTUP_PHYSVAL with serial athena
# art-type: build
# art-include: master/Athena
# Skipping art-output which has no effect for build tests.
# If you create a grid version, check art-output in existing grid tests.

export NAME="trigAna_q221_RDOtoPhysval_mt1_build"
export TEST="TrigAnalysisTest"
export INPUT="/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/TriggerTest/valid1.410000.PowhegPythiaEvtGen_P2012_ttbar_hdamp172p5_nonallhad.merge.RDO.e4993_s3214_r11315/RDO.17533168._000001.pool.root.1"
export EVENTS=20
export THREADS=1
export JOB_LOG="athena.RDOtoAOD.log"
export VALIDATIONFLAGS="doTrigEgamma,doTrigBphys,doTrigMET,doTrigJet,doTrigMuon,doTrigHLTResult,doTrigCalo,doTrigMinBias,doTrigTau,doTrigIDtrk,doTrigBjet"

# run the first transform
echo "Running RDO->RDO_TRIG->ESD->AOD with Reco_tf command:"
(set -x
Reco_tf.py \
--AMI=q221 \
--imf=True \
--athenaopts="--threads=${THREADS}" \
--maxEvents=${EVENTS} \
--inputRDOFile=${INPUT} \
--outputAODFile=AOD.pool.root \
--steering="doRDO_TRIG" \
--valid=True \
--postInclude="TriggerTest/disableChronoStatSvcPrintout.py" \
>${JOB_LOG} 2>&1
) 2>&1

export ATH_RETURN=$?
echo "art-result: ${ATH_RETURN} ${JOB_LOG%.*}"

# add log for merging
echo "### ${JOB_LOG} ###" > athena.merged.log
cat ${JOB_LOG} >> athena.merged.log
export JOB_LOG="athena.PhysVal.log"

# run the second transform
echo "Running AOD->NTUP_PHYSVAL with Reco_tf command:"
(set -x
Reco_tf.py \
--AMI=q221 \
--imf=True \
--maxEvents=${EVENTS} \
--inputAODFile=AOD.pool.root \
--outputNTUP_PHYSVALFile=NTUP_PHYSVAL.pool.root \
--validationFlags="${VALIDATIONFLAGS}" \
--preExec="TriggerFlags.EDMDecodingVersion.set_Value_and_Lock(3)" \
--postInclude="TriggerTest/disableChronoStatSvcPrintout.py" \
>${JOB_LOG} 2>&1
) 2>&1

export ATH_RETURN=$?
echo "art-result: ${ATH_RETURN} ${JOB_LOG%.*}"

# merge transform logs for post-processing and prepare for RegTest comparison
source exec_art_triganalysistest_merge_trf_logs.sh "RDOtoRDOTrigger RAWtoESD ESDtoAOD PhysicsValidation"

# use TrigUpgradeTest post-processing script
source exec_TrigUpgradeTest_art_post.sh

# zip the merged log (can be large and duplicates information)
tar -czf athena.merged.log.tar.gz athena.merged.log
rm -f athena.merged.log
