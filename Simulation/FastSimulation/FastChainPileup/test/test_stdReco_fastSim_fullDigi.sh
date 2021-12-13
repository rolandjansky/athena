#!/usr/bin/env bash

# art-description: test for job configuration ttFC_fastSim_fulldigi (Sim/Digi job) + stdReco_fastSim_fullDigi
# art-type: grid
# art-include: master/Athena
# art-output: config.txt
# art-output: RAWtoESD_config.txt
# art-output: *.root
# art-output: dcube-rdo-truth
# art-output: dcube-id
# art-html: dcube-id

inputRefDir="/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/FastChainPileup/DCube-refs/${AtlasBuildBranch}/test_stdReco_fastSim_fullDigi"
inputXmlDir="/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/FastChainPileup/DCube-configs/${AtlasBuildBranch}"
dcubeXmlID="${inputXmlDir}/physval-stdReco_fastSim_fullDigi.xml"
dcubeRefID="${inputRefDir}/physval-stdReco_fastSim_fullDigi.root"
dcubeXmlRDO="${inputXmlDir}/dcube_RDO_truth.xml"
dcubeRefRDO="${inputRefDir}/RDO_truth.root"

rdoFile="RDO_pileup_fastsim_fulldigi.pool.root"
aodFile="AOD_fastSim_fullDigi.pool.root"
ntupFile="physval-stdReco_fastSim_fullDigi.root"

FastChain_tf.py --simulator ATLFASTIIF_PileUp \
    --digiSteeringConf "SplitNoMerge" \
    --useISF True \
    --randomSeed 123 \
    --enableLooperKiller True \
    --inputEVNTFile /cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/FastChainPileup/ttbar_muplusjets-pythia6-7000.evgen.pool.root \
    --outputRDOFile ${rdoFile} \
    --maxEvents 100 \
    --skipEvents 0 \
    --geometryVersion ATLAS-R2-2015-03-01-00 \
    --conditionsTag OFLCOND-RUN12-SDR-31 \
    --preSimExec 'from TrkDetDescrSvc.TrkDetDescrJobProperties import TrkDetFlags;TrkDetFlags.TRT_BuildStrawLayers=True' \
    --preSimInclude FastChainPileup/FastPileup.py \
    --postInclude='PyJobTransforms/UseFrontier.py,G4AtlasTests/postInclude.DCubeTest_FCpileup.py,DigitizationTests/postInclude.RDO_Plots.py' \
    --postExec 'from AthenaCommon.ConfigurationShelve import saveToAscii;saveToAscii("config.txt")' \
    --DataRunNumber '284500' \
    --postSimExec='genSeq.Pythia8.NCollPerEvent=10;' \
    --imf False
rc=$?
echo "art-result: ${rc} EVNTtoRDO"

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
               --autoConfiguration=everything \
               --outputAODFile ${aodFile} \
               --steering 'doRDO_TRIG' \
               --athenaopts "all:--threads=1" \
               --postExec 'RAWtoESD:from AthenaCommon.ConfigurationShelve import saveToAscii;saveToAscii("RAWtoESD_config.txt")' \
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
