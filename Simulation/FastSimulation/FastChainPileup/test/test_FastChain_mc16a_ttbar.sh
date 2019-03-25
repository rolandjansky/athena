#!/bin/sh
#
# art-description: Run AFII simulation and full digitization of an MC16a ttbar sample with 2016a geometry and conditions, 25ns pile-up
# art-type: grid
# art-include: 21.3/Athena
# art-output: mc16a_ttbar.RDO.pool.root
# art-output: config.txt

HighPtMinbiasHitsFiles="/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/Tier0ChainTests/mc16_13TeV.361239.Pythia8EvtGen_A3NNPDF23LO_minbias_inelastic_high.merge.HITS.e4981_s3087_s3089/*"
LowPtMinbiasHitsFiles="/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/Tier0ChainTests/mc16_13TeV.361238.Pythia8EvtGen_A3NNPDF23LO_minbias_inelastic_low.merge.HITS.e4981_s3087_s3089/*"


FastChain_tf.py \
    --simulator ATLFASTII \
    --digiSteeringConf "Split" \
    --useISF True \
    --randomSeed 123 \
    --enableLooperKiller True \
    --inputEVNTFile /cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/ISF_Validation/mc12_valid.110401.PowhegPythia_P2012_ttbar_nonallhad.evgen.EVNT.e3099.01517252._000001.pool.root.1 \
    --outputRDOFile RDO.pool.root \
    --maxEvents 10 \
    --skipEvents 0 \
    --geometryVersion default:ATLAS-R2-2016-01-00-01 \
    --conditionsTag default:OFLCOND-MC16-SDR-16 \
    --preSimExec 'from TrkDetDescrSvc.TrkDetDescrJobProperties import TrkDetFlags;TrkDetFlags.TRT_BuildStrawLayers=True;from Digitization.DigitizationFlags import digitizationFlags;digitizationFlags.experimentalDigi=["NewMerge"]' \
    --preExec 'EVNTtoRDO:ToolSvc.NewMergeMcEventCollTool.OutputLevel=VERBOSE;' \
    --postInclude='PyJobTransforms/UseFrontier.py' \
    --postExec 'from AthenaCommon.ConfigurationShelve import saveToAscii;saveToAscii("config.txt");ServiceMgr.MessageSvc.Format = "% F%32W%S%7W%R%T %0W%M"' \
    --DataRunNumber '284500' \
    --inputHighPtMinbiasHitsFile ${HighPtMinbiasHitsFiles} \
    --inputLowPtMinbiasHitsFile ${LowPtMinbiasHitsFiles} \
    --pileupFinalBunch '6' \
    --numberOfHighPtMinBias '0.116075313' \
    --numberOfLowPtMinBias '44.3839246425' \
    --numberOfCavernBkg 0 \
    --imf False
echo "art-result: $? EVNTtoRDO step"

# Add Reco step?

ArtPackage=$1
ArtJobName=$2
art.py compare grid --entries 10 ${ArtPackage} ${ArtJobName}
echo  "art-result: $? regression"

