#!/bin/bash

# art-description: Test running HITS->RDO in master, then RDO->RDO_TRIG in 21.0, then RDO_TRIG->AOD in master
# art-type: build
# art-include: master/Athena
# art-output: log.*
# art-output: *.log
# art-output: *.txt
# art-output: *.root
# art-output: runargs.*
# art-output: runwrapper.*

export NAME="trigAna_HITtoAOD_trigRel21_build"
export TEST="TrigAnalysisTest"
export INPUT="/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/TrigAnalysisTest/valid1.410000.PowhegPythiaEvtGen_P2012_ttbar_hdamp172p5_nonallhad.simul.HITS.e4993_s3091_tid10504490_00/HITS.10504490._000001.pool.root.1"
export INPUT_MB_LOW="/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/TrigAnalysisTest/mc16_13TeV.361238.Pythia8EvtGen_A3NNPDF23LO_minbias_inelastic_low.simul.HITS.e4981_s3087_s3111_tid10701323_00/HITS.10701323._000001.pool.root.1"
export INPUT_MB_HIGH="/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/TrigAnalysisTest/mc16_13TeV.361239.Pythia8EvtGen_A3NNPDF23LO_minbias_inelastic_high.simul.HITS.e4981_s3087_s3111_tid10701335_00/HITS.10701335._000011.pool.root.1"
export EVENTS=20
export JOB_LOG="athena.HITtoRDO.log"

# run the first transform (not sure if all these options are needed)
echo "Running HITS->RDO with Reco_tf command:"
(set -x
Reco_tf.py \
--imf=True \
--maxEvents=${EVENTS} \
--inputHITSFile=${INPUT} \
--inputLowPtMinbiasHitsFile=${INPUT_MB_LOW} \
--inputHighPtMinbiasHitsFile=${INPUT_MB_HIGH} \
--outputRDOFile=RDO.pool.root \
--preExec "HITtoRDO:userRunLumiOverride={\"run\":300000, \"startmu\":40.0, \"endmu\":70.0, \"stepmu\":1.0, \"startlb\":1, \"timestamp\": 1500000000};ScaleTaskLength=0.4" \
--preInclude "HITtoRDO:Digitization/ForceUseOfPileUpTools.py,SimulationJobOptions/preInlcude.PileUpBunchTrainsMC16c_2017_Config1.py,RunDependentSimData/configLumi_muRange.py" \
--numberOfCavernBkg="0" \
--numberOfHighPtMinBias="0.15520183" \
--numberOfLowPtMinBias="59.3447981771" \
--pileupFinalBunch="6" \
--jobNumber="1" \
>${JOB_LOG} 2>&1
) 2>&1

export ATH_RETURN=$?
echo "art-result: ${ATH_RETURN} ${JOB_LOG%.*}"

# add log for merging
echo "### ${JOB_LOG} ###" > athena.merged.log
cat ${JOB_LOG} >> athena.merged.log
export JOB_LOG="athena.RDOtoRDOTrigger.log"

# run the second transform
echo "Running RDO->RDO_TRIG with Reco_tf command:"
(set -x
Reco_tf.py \
--asetup="RDOtoRDOTrigger:Athena,21.0,latest,slc6" \
--imf=True \
--maxEvents=${EVENTS} \
--inputRDOFile=RDO.pool.root \
--outputRDO_TRIGFile=RDO_TRIG.pool.root \
>${JOB_LOG} 2>&1
) 2>&1

export ATH_RETURN=$?
echo "art-result: ${ATH_RETURN} ${JOB_LOG%.*}"

# add log for merging
echo "### ${JOB_LOG} ###" > athena.merged.log
cat ${JOB_LOG} >> athena.merged.log
export JOB_LOG="athena.RDOTriggertoAOD.log"

# run the third transform
echo "Running RDO_TRIG->AOD with Reco_tf command:"
(set -x
Reco_tf.py \
--imf=True \
--maxEvents=${EVENTS} \
--inputRDO_TRIGFile=RDO_TRIG.pool.root \
--outputAODFile=AOD.pool.root \
>${JOB_LOG} 2>&1
) 2>&1

export ATH_RETURN=$?
echo "art-result: ${ATH_RETURN} ${JOB_LOG%.*}"

# merge transform logs for post-processing
echo "### ${JOB_LOG} ###" >> athena.merged.log
cat ${JOB_LOG} >> athena.merged.log
trfNames="HITtoRDO RDOtoRDOTrigger RAWtoESD ESDtoAOD"
for trf in ${trfNames}; do
  if [ -f log.${trf} ]; then
    echo "### log.${trf} ###" >> athena.merged.log
    cat log.${trf} >> athena.merged.log
  else
    echo "### WARNING: log.${trf} MISSING ###" >> athena.merged.log
  fi
done
export JOB_LOG="athena.merged.log"

# use TrigUpgradeTest post-processing script
source exec_TrigUpgradeTest_art_post.sh

# zip the merged log (can be large and duplicates information)
tar -czf athena.merged.log.tar.gz athena.merged.log
rm -f athena.merged.log
