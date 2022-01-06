#!/bin/bash
#
# art-description: Run digitization combining a heavy ion sample produced with MC15 using 2015 geometry and conditions with Zmumu events
# art-include: 21.0/Athena
# art-include: 21.3/Athena
# art-include: 21.9/Athena
# art-include: master/Athena
# art-type: grid
# art-output: mc15_2015_heavyIon.RDO.pool.root

DigiOutFileName="mc15_2015_heavyIon.RDO.pool.root"
ZmumuHITSFileName="/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/DigitizationTests/mc15_pPb8TeV.361107.PowhegPythia8EvtGen_AZNLOCTEQ6L1_Zmumu.simul.HITS.e5367_s3164.HITS.11308894._000005.pool.root.1"
HeavyIonHITSFileName="/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/DigitizationTests/mc15_pPb8TeV.420112.Hijing_MinBiasDiff_pPb_8TeV.merge.HITS.e5328_s3148_s3153.HITS.11232927._000249.pool.root.1"

Digi_tf.py \
--inputHITSFile ${ZmumuHITSFileName} \
--inputCavernHitsFile ${HeavyIonHITSFileName} \
--outputRDOFile ${DigiOutFileName} \
--maxEvents 10 \
--skipEvents 0 \
--numberOfCavernBkg 1 \
--preExec 'all:rec.doHIP.set_Value_and_Lock(True);from AthenaCommon.BeamFlags import jobproperties;jobproperties.Beam.numberOfCollisions.set_Value_and_Lock(0.0);jobproperties.Beam.bunchSpacing.set_Value_and_Lock(25);' 'HITtoRDO:from Digitization.DigitizationFlags import digitizationFlags;digitizationFlags.overrideMetadata+=["PhysicsList"]' \
--preInclude 'all:LArConfiguration/LArConfigRun2Old_NoPileup.py' 'HITtoRDO:Digitization/ForceUseOfPileUpTools.py' \
--postExec 'all:CfgMgr.MessageSvc().setError+=["HepMcParticleLink"]' 'HITtoRDO:from AthenaCommon import CfgGetter;mergeMcEventCollTool=CfgGetter.getPublicTool("MergeMcEventCollTool").DoSlimming=False' \
--postInclude 'all:PyJobTransforms/UseFrontier.py,SimulationJobOptions/postInclude.HijingPars.py,PixelConditionsAlgorithms/postInclude.UsePixelModuleLevelMask.py' \
--geometryVersion ATLAS-R2-2015-03-01-00 \
--conditionsTag all:OFLCOND-MC15c-SDR-15 \
--DataRunNumber 295000 \
--pileupInitialBunch 0 \
--pileupFinalBunch 0 \
--bunchSpacing 100 \
--imf False

rc=$?
status=$rc
echo "art-result: $rc Digi_tf.py"

exit $status
