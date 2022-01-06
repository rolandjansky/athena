#!/bin/bash
#
# art-description: Run MC16 pile-up pre-mixing with 2016d geometry and conditions, 25ns pile-up, MT output containers in AthenaMP
# art-include: master/Athena
# art-type: grid
# art-athena-mt: 8
# art-output: mc16d_premixing_MT.MP.RDO.pool.root

DigiOutFileName="mc16d_premixing_MT.RDO.pool.root"

HighPtMinbiasHitsFiles="/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/Tier0ChainTests/mc16_13TeV.361239.Pythia8EvtGen_A3NNPDF23LO_minbias_inelastic_high.merge.HITS.e4981_s3087_s3089/*"
LowPtMinbiasHitsFiles="/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/Tier0ChainTests/mc16_13TeV.361238.Pythia8EvtGen_A3NNPDF23LO_minbias_inelastic_low.merge.HITS.e4981_s3087_s3089/*"

export ATHENA_CORE_NUMBER=8

Digi_tf.py \
--multiprocess \
--PileUpPresampling True \
--inputHITSFile /cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/DigitizationTests/mc16_valid.422075.ParticleGun_single_nu_Pt50.simul.HITS.e6739_e5984_s3126.HITS.14436922._003949.pool.root \
--conditionsTag default:OFLCOND-MC16-SDR-25-02 \
--digiSeedOffset1 170 --digiSeedOffset2 170 \
--geometryVersion default:ATLAS-R2-2016-01-00-01 \
--inputHighPtMinbiasHitsFile ${HighPtMinbiasHitsFiles} \
--inputLowPtMinbiasHitsFile ${LowPtMinbiasHitsFiles} \
--jobNumber 1 \
--maxEvents 25 \
--outputRDOFile ${DigiOutFileName} \
--digiSteeringConf 'StandardSignalOnlyTruth' \
--postInclude 'default:PyJobTransforms/UseFrontier.py' \
--preInclude 'HITtoRDO:Campaigns/PileUpPresamplingMC16d.py' \
--skipEvents 0

rc=$?
status=$rc
echo "art-result: $rc Digi_tf.py"

exit $status
