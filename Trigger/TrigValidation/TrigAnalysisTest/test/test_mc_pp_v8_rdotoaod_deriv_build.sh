#!/bin/bash

# art-description: Trigger MC pp v8 RDO to AOD test and then derivation in 21.2
# art-type: build
# art-include: 21.3/Athena
# Skipping art-output which has no effect for build tests.
# If you create a grid version, check art-output in existing grid tests.

export NAME="mc_pp_v8_rdotoaod_deriv_build"
export COST_MONITORING="False"
export TEST="TrigAnalysisTest"
export MENU="MC_pp_v8"
export EVENTS="10"
export JOBOPTION="TrigAnalysisTest/testAthenaTrigRDOtoAOD.py"

source exec_athena_art_trigger_validation.sh

export JOB_LOG="athena.log"
# add log for merging
echo "### ${JOB_LOG} ###" > athena.merged.log
cat ${JOB_LOG} >> athena.merged.log

export JOB_LOG="athena.AODtoDERIV.log"

# run the second transform
echo "Running AOD->DERIV with Reco_tf command:"
(set -x
Reco_tf.py \
--asetup="AthDerivation,21.2,latest,centos7" \
--inputAODFile AOD.pool.root \
--outputDAODFile DAOD.root \
--reductionConf JETM6 \
--preExec "default:from BTagging.BTaggingFlags import BTaggingFlags;BTaggingFlags.CalibrationTag = \"BTagCalibRUN12-08-49\";from AthenaMP.AthenaMPFlags import jobproperties as ampjp;ampjp.AthenaMPFlags.UseSharedWriter=True;import AthenaPoolCnvSvc.AthenaPool;ServiceMgr.AthenaPoolCnvSvc.OutputMetadataContainer=\"MetaData\";from AthenaCommon.AlgSequence import AlgSequence;topSequence = AlgSequence ();topSequence += CfgMgr.xAODMaker__DynVarFixerAlg(\"BTaggingELFixer\", Containers = [\"BTagging_AntiKt4EMTopoAux.\" ] );" \
>${JOB_LOG} 2>&1
) 2>&1

export ATH_RETURN=$?
echo "art-result: ${ATH_RETURN} ${JOB_LOG%.*}"

# add log for merging
echo "### ${JOB_LOG} ###" >> athena.merged.log
cat ${JOB_LOG} >> athena.merged.log


source exec_art_triggertest_post.sh





