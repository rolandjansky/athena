#!/bin/bash
#
# art-description: Run 4 pile-up ttbar MP tests
# art-type: grid
# art-architecture:  '#x86_64-intel'
# art-athena-mt: 8
# art-include: master/Athena
# art-output: RUN4_ttbar_SP.RDO.pool.root
# art-output: RUN4_ttbar_MP_fork_evt0.RDO.pool.root
# art-output: RUN4_ttbar_MP_fork_evt1.RDO.pool.root

export ATHENA_CORE_NUMBER=8

DigiOutFileNameSP="RUN4_ttbar_SP.RDO.pool.root"
DigiOutFileNameMP0="RUN4_ttbar_MP_fork_evt0.RDO.pool.root"
DigiOutFileNameMP1="RUN4_ttbar_MP_fork_evt1.RDO.pool.root"

Events=50
HSHitsFile="/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/PhaseIIUpgrade/HITS/ATLAS-P2-RUN4-01-00-00/mc15_14TeV.600012.PhPy8EG_A14_ttbar_hdamp258p75_nonallhad.simul.HITS.e8185_s3856/HITS.29179779._000100.pool.root.1"
HighPtMinbiasHitsFiles="/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/PhaseIIUpgrade/HITS/ATLAS-P2-RUN4-01-00-00/mc15_14TeV.800381.Py8EG_A3NNPDF23LO_minbias_inelastic_high_keepJets.merge.HITS.e8205_s3856_s3857/*"
LowPtMinbiasHitsFiles="/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/PhaseIIUpgrade/HITS/ATLAS-P2-RUN4-01-00-00/mc15_14TeV.800380.Py8EG_A3NNPDF23LO_minbias_inelastic_low_keepJets.merge.HITS.e8205_s3856_s3857/*"


Digi_tf.py \
--CA \
--conditionsTag default:OFLCOND-MC15c-SDR-14-05 \
--digiSeedOffset1 170 --digiSeedOffset2 170 \
--digiSteeringConf 'StandardSignalOnlyTruth' \
--geometryVersion default:ATLAS-P2-RUN4-01-00-00 \
--inputHITSFile ${HSHitsFile} \
--inputHighPtMinbiasHitsFile ${HighPtMinbiasHitsFiles} \
--inputLowPtMinbiasHitsFile ${LowPtMinbiasHitsFiles} \
--jobNumber 568 \
--maxEvents ${Events} \
--outputRDOFile ${DigiOutFileNameSP} \
--preInclude 'HITtoRDO:Campaigns.PhaseIIPileUp200' \
--postExec 'HITtoRDO:cfg.getService("PileUpEventLoopMgr").AllowSerialAndMPToDiffer=False' \
--postInclude 'PyJobTransforms.UseFrontier' \
--skipEvents 0

rc=$?
status=$rc
echo "art-result: $rc Digi_tf.py SP"

mv ${DigiOutFileNameSP} backup_${DigiOutFileNameSP}
RDOMerge_tf.py --CA --inputRDOFile backup_${DigiOutFileNameSP} --outputRDO_MRGFile ${DigiOutFileNameSP}
rm backup_${DigiOutFileNameSP}

Digi_tf.py \
--multiprocess --athenaMPEventsBeforeFork 0 \
--CA \
--conditionsTag default:OFLCOND-MC15c-SDR-14-05 \
--digiSeedOffset1 170 --digiSeedOffset2 170 \
--digiSteeringConf 'StandardSignalOnlyTruth' \
--geometryVersion default:ATLAS-P2-RUN4-01-00-00 \
--inputHITSFile ${HSHitsFile} \
--inputHighPtMinbiasHitsFile ${HighPtMinbiasHitsFiles} \
--inputLowPtMinbiasHitsFile ${LowPtMinbiasHitsFiles} \
--jobNumber 568 \
--maxEvents ${Events} \
--outputRDOFile ${DigiOutFileNameMP0} \
--preInclude 'HITtoRDO:Campaigns.PhaseIIPileUp200' \
--postExec 'HITtoRDO:cfg.getService("PileUpEventLoopMgr").AllowSerialAndMPToDiffer=False' \
--postInclude 'PyJobTransforms.UseFrontier' \
--skipEvents 0

rc2=$?
if [ $status -eq 0 ]; then
  status=$rc2
fi
echo "art-result: $rc2 Digi_tf.py MP fork after 0"

Digi_tf.py \
--multiprocess --athenaMPEventsBeforeFork 1 \
--CA \
--conditionsTag default:OFLCOND-MC15c-SDR-14-05 \
--digiSeedOffset1 170 --digiSeedOffset2 170 \
--digiSteeringConf 'StandardSignalOnlyTruth' \
--geometryVersion default:ATLAS-P2-RUN4-01-00-00 \
--inputHITSFile ${HSHitsFile} \
--inputHighPtMinbiasHitsFile ${HighPtMinbiasHitsFiles} \
--inputLowPtMinbiasHitsFile ${LowPtMinbiasHitsFiles} \
--jobNumber 568 \
--maxEvents ${Events} \
--outputRDOFile ${DigiOutFileNameMP1} \
--preInclude 'HITtoRDO:Campaigns.PhaseIIPileUp200' \
--postExec 'HITtoRDO:cfg.getService("PileUpEventLoopMgr").AllowSerialAndMPToDiffer=False' \
--postInclude 'PyJobTransforms.UseFrontier' \
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
