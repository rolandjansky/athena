#!/bin/bash
#
# art-description: Run a digitization example to compare configuration between ConfGetter and the new ComponentAccumulator approach.
# art-type: grid
# art-include: master/Athena
# art-output: mc20e_presampling.CG.RDO.pool.root
# art-output: mc20e_presampling.CA.RDO.pool.root
# art-output: log.*
# art-output: legacy.*
# art-output: DigiPUConfig*

Events=20
DigiOutFileNameCG="mc20e_presampling.CG.RDO.pool.root"
DigiOutFileNameCA="mc20e_presampling.CA.RDO.pool.root"
HSHitsFile="/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/OverlayTests/mc16_13TeV.900149.PG_single_nu_Pt50.simul.HITS.e8307_s3482/HITS.24078104._234467.pool.root.1"
HighPtMinbiasHitsFiles="/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/Tier0ChainTests/mc16_13TeV.800831.Py8EG_minbias_inelastic_highjetphotonlepton.simul.HITS_FILT.e8341_s3687_s3704/*"
LowPtMinbiasHitsFiles="/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/Tier0ChainTests/mc16_13TeV.900311.Epos_minbias_inelastic_lowjetphoton.simul.HITS_FILT.e8341_s3687_s3704/*"


# config only
Digi_tf.py \
--PileUpPresampling True \
--conditionsTag default:OFLCOND-MC16-SDR-RUN2-09 \
--digiSeedOffset1 170 --digiSeedOffset2 170 \
--digiSteeringConf 'StandardSignalOnlyTruth' \
--geometryVersion default:ATLAS-R2-2016-01-00-01 \
--inputHITSFile ${HSHitsFile} \
--inputHighPtMinbiasHitsFile ${HighPtMinbiasHitsFiles} \
--inputLowPtMinbiasHitsFile ${LowPtMinbiasHitsFiles} \
--jobNumber 568 \
--maxEvents ${Events} \
--outputRDOFile ${DigiOutFileNameCG} \
--postInclude 'default:PyJobTransforms/UseFrontier.py' \
--preExec 'HITtoRDO:userRunLumiOverride={"run":310000, "startmu":0.0, "endmu":10.0, "stepmu":1.0, "startlb":1, "timestamp": 1550000000};' \
--preInclude 'HITtoRDO:Campaigns/PileUpPresamplingMC20e.py' \
--skipEvents 0 \
--athenaopts '"--config-only=DigiPUConfigCG.pkl"'

# full run
Digi_tf.py \
--PileUpPresampling True \
--conditionsTag default:OFLCOND-MC16-SDR-RUN2-09 \
--digiSeedOffset1 170 --digiSeedOffset2 170 \
--digiSteeringConf 'StandardSignalOnlyTruth' \
--geometryVersion default:ATLAS-R2-2016-01-00-01 \
--inputHITSFile ${HSHitsFile} \
--inputHighPtMinbiasHitsFile ${HighPtMinbiasHitsFiles} \
--inputLowPtMinbiasHitsFile ${LowPtMinbiasHitsFiles} \
--jobNumber 568 \
--maxEvents ${Events} \
--outputRDOFile ${DigiOutFileNameCG} \
--postExec 'HITtoRDO:job+=CfgMgr.JobOptsDumperAlg(FileName="DigiPUConfigCG.txt")' \
--postInclude 'default:PyJobTransforms/UseFrontier.py' \
--preExec 'HITtoRDO:userRunLumiOverride={"run":310000, "startmu":0.0, "endmu":10.0, "stepmu":1.0, "startlb":1, "timestamp": 1550000000};' \
--preInclude 'HITtoRDO:Campaigns/PileUpPresamplingMC20e.py' \
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
    --conditionsTag default:OFLCOND-MC16-SDR-RUN2-09 \
    --digiSeedOffset1 170 --digiSeedOffset2 170 \
    --digiSteeringConf 'StandardSignalOnlyTruth' \
    --geometryVersion default:ATLAS-R2-2016-01-00-01 \
    --inputHITSFile ${HSHitsFile} \
    --inputHighPtMinbiasHitsFile ${HighPtMinbiasHitsFiles} \
    --inputLowPtMinbiasHitsFile ${LowPtMinbiasHitsFiles} \
    --jobNumber 568 \
    --maxEvents ${Events} \
    --outputRDOFile ${DigiOutFileNameCA} \
    --postInclude 'PyJobTransforms.UseFrontier' 'HITtoRDO:Digitization.DigitizationSteering.DigitizationTestingPostInclude' \
    --preExec 'HITtoRDO:ConfigFlags.Digitization.PU.CustomProfile={"run":310000, "startmu":0.0, "endmu":10.0, "stepmu":1.0, "startlb":1, "timestamp": 1550000000};' \
    --preInclude 'HITtoRDO:Campaigns.MC20e' \
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
        --ignore-leaves RecoTimingObj_p1_HITStoRDO_timings index_ref
    rc3=$?
    status=$rc3
fi
echo "art-result: $rc3 comparison"

exit $status
