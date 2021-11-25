#!/bin/sh
#
# art-description: G4MS test with pile-up profile
# art-type: grid
# art-include: master/Athena
# art-output: run-*

maxevent=25
inputfile="/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/ISF_Validation/mc12_valid.110401.PowhegPythia_P2012_ttbar_nonallhad.evgen.EVNT.e3099.01517252._000001.pool.root.1"
HighPtMinbiasHitsFiles="/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/Tier0ChainTests/mc16_13TeV.361239.Pythia8EvtGen_A3NNPDF23LO_minbias_inelastic_high.merge.HITS.e4981_s3087_s3089/*"
LowPtMinbiasHitsFiles="/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/Tier0ChainTests/mc16_13TeV.361238.Pythia8EvtGen_A3NNPDF23LO_minbias_inelastic_low.merge.HITS.e4981_s3087_s3089/*"


mkdir run-wopostexec; cd run-wopostexec
FastChain_tf.py \
    --simulator ATLFASTIIF_G4MS \
    --useISF True \
    --digiSteeringConf "StandardSignalOnlyTruth" \
    --randomSeed 123 \
    --enableLooperKiller True \
    --physicsList 'FTFP_BERT_ATL' \
    --jobNumber 1 \
    --bunchSpacing 25 \
    --digiSeedOffset1 '1' \
    --digiSeedOffset2 '2' \
    --inputEVNTFile ${inputfile} \
    --outputRDOFile RDO.pool.root \
    --maxEvents ${maxevent} \
    --skipEvents 0 \
    --geometryVersion default:ATLAS-R2-2016-01-00-01 \
    --conditionsTag default:OFLCOND-MC16-SDR-16 \
    --preSimExec 'from TrkDetDescrSvc.TrkDetDescrJobProperties import TrkDetFlags;TrkDetFlags.TRT_BuildStrawLayers=True;from ISF_Config.ISF_jobProperties import ISF_Flags;ISF_Flags.UseTrackingGeometryCond=False' \
    --preSimInclude 'Campaigns/MC16a.py' 'Campaigns/PileUpMC16a.py' \
    --postInclude='PyJobTransforms/UseFrontier.py' \
    --postExec 'ServiceMgr.MessageSvc.Format = "% F%32W%S%7W%R%T %0W%M"' \
    --inputHighPtMinbiasHitsFile ${HighPtMinbiasHitsFiles} \
    --inputLowPtMinbiasHitsFile ${LowPtMinbiasHitsFiles} \
    --pileupFinalBunch '6' \
    --numberOfHighPtMinBias '0.116075313' \
    --numberOfLowPtMinBias '44.3839246425' \
    --numberOfCavernBkg 0 \
    --imf False
rc1=$?
echo  "art-result: ${rc1} EVNTtoRDO"

cd ..
mkdir run-withpostexec; cd run-withpostexec
FastChain_tf.py \
    --simulator ATLFASTIIF_G4MS \
    --useISF True \
    --digiSteeringConf "StandardSignalOnlyTruth" \
    --randomSeed 123 \
    --enableLooperKiller True \
    --physicsList 'FTFP_BERT_ATL' \
    --jobNumber 1 \
    --bunchSpacing 25 \
    --digiSeedOffset1 '1' \
    --digiSeedOffset2 '2' \
    --inputEVNTFile ${inputfile} \
    --outputRDOFile RDO.pool.root \
    --maxEvents ${maxevent} \
    --skipEvents 0 \
    --geometryVersion default:ATLAS-R2-2016-01-00-01 \
    --conditionsTag default:OFLCOND-MC16-SDR-16 \
    --preSimExec 'from TrkDetDescrSvc.TrkDetDescrJobProperties import TrkDetFlags;TrkDetFlags.TRT_BuildStrawLayers=True;from ISF_Config.ISF_jobProperties import ISF_Flags;ISF_Flags.UseTrackingGeometryCond=False' \
    --preSimInclude 'Campaigns/MC16a.py' 'Campaigns/PileUpMC16a.py' \
    --postInclude='PyJobTransforms/UseFrontier.py' \
    --postExec 'ServiceMgr.EventSelector.FirstLB=1;ServiceMgr.EventSelector.InitialTimeStamp=1446539425;ServiceMgr.EventSelector.OverrideRunNumber=True;ServiceMgr.EventSelector.OverrideRunNumberFromInput=True;ServiceMgr.EventSelector.RunNumber=284500;ServiceMgr.MessageSvc.Format = "% F%32W%S%7W%R%T %0W%M"' \
    --inputHighPtMinbiasHitsFile ${HighPtMinbiasHitsFiles} \
    --inputLowPtMinbiasHitsFile ${LowPtMinbiasHitsFiles} \
    --pileupFinalBunch '6' \
    --numberOfHighPtMinBias '0.116075313' \
    --numberOfLowPtMinBias '44.3839246425' \
    --numberOfCavernBkg 0 \
    --imf False

cd ..
rc2=$?
echo  "art-result: ${rc2} "EVNTtoRDO-withpostExec""

if [[ ${rc1} -eq 0 && ${rc2} -eq 0 ]]
then
  art.py compare ref run-wopostexec/RDO.pool.root run-withpostexec/RDO.pool.root --mode=summary --entries 10
  echo  "art-result: $? diff-root"
fi
