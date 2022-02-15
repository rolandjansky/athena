#!/bin/bash
#
# art-description: Run a digitization example to compare configuration between ConfGetter and the new ComponentAccumulator approach.
# art-type: grid
# art-include: master/Athena
# art-output: mc21a.ttbar.nopileup.CG.RDO.pool.root
# art-output: mc21a.ttbar.nopileup.CA.RDO.pool.root
# art-output: log.*
# art-output: legacy.*
# art-output: DigiPUConfig*

Events=3
DigiOutFileNameCG="mc21a.ttbar.nopileup.CG.RDO.pool.root"
DigiOutFileNameCA="mc21a.ttbar.nopileup.CA.RDO.pool.root"
HSHitsFile="/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/DigitizationTests/NSW/mc21_13p6TeV.601229.PhPy8EG_A14_ttbar_hdamp258p75_SingleLep.simul.HITS.e8357_e7400_s3775/HITS.27679639._068389.pool.root.1"


# config only
Digi_tf.py \
--DataRunNumber 330000 \
--conditionsTag default:OFLCOND-MC16-SDR-RUN3-05 \
--digiSeedOffset1 170 --digiSeedOffset2 170 \
--geometryVersion default:ATLAS-R3S-2021-02-00-00 \
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
--conditionsTag default:OFLCOND-MC16-SDR-RUN3-05 \
--digiSeedOffset1 170 --digiSeedOffset2 170 \
--geometryVersion default:ATLAS-R3S-2021-02-00-00 \
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
    --conditionsTag default:OFLCOND-MC16-SDR-RUN3-05 \
    --digiSeedOffset1 170 --digiSeedOffset2 170 \
    --geometryVersion default:ATLAS-R3S-2021-02-00-00 \
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
if [ $rc -eq 0 ] && [ $rc2 -eq 0 ]
then
    acmd.py diff-root ${DigiOutFileNameCG} ${DigiOutFileNameCA} \
        --mode=semi-detailed --error-mode resilient --order-trees \
        --ignore-leaves RecoTimingObj_p1_HITStoRDO_timings index_ref
    rc3=$?
    status=$rc3
fi
echo "art-result: $rc3 OLDvsCA"

# get reference directory
source DigitizationCheckReferenceLocation.sh
echo "Reference set being used: ${DigitizationTestsVersion}"

rc4=-9999
if [[ $rc -eq 0 ]]
then
    # Do reference comparisons
    art.py compare ref --mode=semi-detailed --no-diff-meta "$DigiOutFileNameCG" "/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/DigitizationTests/ReferenceFiles/$DigitizationTestsVersion/$CMTCONFIG/$DigiOutFileNameCG"
    rc4=$?
    status=$rc4
fi
echo "art-result: $rc1 OLDvsFixedRef"

rc5=-9999
if [[ $rc -eq 0 ]]
then
    checkFile "$DigiOutFileNameCG"
    rc5=$?
    status=$rc5
fi
echo "art-result: $rc5 checkFile"

rc6=-9999
if [[ $rc -eq 0 ]]
then
    art.py compare grid --entries 10 "$1" "$2" --mode=semi-detailed --file="$DigiOutFileNameCG"
    rc6=$?
    status=$rc6
fi
echo "art-result: $rc6 regression"

exit $status
