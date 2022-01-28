#!/bin/bash

Events=100
DigiOutFileName="mc21a_presampling.RDO.pool.root"
HSHitsFile="/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/DigitizationTests/NSW/Neutrinos.R3.2021-06-27.HITS.pool.root"
HighPtMinbiasHitsFiles="/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/DigitizationTests/NSW/group.det-muon.high-pTMinBias.AsymRun3.rel2021_06_22.merge.HITS_EXT0/*"
LowPtMinbiasHitsFiles="/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/DigitizationTests/NSW/group.det-muon.low-pTMinBias.AsymRun3.rel2021_06_22.merge.HITS_EXT0/*"


# full run
Digi_tf.py \
--PileUpPresampling True \
--DataRunNumber 330000 \
--conditionsTag default:OFLCOND-MC16-SDR-RUN3-01 \
--digiSeedOffset1 170 --digiSeedOffset2 170 \
--digiSteeringConf 'StandardSignalOnlyTruth' \
--geometryVersion default:ATLAS-R3-2021-01-00-02 \
--inputHITSFile ${HSHitsFile} \
--inputHighPtMinbiasHitsFile ${HighPtMinbiasHitsFiles} \
--inputLowPtMinbiasHitsFile ${LowPtMinbiasHitsFiles} \
--jobNumber 1 \
--maxEvents ${Events} \
--outputRDOFile ${DigiOutFileName} \
--postExec 'all:CfgMgr.MessageSvc().setError+=["HepMcParticleLink"]' \
--postInclude 'default:PyJobTransforms/UseFrontier.py' \
--preInclude 'HITtoRDO:Campaigns/PileUpPresamplingMC21a.py' \
--skipEvents 0

status=$?
exit $status
