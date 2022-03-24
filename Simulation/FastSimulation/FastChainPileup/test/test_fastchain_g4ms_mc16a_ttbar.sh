#!/bin/sh
#
# art-description: Run AFII simulation and full digitization of an MC16a ttbar sample with 2016a geometry and conditions, 25ns pile-up
# art-type: grid
# art-include: master/Athena
# art-output: *.root
# art-output: config.txt
# art-output: dcube*
# art-html: dcube-id


HighPtMinbiasHitsFiles="/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/Tier0ChainTests/mc16_13TeV.361239.Pythia8EvtGen_A3NNPDF23LO_minbias_inelastic_high.merge.HITS.e4981_s3087_s3089/*"
LowPtMinbiasHitsFiles="/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/Tier0ChainTests/mc16_13TeV.361238.Pythia8EvtGen_A3NNPDF23LO_minbias_inelastic_low.merge.HITS.e4981_s3087_s3089/*"
inputRefDir="/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/FastChainPileup/DCube-refs/${AtlasBuildBranch}/test_fastchain_g4ms_mc16a_ttbar"
inputXmlDir="/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/FastChainPileup/DCube-configs/${AtlasBuildBranch}"
dcubeXmlRDO="${inputXmlDir}/dcube_RDO_truth_g4ms.xml"
dcubeRefRDO="${inputRefDir}/RDO_truth.root"
dcubeXmlID="${inputXmlDir}/dcube_physval_g4ms.xml"
dcubeRefID="${inputRefDir}/physval_g4ms.root"
rdoFile="RDO.pool.root"
aodFile="AOD.pool.root"
ntupFile="physval_g4ms.root"

FastChain_tf.py \
    --simulator ATLFASTIIF_G4MS \
    --digiSteeringConf "Split" \
    --useISF True \
    --randomSeed 123 \
    --enableLooperKiller True \
    --inputEVNTFile /cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/ISF_Validation/mc12_valid.110401.PowhegPythia_P2012_ttbar_nonallhad.evgen.EVNT.e3099.01517252._000001.pool.root.1 \
    --outputRDOFile ${rdoFile} \
    --maxEvents 25 \
    --skipEvents 0 \
    --geometryVersion default:ATLAS-R2-2016-01-00-01 \
    --conditionsTag default:OFLCOND-MC16-SDR-RUN2-09 \
    --preSimExec 'from TrkDetDescrSvc.TrkDetDescrJobProperties import TrkDetFlags;TrkDetFlags.TRT_BuildStrawLayers=True;from Digitization.DigitizationFlags import digitizationFlags;digitizationFlags.experimentalDigi=["NewMerge"]' \
    --preExec 'EVNTtoRDO:ToolSvc.NewMergeMcEventCollTool.OutputLevel=VERBOSE;' \
    --postInclude='PyJobTransforms/UseFrontier.py,G4AtlasTests/postInclude.DCubeTest_FCpileup.py,DigitizationTests/postInclude.RDO_Plots.py' \
    --postExec 'from AthenaCommon.ConfigurationShelve import saveToAscii;saveToAscii("config.txt");ServiceMgr.MessageSvc.Format = "% F%32W%S%7W%R%T %0W%M"' \
    --DataRunNumber '284500' \
    --inputHighPtMinbiasHitsFile ${HighPtMinbiasHitsFiles} \
    --inputLowPtMinbiasHitsFile ${LowPtMinbiasHitsFiles} \
    --pileupFinalBunch '6' \
    --numberOfHighPtMinBias '0.116075313' \
    --numberOfLowPtMinBias '44.3839246425' \
    --numberOfCavernBkg 0 \
    --imf False
rc=$?
echo  "art-result: $rc EVNTtoRDO"

rc1=999
rc2=999
rc3=999
rc4=999
rc5=999
if [ ${rc} -eq 0 ]
then
    # Histogram comparison with DCube
    $ATLAS_LOCAL_ROOT/dcube/current/DCubeClient/python/dcube.py \
    -p -x dcube-rdo-truth \
    -c ${dcubeXmlRDO} -r ${dcubeRefRDO} RDO_truth.root
    rc1=$?

    Reco_tf.py --inputRDOFile ${rdoFile} --maxEvents '-1' \
               --skipEvents '0' --conditionsTag 'default:OFLCOND-MC16-SDR-RUN2-09' \
               --geometryVersion 'default:ATLAS-R2-2016-01-00-01' \
               --outputAODFile ${aodFile} \
               --steering 'doRDO_TRIG' \
               --athenaopts "all:--threads=1" \
               --imf False
     rc2=$?
     if [ ${rc2} -eq 0 ]
     then
         # NTUP prod.
         Reco_tf.py --inputAODFile ${aodFile} --maxEvents '-1' \
                    --outputNTUP_PHYSVALFile ${ntupFile} \
                    --ignoreErrors True \
                    --validationFlags 'doInDet' \
                    --valid 'True'
         rc3=$?

         # Regression test
         ArtPackage=$1
         ArtJobName=$2
         art.py compare grid --entries 10 ${ArtPackage} ${ArtJobName} --mode=summary
         rc4=$?

         if [ ${rc3} -eq 0 ]
         then
             # Histogram comparison with DCube
             $ATLAS_LOCAL_ROOT/dcube/current/DCubeClient/python/dcube.py \
             -p -x dcube-id \
             -c ${dcubeXmlID} -r ${dcubeRefID} ${ntupFile}
             rc5=$?
         fi
     fi
fi
echo  "art-result: ${rc1} dcubeRDO"
echo  "art-result: ${rc2} RDOtoAOD"
echo  "art-result: ${rc3} AODtoNTUP"
echo  "art-result: ${rc4} regression"
echo  "art-result: ${rc5} dcubeID"
