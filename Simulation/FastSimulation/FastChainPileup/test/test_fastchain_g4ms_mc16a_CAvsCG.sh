#!/bin/sh
#
# art-description: Run AFII simulation and full digitization of an MC16a ttbar sample with 2016a geometry and conditions, 25ns pile-up
# art-type: grid
# art-include: master/Athena
# art-output: run_*

maxevent=2
inputfile="/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/ISF_Validation/mc12_valid.110401.PowhegPythia_P2012_ttbar_nonallhad.evgen.EVNT.e3099.01517252._000001.pool.root.1"
HighPtMinbiasHitsFiles="/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/Tier0ChainTests/mc16_13TeV.361239.Pythia8EvtGen_A3NNPDF23LO_minbias_inelastic_high.merge.HITS.e4981_s3087_s3089/*"
LowPtMinbiasHitsFiles="/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/Tier0ChainTests/mc16_13TeV.361238.Pythia8EvtGen_A3NNPDF23LO_minbias_inelastic_low.merge.HITS.e4981_s3087_s3089/*"

mkdir ./run_cg; cd run_cg

FastChain_tf.py \
    --simulator ATLFASTIIF_G4MS \
    --digiSteeringConf "Split" \
    --useISF True \
    --randomSeed 123 \
    --enableLooperKiller True \
    --inputEVNTFile ${inputfile} \
    --outputRDOFile RDO_CG.pool.root \
    --maxEvents ${maxevent} \
    --skipEvents 0 \
    --geometryVersion default:ATLAS-R2-2016-01-00-01 \
    --conditionsTag default:OFLCOND-MC16-SDR-16 \
    --preSimExec 'from AthenaCommon.DetFlags import DetFlags;DetFlags.simulate.BCM_setOff();from TrkDetDescrSvc.TrkDetDescrJobProperties import TrkDetFlags;TrkDetFlags.TRT_BuildStrawLayers=True;from Digitization.DigitizationFlags import digitizationFlags;digitizationFlags.experimentalDigi=["NewMerge"]' \
    --postInclude='PyJobTransforms/UseFrontier.py' \
    --DataRunNumber '284500' \
    --inputHighPtMinbiasHitsFile ${HighPtMinbiasHitsFiles} \
    --inputLowPtMinbiasHitsFile ${LowPtMinbiasHitsFiles} \
    --pileupFinalBunch '6' \
    --numberOfHighPtMinBias '0.116075313' \
    --numberOfLowPtMinBias '44.3839246425' \
    --numberOfCavernBkg 0 \
    --bunchSpacing 25 \
    --physicsList 'FTFP_BERT_ATL' \
    --imf False
cg=$?
echo "art-result: $cg SimDigi_CG"

cd ../; mkdir run_ca; cd run_ca
FastChain_tf.py \
    --CA \
    --simulator ATLFASTIIF_G4MS \
    --digiSteeringConf "Split" \
    --useISF True \
    --randomSeed 123 \
    --enableLooperKiller True \
    --inputEVNTFile ${inputfile} \
    --outputRDOFile RDO_CA.pool.root \
    --maxEvents ${maxevent} \
    --skipEvents 0 \
    --geometryVersion default:ATLAS-R2-2016-01-00-01 \
    --conditionsTag default:OFLCOND-MC16-SDR-16 \
    --preSimExec 'from TrkDetDescrSvc.TrkDetDescrJobProperties import TrkDetFlags;TrkDetFlags.TRT_BuildStrawLayers=True' \
    --preExec 'import InDetRecExample.TrackingCommon as kludgeTheConfig;kludgeTheConfig.use_tracking_geometry_cond_alg=False' \
    --DataRunNumber '284500' \
    --inputHighPtMinbiasHitsFile ${HighPtMinbiasHitsFiles} \
    --inputLowPtMinbiasHitsFile ${LowPtMinbiasHitsFiles} \
    --pileupFinalBunch '6' \
    --numberOfHighPtMinBias '0.116075313' \
    --numberOfLowPtMinBias '44.3839246425' \
    --numberOfCavernBkg 0 \
    --bunchSpacing 25 \
    --physicsList 'FTFP_BERT_ATL' \
    --imf False
ca=$?
echo  "art-result: $ca SimDigi_CA"

diff=999
if [ $cg -eq 0 ] && [ $ca -eq 0 ]
then
   cd ..
   art.py compare ref run_ca/RDO_CA.pool.root run_cg/RDO_CG.pool.root --mode=semi-detailed 
   diff=$?
fi
echo  "art-result: ${diff} regression"
