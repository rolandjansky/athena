#!/bin/sh
#
# art-description: CA vs Legacy code diff (AFII_G4MS with pileup profile)
# art-type: grid
# art-include: master/Athena
# art-include: 22.0/Athena
# art-include: 22.0-mc20/Athena
# art-output: run_*
# art-output: pkldiff.log
# art-architecture: '#x86_64-intel'

maxevent=25
inputfile="/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/ISF_Validation/mc12_valid.110401.PowhegPythia_P2012_ttbar_nonallhad.evgen.EVNT.e3099.01517252._000001.pool.root.1"
HighPtMinbiasHitsFiles="/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/Tier0ChainTests/mc16_13TeV.361239.Pythia8EvtGen_A3NNPDF23LO_minbias_inelastic_high.merge.HITS.e4981_s3087_s3089/*"
LowPtMinbiasHitsFiles="/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/Tier0ChainTests/mc16_13TeV.361238.Pythia8EvtGen_A3NNPDF23LO_minbias_inelastic_low.merge.HITS.e4981_s3087_s3089/*"

mkdir ./run_cg_pkl; cd run_cg_pkl
FastChain_tf.py \
    --simulator ATLFASTIIF_G4MS \
    --useISF True \
    --digiSteeringConf "StandardSignalOnlyTruth" \
    --randomSeed 123 \
    --enableLooperKiller True \
    --physicsList 'FTFP_BERT_ATL' \
    --jobNumber 1 \
    --digiSeedOffset1 '1' \
    --digiSeedOffset2 '2' \
    --inputEVNTFile ${inputfile} \
    --outputRDOFile RDO_CG.pool.root \
    --maxEvents ${maxevent} \
    --skipEvents 0 \
    --geometryVersion default:ATLAS-R2-2016-01-00-01 \
    --conditionsTag default:OFLCOND-MC16-SDR-RUN2-09 \
    --preSimExec 'from TrkDetDescrSvc.TrkDetDescrJobProperties import TrkDetFlags;TrkDetFlags.TRT_BuildStrawLayers=True;' \
    --preSimInclude 'Campaigns/MC16a.py' 'Campaigns/PileUpMC16a.py' \
    --postInclude='PyJobTransforms/UseFrontier.py' \
    --postExec 'ServiceMgr.MessageSvc.Format = "% F%32W%S%7W%R%T %0W%M";from IOVDbSvc.CondDB import conddb;conddb.addOverride("/TILE/OFL02/CALIB/SFR","TileOfl02CalibSfr-SIM-05")' \
    --inputHighPtMinbiasHitsFile ${HighPtMinbiasHitsFiles} \
    --inputLowPtMinbiasHitsFile ${LowPtMinbiasHitsFiles} \
    --pileupFinalBunch '6' \
    --numberOfHighPtMinBias '0.116075313' \
    --numberOfLowPtMinBias '44.3839246425' \
    --numberOfCavernBkg 0 \
    --athenaopts '"--config-only=ConfigCG.pkl"' \
    --imf False

cgpkl=999
if [ -f "ConfigCG.pkl" ]; then
    cgpkl=0
fi
echo "art-result: $cgpkl EVNTtoRDO_CG_PKL"

cd ..; mkdir ./run_cg; cd run_cg
FastChain_tf.py \
    --simulator ATLFASTIIF_G4MS \
    --useISF True \
    --digiSteeringConf "StandardSignalOnlyTruth" \
    --randomSeed 123 \
    --enableLooperKiller True \
    --physicsList 'FTFP_BERT_ATL' \
    --jobNumber 1 \
    --digiSeedOffset1 '1' \
    --digiSeedOffset2 '2' \
    --inputEVNTFile ${inputfile} \
    --outputRDOFile RDO_CG.pool.root \
    --maxEvents ${maxevent} \
    --skipEvents 0 \
    --geometryVersion default:ATLAS-R2-2016-01-00-01 \
    --conditionsTag default:OFLCOND-MC16-SDR-RUN2-09 \
    --preSimExec 'from TrkDetDescrSvc.TrkDetDescrJobProperties import TrkDetFlags;TrkDetFlags.TRT_BuildStrawLayers=True;' \
    --preSimInclude 'Campaigns/MC16a.py' 'Campaigns/PileUpMC16a.py' \
    --postInclude='PyJobTransforms/UseFrontier.py' \
    --postExec 'ServiceMgr.MessageSvc.Format = "% F%32W%S%7W%R%T %0W%M";from IOVDbSvc.CondDB import conddb;conddb.addOverride("/TILE/OFL02/CALIB/SFR","TileOfl02CalibSfr-SIM-05")' \
    --inputHighPtMinbiasHitsFile ${HighPtMinbiasHitsFiles} \
    --inputLowPtMinbiasHitsFile ${LowPtMinbiasHitsFiles} \
    --pileupFinalBunch '6' \
    --numberOfHighPtMinBias '0.116075313' \
    --numberOfLowPtMinBias '44.3839246425' \
    --numberOfCavernBkg 0 \
    --imf False
cg=$?
echo "art-result: $cg EVNTtoRDO_CG"

cd ../; mkdir run_ca; cd run_ca
FastChain_tf.py \
    --CA \
    --simulator ATLFASTIIF_G4MS \
    --useISF True \
    --digiSteeringConf "StandardSignalOnlyTruth" \
    --randomSeed 123 \
    --enableLooperKiller True \
    --physicsList 'FTFP_BERT_ATL' \
    --jobNumber 1 \
    --digiSeedOffset1 '1' \
    --digiSeedOffset2 '2' \
    --inputEVNTFile ${inputfile} \
    --outputRDOFile RDO_CA.pool.root \
    --maxEvents ${maxevent} \
    --skipEvents 0 \
    --geometryVersion default:ATLAS-R2-2016-01-00-01 \
    --conditionsTag default:OFLCOND-MC16-SDR-RUN2-09 \
    --preSimExec 'from TrkDetDescrSvc.TrkDetDescrJobProperties import TrkDetFlags;TrkDetFlags.TRT_BuildStrawLayers=True;' \
    --preInclude 'Campaigns.MC16a' \
    --postInclude 'PyJobTransforms.UseFrontier' \
    --inputHighPtMinbiasHitsFile ${HighPtMinbiasHitsFiles} \
    --inputLowPtMinbiasHitsFile ${LowPtMinbiasHitsFiles} \
    --pileupFinalBunch '6' \
    --numberOfHighPtMinBias '0.116075313' \
    --numberOfLowPtMinBias '44.3839246425' \
    --numberOfCavernBkg 0 \
    --postExec 'with open("ConfigCA.pkl", "wb") as f: cfg.store(f);from IOVDbSvc.IOVDbSvcConfig import addOverride;cfg.merge(addOverride(ConfigFlags,"/TILE/OFL02/CALIB/SFR","TileOfl02CalibSfr-SIM-05"))' \
    --imf False
ca=$?
echo  "art-result: $ca EVNTtoRDO_CA"
cd ..

diff=999
pkldiff=999
if [ $cg -eq 0 ] && [ $ca -eq 0 ]
then
   confTool.py --diff --ignoreIrrelevant --shortenDefaultComponents --ignoreDefaults run_cg_pkl/ConfigCG.pkl run_ca/ConfigCA.pkl > pkldiff.log
   pkldiff=$(grep -o 'differ' pkldiff.log | wc -l)

   art.py compare ref run_ca/RDO_CA.pool.root run_cg/RDO_CG.pool.root --mode=semi-detailed --entries 10
   diff=$?
fi
echo  "art-result: ${pkldiff} pklDiff"
echo  "art-result: ${diff} regression"
