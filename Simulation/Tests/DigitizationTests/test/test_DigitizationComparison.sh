#!/bin/sh
#
# art-description: Run a digitization example to compare configuration between ConfGetter and the new ComponentAccumulator approch.
# art-type: grid
# art-include: master/Athena
# art-output: mc16a_ttbar.CG.RDO.pool.root
# art-output: mc16a_ttbar.CA.RDO.pool.root
# art-output: log.*

DigitizationComparisonNew_test.py
rc=$?
echo  "art-result: $rc CAdigi"

_preExec="\
all:from AthenaCommon.BeamFlags import jobproperties;\
jobproperties.Beam.numberOfCollisions.set_Value_and_Lock(20.0);\
from LArROD.LArRODFlags import larRODFlags;\
larRODFlags.NumberOfCollisions.set_Value_and_Lock(20);\
larRODFlags.nSamples.set_Value_and_Lock(4);\
larRODFlags.doOFCPileupOptimization.set_Value_and_Lock(True);\
larRODFlags.firstSample.set_Value_and_Lock(0);\
larRODFlags.useHighestGainAutoCorr.set_Value_and_Lock(True)\
"

_postExec="\
default:condSeq.LArAutoCorrTotalCondAlg.deltaBunch=1;\
job+=CfgMgr.JobOptsDumperAlg(FileName=\"OverlayLegacyConfig.txt\");\
from AthenaCommon.OldStyleConfig import Service;\
Service('StoreGateSvc').Dump=True;\
from AthenaCommon.ConfigurationShelve import saveToAscii;\
saveToAscii('Oldstyleoutput.txt');\
from AthenaCommon.AlgSequence import AthSequencer;\
print(AthSequencer('AthCondSeq'))\
"

_preInclude="\
HITtoRDO:Digitization/ForceUseOfAlgorithms.py\
"

rc2=-9999
if [ $rc -eq 0 ]
then
    # config only version
    Digi_tf.py \
    --inputHITSFile /cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/Tier0ChainTests/valid1.410000.PowhegPythiaEvtGen_P2012_ttbar_hdamp172p5_nonallhad.simul.HITS.e4993_s3091/HITS.10504490._000425.pool.root.1 \
    --conditionsTag default:OFLCOND-MC16-SDR-16 \
    --digiSeedOffset1 170 \
    --digiSeedOffset2 170 \
    --geometryVersion default:ATLAS-R2-2016-01-00-01 \
    --DataRunNumber 284500 \
    --outputRDOFile mc16a_ttbar.CG.RDO.pool.root \
    --postExec "$_postExec" \
    --postInclude "default:PyJobTransforms/UseFrontier.py" \
    --preExec "$_preExec" \
    --preInclude "$_preInclude" \
    --skipEvents 0 \
    --maxEvents 20 \
    --athenaopts '"--config-only=DigitizationConfigCG.pkl"'
    # config and run version
    Digi_tf.py \
    --inputHITSFile /cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/Tier0ChainTests/valid1.410000.PowhegPythiaEvtGen_P2012_ttbar_hdamp172p5_nonallhad.simul.HITS.e4993_s3091/HITS.10504490._000425.pool.root.1 \
    --conditionsTag default:OFLCOND-MC16-SDR-16 \
    --digiSeedOffset1 170 \
    --digiSeedOffset2 170 \
    --geometryVersion default:ATLAS-R2-2016-01-00-01 \
    --DataRunNumber 284500 \
    --outputRDOFile mc16a_ttbar.CG.RDO.pool.root \
    --postExec "$_postExec" \
    --postInclude "default:PyJobTransforms/UseFrontier.py" \
    --preExec "$_preExec" \
    --preInclude "$_preInclude" \
    --skipEvents 0 \
    --maxEvents 20
    rc2=$?
fi

echo  "art-result: $rc2 STdigi"

rc3=-9999
if [ $rc2 -eq 0 ]
then
    acmd.py diff-root mc16a_ttbar.CG.RDO.pool.root mc16a_ttbar.CA.RDO.pool.root --mode=semi-detailed --order-trees --ignore-leaves RecoTimingObj_p1_HITStoRDO_timings index_ref --error-mode resilient
    rc3=$?
fi

echo  "art-result: $rc3 comparison"
