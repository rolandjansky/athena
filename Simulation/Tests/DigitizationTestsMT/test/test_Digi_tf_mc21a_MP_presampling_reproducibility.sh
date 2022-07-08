#!/bin/bash
#
# art-description: Run mc21a pile-up presampling
# art-type: grid
# art-architecture:  '#x86_64-intel'
# art-athena-mt: 8
# art-include: 22.0/Athena
# art-include: master/Athena
# art-output: mc21a_presampling_SP.RDO.pool.root
# art-output: mc21a_presampling_MP_fork_evt0.RDO.pool.root
# art-output: mc21a_presampling_MP_fork_evt1.RDO.pool.root

export ATHENA_CORE_NUMBER=8

Events=100
DigiOutFileNameSP="mc21a_presampling_SP.RDO.pool.root"
DigiOutFileNameMP0="mc21a_presampling_MP_fork_evt0.RDO.pool.root"
DigiOutFileNameMP1="mc21a_presampling_MP_fork_evt1.RDO.pool.root"

HSHitsFile="/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/DigitizationTests/NSW/mc21_13p6TeV.900149.PG_single_nu_Pt50.simul.HITS.e8442_e8447_s3822/*"
HighPtMinbiasHitsFiles1="/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/DigitizationTests/NSW/mc21_13p6TeV.800831.Py8EG_minbias_inelastic_highjetphotonlepton.merge.HITS.e8341_s3775_s3787/*"
HighPtMinbiasHitsFiles2="/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/DigitizationTests/NSW/mc21_13p6TeV.800831.Py8EG_minbias_inelastic_highjetphotonlepton.merge.HITS.e8341_s3782_s3787/*"
HighPtMinbiasHitsFiles3="/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/DigitizationTests/NSW/mc21_13p6TeV.800831.Py8EG_minbias_inelastic_highjetphotonlepton.merge.HITS.e8341_s3783_s3787/*"
HighPtMinbiasHitsFiles4="/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/DigitizationTests/NSW/mc21_13p6TeV.800831.Py8EG_minbias_inelastic_highjetphotonlepton.merge.HITS.e8341_s3784_s3787/*"
LowPtMinbiasHitsFiles1="/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/DigitizationTests/NSW/mc21_13p6TeV.900311.Epos_minbias_inelastic_lowjetphoton.merge.HITS.e8341_s3775_s3787/*"
LowPtMinbiasHitsFiles2="/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/DigitizationTests/NSW/mc21_13p6TeV.900311.Epos_minbias_inelastic_lowjetphoton.merge.HITS.e8341_s3782_s3787/*"
LowPtMinbiasHitsFiles3="/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/DigitizationTests/NSW/mc21_13p6TeV.900311.Epos_minbias_inelastic_lowjetphoton.merge.HITS.e8341_s3783_s3787/*"
LowPtMinbiasHitsFiles4="/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/DigitizationTests/NSW/mc21_13p6TeV.900311.Epos_minbias_inelastic_lowjetphoton.merge.HITS.e8341_s3784_s3787/*"


Digi_tf.py \
--splitConfig 'HITtoRDO:Campaigns.BeamspotSplitMC21a' \
--detectors Truth \
--PileUpPresampling True \
--conditionsTag default:OFLCOND-MC21-SDR-RUN3-05 \
--digiSeedOffset1 170 --digiSeedOffset2 170 \
--digiSteeringConf 'StandardSignalOnlyTruth' \
--geometryVersion default:ATLAS-R3S-2021-02-00-00 \
--inputHITSFile ${HSHitsFile} \
--inputHighPtMinbiasHitsFile ${HighPtMinbiasHitsFiles1} \
--inputHighPtMinbiasHitsFile ${HighPtMinbiasHitsFiles2} \
--inputHighPtMinbiasHitsFile ${HighPtMinbiasHitsFiles3} \
--inputHighPtMinbiasHitsFile ${HighPtMinbiasHitsFiles4} \
--inputLowPtMinbiasHitsFile ${LowPtMinbiasHitsFiles1} \
--inputLowPtMinbiasHitsFile ${LowPtMinbiasHitsFiles2} \
--inputLowPtMinbiasHitsFile ${LowPtMinbiasHitsFiles3} \
--inputLowPtMinbiasHitsFile ${LowPtMinbiasHitsFiles4} \
--jobNumber 568 \
--maxEvents ${Events} \
--outputRDOFile ${DigiOutFileNameSP} \
--preInclude 'HITtoRDO:Campaigns/PileUpPresamplingMC21a.py' \
--postExec 'HITtoRDO:ServiceMgr.PileUpEventLoopMgr.AllowSerialAndMPToDiffer=False' \
--postInclude 'default:PyJobTransforms/UseFrontier.py' 'all:PyJobTransforms/HepMcParticleLinkVerbosity.py' \
--skipEvents 0

rc=$?
status=$rc
echo "art-result: $rc Digi_tf.py SP"

Digi_tf.py \
--multiprocess --athenaMPEventsBeforeFork 0 \
--splitConfig 'HITtoRDO:Campaigns.BeamspotSplitMC21a' \
--detectors Truth \
--PileUpPresampling True \
--conditionsTag default:OFLCOND-MC21-SDR-RUN3-05 \
--digiSeedOffset1 170 --digiSeedOffset2 170 \
--digiSteeringConf 'StandardSignalOnlyTruth' \
--geometryVersion default:ATLAS-R3S-2021-02-00-00 \
--inputHITSFile ${HSHitsFile} \
--inputHighPtMinbiasHitsFile ${HighPtMinbiasHitsFiles1} \
--inputHighPtMinbiasHitsFile ${HighPtMinbiasHitsFiles2} \
--inputHighPtMinbiasHitsFile ${HighPtMinbiasHitsFiles3} \
--inputHighPtMinbiasHitsFile ${HighPtMinbiasHitsFiles4} \
--inputLowPtMinbiasHitsFile ${LowPtMinbiasHitsFiles1} \
--inputLowPtMinbiasHitsFile ${LowPtMinbiasHitsFiles2} \
--inputLowPtMinbiasHitsFile ${LowPtMinbiasHitsFiles3} \
--inputLowPtMinbiasHitsFile ${LowPtMinbiasHitsFiles4} \
--jobNumber 568 \
--maxEvents ${Events} \
--outputRDOFile ${DigiOutFileNameMP0} \
--preInclude 'HITtoRDO:Campaigns/PileUpPresamplingMC21a.py' \
--postExec 'HITtoRDO:ServiceMgr.PileUpEventLoopMgr.AllowSerialAndMPToDiffer=False' \
--postInclude 'default:PyJobTransforms/UseFrontier.py' 'all:PyJobTransforms/HepMcParticleLinkVerbosity.py' \
--skipEvents 0

rc2=$?
if [[ $status -eq 0 ]]; then
  status=$rc2
fi
echo "art-result: $rc2 Digi_tf.py MP fork after 0"

Digi_tf.py \
--multiprocess --athenaMPEventsBeforeFork 1 \
--splitConfig 'HITtoRDO:Campaigns.BeamspotSplitMC21a' \
--detectors Truth \
--PileUpPresampling True \
--conditionsTag default:OFLCOND-MC21-SDR-RUN3-05 \
--digiSeedOffset1 170 --digiSeedOffset2 170 \
--digiSteeringConf 'StandardSignalOnlyTruth' \
--geometryVersion default:ATLAS-R3S-2021-02-00-00 \
--inputHITSFile ${HSHitsFile} \
--inputHighPtMinbiasHitsFile ${HighPtMinbiasHitsFiles1} \
--inputHighPtMinbiasHitsFile ${HighPtMinbiasHitsFiles2} \
--inputHighPtMinbiasHitsFile ${HighPtMinbiasHitsFiles3} \
--inputHighPtMinbiasHitsFile ${HighPtMinbiasHitsFiles4} \
--inputLowPtMinbiasHitsFile ${LowPtMinbiasHitsFiles1} \
--inputLowPtMinbiasHitsFile ${LowPtMinbiasHitsFiles2} \
--inputLowPtMinbiasHitsFile ${LowPtMinbiasHitsFiles3} \
--inputLowPtMinbiasHitsFile ${LowPtMinbiasHitsFiles4} \
--jobNumber 568 \
--maxEvents ${Events} \
--outputRDOFile ${DigiOutFileNameMP1} \
--preInclude 'HITtoRDO:Campaigns/PileUpPresamplingMC21a.py' \
--postExec 'HITtoRDO:ServiceMgr.PileUpEventLoopMgr.AllowSerialAndMPToDiffer=False' \
--postInclude 'default:PyJobTransforms/UseFrontier.py' 'all:PyJobTransforms/HepMcParticleLinkVerbosity.py' \
--skipEvents 0

rc3=$?
if [[ $status -eq 0 ]]; then
  status=$rc3
fi
echo "art-result: $rc3 Digi_tf.py MP fork after 1"

rc4=-9999
if [[ $rc -eq 0 ]] && [[ $rc2 -eq 0 ]]
then
    acmd.py diff-root ${DigiOutFileNameSP} ${DigiOutFileNameMP0} \
        --mode=semi-detailed --error-mode resilient --order-trees \
        --ignore-leaves RecoTimingObj_p1_Bkg_HITStoRDO_timings index_ref
    rc4=$?
    if [[ $status -eq 0 ]]; then
        status=$rc4
    fi
fi
echo "art-result: $rc4 SP vs MP fork after 0"

rc5=-9999
if [[ $rc -eq 0 ]] && [[ $rc3 -eq 0 ]]
then
    acmd.py diff-root ${DigiOutFileNameSP} ${DigiOutFileNameMP1} \
        --mode=semi-detailed --error-mode resilient --order-trees \
        --ignore-leaves RecoTimingObj_p1_Bkg_HITStoRDO_timings index_ref
    rc5=$?
    if [[ $status -eq 0 ]]; then
        status=$rc5
    fi
fi
echo "art-result: $rc5 SP vs MP fork after 1"

exit $status
