#!/bin/bash
#
# art-description: Run a digitization example to compare configuration between ConfGetter and the new ComponentAccumulator approach.
# art-type: grid
# art-include: master/Athena
# art-output: mc21a_presampling.CG.RDO.pool.root
# art-output: mc21a_presampling.CA.RDO.pool.root
# art-output: log.*
# art-output: legacy.*
# art-output: DigiPUConfig*

Events=25
DigiOutFileNameCG="mc21a_presampling.CG.RDO.pool.root"
DigiOutFileNameCA="mc21a_presampling.CA.RDO.pool.root"
HSHitsFile="/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/DigitizationTests/NSW/mc21_13p6TeV.159000.ParticleGenerator_nu_E50.simul.HITS.e3711/HITS.05259738._001019.pool.root.1"
HighPtMinbiasHitsFiles="/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/DigitizationTests/NSW/mc21_13p6TeV.800831.Py8EG_minbias_inelastic_highjetphotonlepton.merge.HITS.e8341_s3775_s3787/*"
LowPtMinbiasHitsFiles="/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/DigitizationTests/NSW/mc21_13p6TeV.900311.Epos_minbias_inelastic_lowjetphoton.merge.HITS.e8341_s3775_s3787/*"


# config only
Digi_tf.py \
--PileUpPresampling True \
--conditionsTag default:OFLCOND-MC21-SDR-RUN3-05 \
--digiSeedOffset1 170 --digiSeedOffset2 170 \
--digiSteeringConf 'StandardSignalOnlyTruth' \
--geometryVersion default:ATLAS-R3S-2021-02-00-00 \
--inputHITSFile ${HSHitsFile} \
--inputHighPtMinbiasHitsFile ${HighPtMinbiasHitsFiles} \
--inputLowPtMinbiasHitsFile ${LowPtMinbiasHitsFiles} \
--jobNumber 568 \
--maxEvents ${Events} \
--outputRDOFile ${DigiOutFileNameCG} \
--postInclude 'default:PyJobTransforms/UseFrontier.py' \
--preInclude 'HITtoRDO:Campaigns/PileUpPresamplingMC21aSingleBeamspot.py' \
--skipEvents 0 \
--athenaopts '"--config-only=DigiPUConfigCG.pkl"'

# full run
Digi_tf.py \
--PileUpPresampling True \
--conditionsTag default:OFLCOND-MC21-SDR-RUN3-05 \
--digiSeedOffset1 170 --digiSeedOffset2 170 \
--digiSteeringConf 'StandardSignalOnlyTruth' \
--geometryVersion default:ATLAS-R3S-2021-02-00-00 \
--inputHITSFile ${HSHitsFile} \
--inputHighPtMinbiasHitsFile ${HighPtMinbiasHitsFiles} \
--inputLowPtMinbiasHitsFile ${LowPtMinbiasHitsFiles} \
--jobNumber 568 \
--maxEvents ${Events} \
--outputRDOFile ${DigiOutFileNameCG} \
--postExec 'HITtoRDO:job+=CfgMgr.JobOptsDumperAlg(FileName="DigiPUConfigCG.txt")' \
--postInclude 'default:PyJobTransforms/UseFrontier.py' \
--preInclude 'HITtoRDO:Campaigns/PileUpPresamplingMC21aSingleBeamspot.py' \
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
    --PileUpPresampling True \
    --conditionsTag default:OFLCOND-MC21-SDR-RUN3-05 \
    --digiSeedOffset1 170 --digiSeedOffset2 170 \
    --digiSteeringConf 'StandardSignalOnlyTruth' \
    --geometryVersion default:ATLAS-R3S-2021-02-00-00 \
    --inputHITSFile ${HSHitsFile} \
    --inputHighPtMinbiasHitsFile ${HighPtMinbiasHitsFiles} \
    --inputLowPtMinbiasHitsFile ${LowPtMinbiasHitsFiles} \
    --jobNumber 568 \
    --maxEvents ${Events} \
    --outputRDOFile ${DigiOutFileNameCA} \
    --postInclude 'PyJobTransforms.UseFrontier' 'HITtoRDO:Digitization.DigitizationSteering.DigitizationTestingPostInclude' \
    --preInclude 'HITtoRDO:Campaigns.MC21aSingleBeamspot' \
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
        --ignore-leaves RecoTimingObj_p1_Bkg_HITStoRDO_timings index_ref
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
echo "art-result: $rc4 OLDvsFixedRef"

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
