#!/bin/bash
#
# art-description: Run a digitization example to compare configuration between ConfGetter and the new ComponentAccumulator approach.
# art-type: grid
# art-include: master/Athena
# art-output: mc21a_Zmumu.CG.RDO.pool.root
# art-output: mc21a_Zmumu.CA.RDO.pool.root
# art-output: log.*
# art-output: legacy.*
# art-output: DigiPUConfig*

Events=3
DigiOutFileNameCG="mc21a_Zmumu.CG.RDO.pool.root"
DigiOutFileNameCA="mc21a_Zmumu.CA.RDO.pool.root"
HSHitsFile="/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/DigitizationTests/NSW/mc16_13TeV.361107.PowhegPythia8EvtGen_AZNLOCTEQ6L1_Zmumu.R3.2021-06-27.HITS.pool.root"


# config only
Digi_tf.py \
--DataRunNumber 330000 \
--conditionsTag default:OFLCOND-MC16-SDR-RUN3-02 \
--digiSeedOffset1 170 --digiSeedOffset2 170 \
--geometryVersion default:ATLAS-R3-2021-01-00-02 \
--inputHITSFile ${HSHitsFile} \
--jobNumber 568 \
--maxEvents ${Events} \
--outputRDOFile ${DigiOutFileNameCG} \
--postInclude 'default:PyJobTransforms/UseFrontier.py' \
--preInclude 'all:Campaigns/MC21NoPileUp.py' \
--skipEvents 0 \
--athenaopts '"--config-only=DigiPUConfigCG.pkl"'

# full run
Digi_tf.py \
--DataRunNumber 330000 \
--conditionsTag default:OFLCOND-MC16-SDR-RUN3-02 \
--digiSeedOffset1 170 --digiSeedOffset2 170 \
--geometryVersion default:ATLAS-R3-2021-01-00-02 \
--inputHITSFile ${HSHitsFile} \
--jobNumber 568 \
--maxEvents ${Events} \
--outputRDOFile ${DigiOutFileNameCG} \
--postExec 'HITtoRDO:job+=CfgMgr.JobOptsDumperAlg(FileName="DigiPUConfigCG.txt")' \
--postInclude 'default:PyJobTransforms/UseFrontier.py' \
--preInclude 'all:Campaigns/MC21NoPileUp.py' \
--skipEvents 0

rc=$?
status=$rc
echo "art-result: $rc digiOLD"
mv runargs.HITtoRDO.py runargs.legacy.HITtoRDO.py
mv log.HITtoRDO legacy.HITtoRDO

rc2=-9999
if [[ $rc -eq 0 ]]
then
    Digi_tf.py \
    --CA \
    --DataRunNumber 330000 \
    --conditionsTag default:OFLCOND-MC16-SDR-RUN3-02 \
    --digiSeedOffset1 170 --digiSeedOffset2 170 \
    --geometryVersion default:ATLAS-R3-2021-01-00-02 \
    --inputHITSFile ${HSHitsFile} \
    --jobNumber 568 \
    --maxEvents ${Events} \
    --outputRDOFile ${DigiOutFileNameCA} \
    --postInclude 'PyJobTransforms.UseFrontier' 'HITtoRDO:Digitization.DigitizationSteering.DigitizationTestingPostInclude' \
    --preInclude 'HITtoRDO:Campaigns.MC21NoPileUp' \
    --skipEvents 0

    rc2=$?
    status=$rc2
fi
echo "art-result: $rc2 digiCA"

rc3=-9999
if [[ $rc2 -eq 0 ]]
then
    acmd.py diff-root ${DigiOutFileNameCG} ${DigiOutFileNameCA} \
        --mode=semi-detailed --error-mode resilient --order-trees \
        --ignore-leaves RecoTimingObj_p1_HITStoRDO_timings index_ref
    rc3=$?
    status=$rc3
fi
echo "art-result: $rc3 OLDvsCA"

exit $status
