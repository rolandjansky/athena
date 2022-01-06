#!/bin/bash
#
# art-description: Run a digitization example to compare configuration between ConfGetter and the new ComponentAccumulator approach.
# art-type: grid
# art-include: master/Athena
# art-output: mc21a_presampling.CG.RDO.pool.root
# art-output: mc21a_presampling.CA.RDO.pool.root
# art-output: log.*
# art-output: DigiPUConfig*

Events=3
DigiOutFileNameCG="mc21a_presampling.CG.RDO.pool.root"
DigiOutFileNameCA="mc21a_presampling.CA.RDO.pool.root"
HSHitsFile="/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/DigitizationTests/NSW/Neutrinos.R3.2021-06-27.HITS.pool.root"
HighPtMinbiasHitsFiles="/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/DigitizationTests/NSW/group.det-muon.high-pTMinBias.AsymRun3.rel2021_06_22.merge.HITS_EXT0/*"
LowPtMinbiasHitsFiles="/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/DigitizationTests/NSW/group.det-muon.low-pTMinBias.AsymRun3.rel2021_06_22.merge.HITS_EXT0/*"


# config only
Digi_tf.py \
--PileUpPresampling True \
--DataRunNumber 330000 \
--conditionsTag default:OFLCOND-MC16-SDR-RUN3-02 \
--digiSeedOffset1 170 --digiSeedOffset2 170 \
--digiSteeringConf 'StandardSignalOnlyTruth' \
--geometryVersion default:ATLAS-R3-2021-01-00-02 \
--inputHITSFile ${HSHitsFile} \
--inputHighPtMinbiasHitsFile ${HighPtMinbiasHitsFiles} \
--inputLowPtMinbiasHitsFile ${LowPtMinbiasHitsFiles} \
--jobNumber 568 \
--maxEvents ${Events} \
--outputRDOFile ${DigiOutFileNameCG} \
--postInclude 'default:PyJobTransforms/UseFrontier.py' \
--preInclude 'HITtoRDO:Campaigns/PileUpPresamplingMC21a.py' \
--skipEvents 0 \
--athenaopts '"--config-only=DigiPUConfigCG.pkl"'

# full run
Digi_tf.py \
--PileUpPresampling True \
--DataRunNumber 330000 \
--conditionsTag default:OFLCOND-MC16-SDR-RUN3-02 \
--digiSeedOffset1 170 --digiSeedOffset2 170 \
--digiSteeringConf 'StandardSignalOnlyTruth' \
--geometryVersion default:ATLAS-R3-2021-01-00-02 \
--inputHITSFile ${HSHitsFile} \
--inputHighPtMinbiasHitsFile ${HighPtMinbiasHitsFiles} \
--inputLowPtMinbiasHitsFile ${LowPtMinbiasHitsFiles} \
--jobNumber 568 \
--maxEvents ${Events} \
--outputRDOFile ${DigiOutFileNameCG} \
--postExec 'HITtoRDO:job+=CfgMgr.JobOptsDumperAlg(FileName="DigiPUConfigCG.txt")' \
--postInclude 'default:PyJobTransforms/UseFrontier.py' \
--preInclude 'HITtoRDO:Campaigns/PileUpPresamplingMC21a.py' \
--skipEvents 0

rc=$?
status=$rc
echo "art-result: $rc CGdigi"
mv runargs.HITtoRDO.py runargs.legacy.HITtoRDO.py 
mv log.HITtoRDO legacy.HITtoRDO

rc2=-9999
if [[ $rc -eq 0 ]]
then
    Digi_tf.py \
    --CA \
    --PileUpPresampling True \
    --DataRunNumber 330000 \
    --conditionsTag default:OFLCOND-MC16-SDR-RUN3-02 \
    --digiSeedOffset1 170 --digiSeedOffset2 170 \
    --digiSteeringConf 'StandardSignalOnlyTruth' \
    --geometryVersion default:ATLAS-R3-2021-01-00-02 \
    --inputHITSFile ${HSHitsFile} \
    --inputHighPtMinbiasHitsFile ${HighPtMinbiasHitsFiles} \
    --inputLowPtMinbiasHitsFile ${LowPtMinbiasHitsFiles} \
    --jobNumber 568 \
    --maxEvents ${Events} \
    --outputRDOFile ${DigiOutFileNameCA} \
    --postInclude 'PyJobTransforms.UseFrontier' 'HITtoRDO:Digitization.DigitizationSteering.DigitizationTestingPostInclude' \
    --preInclude 'HITtoRDO:Campaigns.MC21a' \
    --skipEvents 0

    rc2=$?
    status=$rc2
fi

echo "art-result: $rc2 CAdigi"

rc3=-9999
if [[ $rc2 -eq 0 ]]
then
    acmd.py diff-root ${DigiOutFileNameCG} ${DigiOutFileNameCA} \
        --mode=semi-detailed --error-mode resilient --order-trees \
        --ignore-leaves RecoTimingObj_p1_Bkg_HITStoRDO_timings index_ref
    rc3=$?
    status=$rc3
fi

echo "art-result: $rc3 comparison"

exit $status
