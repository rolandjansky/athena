#!/bin/sh
#
# art-description: Modified/Simplified MC16a digitization job
# art-type: build
# art-include: 21.0/Athena
# art-include: 21.3/Athena
# art-include: 21.9/Athena
# art-include: master/Athena

HighPtMinbiasHitsFiles="/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/Tier0ChainTests/mc16_13TeV.361239.Pythia8EvtGen_A3NNPDF23LO_minbias_inelastic_high.merge.HITS.e4981_s3087_s3089/*"
LowPtMinbiasHitsFiles="/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/Tier0ChainTests/mc16_13TeV.361238.Pythia8EvtGen_A3NNPDF23LO_minbias_inelastic_low.merge.HITS.e4981_s3087_s3089/*"

Digi_tf.py \
--inputHITSFile /cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/Tier0ChainTests/valid1.410000.PowhegPythiaEvtGen_P2012_ttbar_hdamp172p5_nonallhad.simul.HITS.e4993_s3091/HITS.10504490._000425.pool.root.1 \
--conditionsTag default:OFLCOND-MC16-SDR-16 \
--geometryVersion default:ATLAS-R2-2016-01-00-01 \
--inputHighPtMinbiasHitsFile ${HighPtMinbiasHitsFiles} \
--inputLowPtMinbiasHitsFile ${LowPtMinbiasHitsFiles} \
--maxEvents 10 \
--numberOfCavernBkg 0 \
--numberOfHighPtMinBias 0.01 \
--numberOfLowPtMinBias 0.99 \
--outputRDOFile test.RDO.pool.root \
--skipEvents 0 \
--bunchSpacing 450 \
--pileupInitialBunch -1 \
--pileupFinalBunch 1 \
--DataRunNumber 284500

echo  "art-result: $? digitization"
