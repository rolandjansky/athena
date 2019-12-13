#!/usr/bin/env bash

# art-description: test ttFC_fastSim_fastDigi + ttFC_reco_noSplit_noPseudoT_fastSim_fastDigi
# art-type: grid
# art-include: 21.3/Athena
# art-output: config.txt
# art-output: RAWtoESD_config.txt
# art-output: *.root
# art-output: dcube-rdo-truth
# art-output: dcube-id

inputRefDir="/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/FastChainPileup/DCube-refs/${AtlasBuildBranch}/test_ttFC_reco_noSplit_noPseudoT_fastSim_fastDigi"
inputXmlDir="/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/FastChainPileup/DCube-configs"
art_dcube="/cvmfs/atlas.cern.ch/repo/sw/art/dcube/bin/art-dcube"
dcubeName="ttFC_reco_noSplit_noPseudoT_fastSim_fastDigi"
dcubeXmlID="${inputXmlDir}/dcube_ID.xml"
dcubeRefID="${inputRefDir}/InDetStandardPlots.root"
dcubeXmlRDO="${inputXmlDir}/dcube_RDO_truth_pileup.xml"
dcubeRefRDO="${inputRefDir}/RDO_truth.root"

FastChain_tf.py --simulator ATLFASTIIF_PileUp \
    --digiSteeringConf "SplitNoMergeFF" \
    --useISF True \
    --randomSeed 123 \
    --enableLooperKiller True \
    --inputEVNTFile /cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/FastChainPileup/ttbar_muplusjets-pythia6-7000.evgen.pool.root --outputRDOFile RDO_pileup_fastsim_fastdigi.pool.root \
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
    --preDigiInclude="FastTRT_Digitization/preInclude.FastTRT_Digi.Validation.py" \
    --imf False

rc1=$?
echo "art-result: ${rc1} EVNTtoRDO"
rc2=-9999
if [ ${rc1} -eq 0 ]
then
    bash ${art_dcube} ${dcubeName} RDO_truth.root ${dcubeXmlRDO} ${dcubeRefRDO}
    rc2=$?
    if [ -d "dcube" ]
    then
       mv "dcube" "dcube-rdo-truth"
    fi
fi
echo "art-result: ${rc2} dcubeRDO"

# Reconstruction
FastChain_tf.py --maxEvents 500 \
    --skipEvents 0 \
    --geometryVersion ATLAS-R2-2015-03-01-00 \
    --conditionsTag OFLCOND-RUN12-SDR-31 \
    --inputRDOFile 'RDO_pileup_fastsim_fastdigi.pool.root' \
    --outputAODFile AOD_noSplit_noPseudoT_fastSim_fastDigi.pool.root \
    --preExec "RAWtoESD:rec.doTrigger.set_Value_and_Lock(False);recAlgs.doTrigger.set_Value_and_Lock(False);InDetFlags.pixelClusterSplittingType.set_Value_and_Lock(\"AnalogClus\");InDetFlags.doTIDE_Ambi.set_Value_and_Lock(False);InDetFlags.doStandardPlots.set_Value_and_Lock(True)" \
    --postExec 'RAWtoESD:from AthenaCommon.ConfigurationShelve import saveToAscii;saveToAscii("RAWtoESD_config.txt")' \
    --imf False
rc3=$?
rc4=-9999
rc5=-9999
if [ ${rc3} -eq 0 ]
then
    # Regression test
    ArtPackage=$1
    ArtJobName=$2
    art.py compare grid --entries 10 ${ArtPackage} ${ArtJobName} --mode=summary
    rc4=$?

    # Histogram comparison with DCube
    bash ${art_dcube} ${dcubeName} InDetStandardPlots.root ${dcubeXmlID} ${dcubeRefID}
    rc5=$?
    if [ -d "dcube" ]
    then
       mv "dcube" "dcube-id"
    fi
fi
echo  "art-result: ${rc3} RDOtoAOD"
echo  "art-result: ${rc4} regression"
echo  "art-result: ${rc5} dcubeID"
