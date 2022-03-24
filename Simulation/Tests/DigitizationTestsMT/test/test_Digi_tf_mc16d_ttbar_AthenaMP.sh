#!/bin/bash
#
# art-description: Run digitization of an mc16d ttbar sample with 2016d geometry and conditions, 25ns pile-up in AthenaMP
# art-include: master/Athena
# art-type: grid
# art-athena-mt: 8
# art-output: mc16d_ttbar.MP.RDO.pool.root

DigiOutFileName="mc16d_ttbar.MP.RDO.pool.root"

HighPtMinbiasHitsFiles="/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/Tier0ChainTests/mc16_13TeV.361239.Pythia8EvtGen_A3NNPDF23LO_minbias_inelastic_high.merge.HITS.e4981_s3087_s3089/*"
LowPtMinbiasHitsFiles="/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/Tier0ChainTests/mc16_13TeV.361238.Pythia8EvtGen_A3NNPDF23LO_minbias_inelastic_low.merge.HITS.e4981_s3087_s3089/*"


export ATHENA_CORE_NUMBER=8

Digi_tf.py \
--multiprocess \
--inputHITSFile /cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/Tier0ChainTests/valid1.410000.PowhegPythiaEvtGen_P2012_ttbar_hdamp172p5_nonallhad.simul.HITS.e4993_s3091/HITS.10504490._000425.pool.root.1 \
--conditionsTag default:OFLCOND-MC16-SDR-25-02 \
--digiSeedOffset1 170 --digiSeedOffset2 170 \
--geometryVersion default:ATLAS-R2-2016-01-00-01 \
--inputHighPtMinbiasHitsFile ${HighPtMinbiasHitsFiles} \
--inputLowPtMinbiasHitsFile ${LowPtMinbiasHitsFiles} \
--jobNumber 1 \
--maxEvents 25 \
--outputRDOFile ${DigiOutFileName} \
--digiSteeringConf 'StandardSignalOnlyTruth' \
--postInclude 'default:PyJobTransforms/UseFrontier.py' 'all:PyJobTransforms/HepMcParticleLinkVerbosity.py' \
--preInclude 'all:Campaigns/MC16d.py' 'HITtoRDO:Campaigns/PileUpMC16d.py' \
--postExec 'RDOMergeAthenaMP0:from IOVDbSvc.CondDB import conddb;conddb.addOverride("/TRT/Calib/PID_NN", "TRTCalibPID_NN_v1");conddb.addOverride("/TRT/Onl/Calib/PID_NN", "TRTCalibPID_NN_v1")' \
--skipEvents 0

rc=$?
status=$rc
echo "art-result: $rc Digi_tf.py"

exit $status
