#!/bin/sh
#
# art-description: G4MS test with pile-up profile
# art-type: grid
# art-include: master/Athena

maxevent=25
inputfile="/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/ISF_Validation/mc12_valid.110401.PowhegPythia_P2012_ttbar_nonallhad.evgen.EVNT.e3099.01517252._000001.pool.root.1"
HighPtMinbiasHitsFiles="/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/Tier0ChainTests/mc16_13TeV.361239.Pythia8EvtGen_A3NNPDF23LO_minbias_inelastic_high.merge.HITS.e4981_s3087_s3089/*"
LowPtMinbiasHitsFiles="/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/Tier0ChainTests/mc16_13TeV.361238.Pythia8EvtGen_A3NNPDF23LO_minbias_inelastic_low.merge.HITS.e4981_s3087_s3089/*"
rdoFile="RDO.pool.root"
aodFile="AOD.pool.root"
ntupFile="physval_g4ms.root"


FastChain_tf.py \
    --simulator ATLFAST3 \
    --useISF True \
    --digiSteeringConf "StandardSignalOnlyTruth" \
    --randomSeed 123 \
    --enableLooperKiller True \
    --physicsList 'FTFP_BERT_ATL' \
    --jobNumber 1 \
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

# RDO -> AOD and AOD -> NTUP stages
rc1_1=999
rc1_2=999
if [ ${rc1} -eq 0 ]
then
    echo "Running Reco_tf.py:  RDO to AOD"
    Reco_tf.py --inputRDOFile ${rdoFile} --maxEvents '-1' \
               --skipEvents '0' --conditionsTag 'default:OFLCOND-MC16-SDR-16' \
               --geometryVersion 'default:ATLAS-R2-2016-01-00-01' \
               --outputAODFile ${aodFile} \
               --preExec 'from RecExConfig.RecFlags import rec;rec.doTrigger.set_Value_and_Lock(False)'\
               --imf False
     rc1_1=$?
     if [ ${rc1_1} -eq 0 ]
     then
	 echo "Running Reco_tf.py:  AOD to NTUP"
         # NTUP prod.
         Reco_tf.py --inputAODFile ${aodFile} --maxEvents '-1' \
                    --outputNTUP_PHYSVALFile ${ntupFile} \
                    --ignoreErrors True \
                    --validationFlags 'doInDet' \
                    --valid 'True'
         rc1_2=$?
     fi
fi
echo "art-result: ${rc1_1} RDOtoAOD"
echo "art-result: ${rc1_2} AODtoNTUP"
