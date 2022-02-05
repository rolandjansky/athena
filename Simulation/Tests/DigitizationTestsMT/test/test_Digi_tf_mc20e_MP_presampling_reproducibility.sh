#!/bin/bash
#
# art-description: Run MC20e pile-up pre-mixing with 2018 geometry and conditions, 25ns pile-up, MT output containers
# art-type: grid
# art-athena-mt: 8
# art-include: 22.0-mc20/Athena
# art-include: master/Athena
# art-output: mc20e_presampling_SP.RDO.pool.root
# art-output: mc20e_presampling_MP_fork_evt0.RDO.pool.root
# art-output: mc20e_presampling_MP_fork_evt1.RDO.pool.root

export ATHENA_CORE_NUMBER=8

DigiOutFileNameSP="mc20e_presampling_SP.RDO.pool.root"
DigiOutFileNameMP0="mc20e_presampling_MP_fork_evt0.RDO.pool.root"
DigiOutFileNameMP1="mc20e_presampling_MP_fork_evt1.RDO.pool.root"

InputHitsFile="/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/DigitizationTests/mc16_valid.422075.ParticleGun_single_nu_Pt50.simul.HITS.e6739_e5984_s3126.HITS.14436922._003949.pool.root"
HighPtMinbiasHitsFiles="/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/Tier0ChainTests/mc16_13TeV.361239.Pythia8EvtGen_A3NNPDF23LO_minbias_inelastic_high.merge.HITS.e4981_s3087_s3089/*"
LowPtMinbiasHitsFiles="/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/Tier0ChainTests/mc16_13TeV.361238.Pythia8EvtGen_A3NNPDF23LO_minbias_inelastic_low.merge.HITS.e4981_s3087_s3089/*"


Digi_tf.py \
--PileUpPresampling True \
--inputHITSFile ${InputHitsFile} \
--conditionsTag default:OFLCOND-MC16-SDR-RUN2-09 \
--digiSeedOffset1 170 --digiSeedOffset2 170 \
--geometryVersion default:ATLAS-R2-2016-01-00-01 \
--inputHighPtMinbiasHitsFile ${HighPtMinbiasHitsFiles} \
--inputLowPtMinbiasHitsFile ${LowPtMinbiasHitsFiles} \
--jobNumber 38 \
--maxEvents 25 \
--outputRDOFile ${DigiOutFileNameSP} \
--digiSteeringConf 'StandardSignalOnlyTruth' \
--postExec 'HITtoRDO:ServiceMgr.PileUpEventLoopMgr.AllowSerialAndMPToDiffer=False' \
--postInclude 'default:PyJobTransforms/UseFrontier.py' 'all:PyJobTransforms/HepMcParticleLinkVerbosity.py' \
--preInclude 'HITtoRDO:Campaigns/PileUpPresamplingMC20e.py' \
--skipEvents 0

rc=$?
status=$rc
echo "art-result: $rc Digi_tf.py SP"

Digi_tf.py \
--multiprocess --athenaMPEventsBeforeFork 0 \
--PileUpPresampling True \
--inputHITSFile ${InputHitsFile} \
--conditionsTag default:OFLCOND-MC16-SDR-RUN2-09 \
--digiSeedOffset1 170 --digiSeedOffset2 170 \
--geometryVersion default:ATLAS-R2-2016-01-00-01 \
--inputHighPtMinbiasHitsFile ${HighPtMinbiasHitsFiles} \
--inputLowPtMinbiasHitsFile ${LowPtMinbiasHitsFiles} \
--jobNumber 38 \
--maxEvents 25 \
--outputRDOFile ${DigiOutFileNameMP0} \
--digiSteeringConf 'StandardSignalOnlyTruth' \
--postExec 'HITtoRDO:ServiceMgr.PileUpEventLoopMgr.AllowSerialAndMPToDiffer=False' \
--postInclude 'default:PyJobTransforms/UseFrontier.py' 'all:PyJobTransforms/HepMcParticleLinkVerbosity.py' \
--preInclude 'HITtoRDO:Campaigns/PileUpPresamplingMC20e.py' \
--skipEvents 0

rc2=$?
if [ $status -eq 0 ]; then
  status=$rc2
fi
echo "art-result: $rc2 Digi_tf.py MP fork after 0"

Digi_tf.py \
--multiprocess --athenaMPEventsBeforeFork 1 \
--PileUpPresampling True \
--inputHITSFile ${InputHitsFile} \
--conditionsTag default:OFLCOND-MC16-SDR-RUN2-09 \
--digiSeedOffset1 170 --digiSeedOffset2 170 \
--geometryVersion default:ATLAS-R2-2016-01-00-01 \
--inputHighPtMinbiasHitsFile ${HighPtMinbiasHitsFiles} \
--inputLowPtMinbiasHitsFile ${LowPtMinbiasHitsFiles} \
--jobNumber 38 \
--maxEvents 25 \
--outputRDOFile ${DigiOutFileNameMP1} \
--digiSteeringConf 'StandardSignalOnlyTruth' \
--postExec 'HITtoRDO:ServiceMgr.PileUpEventLoopMgr.AllowSerialAndMPToDiffer=False' \
--postInclude 'default:PyJobTransforms/UseFrontier.py' 'all:PyJobTransforms/HepMcParticleLinkVerbosity.py' \
--preInclude 'HITtoRDO:Campaigns/PileUpPresamplingMC20e.py' \
--skipEvents 0

rc3=$?
if [[ $status -eq 0 ]]; then
    status=$rc3
fi
echo "art-result: $rc3 Digi_tf.py MP fork after 1"

rc4=-9999
if [[ $status -eq 0 ]] && [[ $rc -eq 0 ]] && [[ $rc2 -eq 0 ]]
then
    acmd.py diff-root ${DigiOutFileNameSP} ${DigiOutFileNameMP0} \
        --mode=semi-detailed --error-mode resilient --order-trees \
        --ignore-leaves RecoTimingObj_p1_Bkg_HITStoRDO_timings index_ref
    rc4=$?
    status=$rc4
fi
echo "art-result: $rc4 SP vs MP fork after 0"

rc5=-9999
if [[ $status -eq 0 ]] && [[ $rc -eq 0 ]] && [[ $rc3 -eq 0 ]]
then
    acmd.py diff-root ${DigiOutFileNameSP} ${DigiOutFileNameMP1} \
        --mode=semi-detailed --error-mode resilient --order-trees \
        --ignore-leaves RecoTimingObj_p1_Bkg_HITStoRDO_timings index_ref
    rc5=$?
    status=$rc5
fi
echo "art-result: $rc5 SP vs MP fork after 1"

exit $status
