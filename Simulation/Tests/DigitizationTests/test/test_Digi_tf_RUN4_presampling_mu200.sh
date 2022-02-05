#!/bin/bash
#
# art-description: Run 4 pile-up presampling
# art-type: grid
# art-include: master/Athena
# art-output: RUN4_presampling.mu200.RDO.pool.root

Events=3
HSHitsFile="/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/PhaseIIUpgrade/HITS/single_neutrino.HITS.pool.root"
HighPtMinbiasHitsFiles="/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/PhaseIIUpgrade/HITS/user.tadej.Upgrade.800381.Py8EG_A3NNPDF23LO_minbias_inelastic_high_keepJets.simul.HITS_FILT.20210902.r1_EXT0/*"
LowPtMinbiasHitsFiles="/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/PhaseIIUpgrade/HITS/user.tadej.Upgrade.800380.Py8EG_A3NNPDF23LO_minbias_inelastic_low_keepJets.simul.HITS_FILT.20210902.r1_EXT0/*"
DigiOutFileName="RUN4_presampling.mu200.RDO.pool.root"

Digi_tf.py \
--CA \
--PileUpPresampling True \
--conditionsTag default:OFLCOND-MC15c-SDR-14-05 \
--digiSeedOffset1 170 --digiSeedOffset2 170 \
--digiSteeringConf 'StandardSignalOnlyTruth' \
--geometryVersion default:ATLAS-P2-ITK-24-00-00 \
--inputHITSFile ${HSHitsFile} \
--inputHighPtMinbiasHitsFile ${HighPtMinbiasHitsFiles} \
--inputLowPtMinbiasHitsFile ${LowPtMinbiasHitsFiles} \
--jobNumber 568 \
--maxEvents ${Events} \
--outputRDOFile ${DigiOutFileName} \
--preInclude 'HITtoRDO:Campaigns.PhaseIIPileUp' \
--postInclude 'PyJobTransforms.UseFrontier' \
--skipEvents 0

rc=$?
status=$rc
echo "art-result: $rc Digi_tf.py"

exit $status
