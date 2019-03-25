#!/usr/bin/env bash

# art-description: ttFC_fullHS_fastPU_simDigi
# art-type: grid
# specify branches of athena that are being targeted:
# art-include: 21.0/Athena
# art-include: 21.3/Athena
# art-output: config.txt
# art-output: RAWtoESD_config.txt
# Run FastChain 'Fast PU, Full HS' and tests: G4HS_FastPileup sim (G4 for HS, Pythia on the fly + FastCaloSim for PU) + fast digi PU/full digi HS + Split reco (truth tracking PU, full HS)

FastChain_tf.py --simulator G4HS_FastPileup \
    --digiSteeringConf "SplitNoMergeSF" \
    --useISF True \
    --randomSeed 123 \
    --enableLooperKiller True \
    --inputEVNTFile /cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/FastChainPileup/ttbar_muplusjets-pythia6-7000.evgen.pool.root --outputRDOFile RDO_fullHS_fastPU_simdigi.pool.root \
    --maxEvents 10 \
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

echo "art-result: $? EVNTtoRDO step"

FastChain_tf.py --maxEvents 10 \
    --skipEvents 0 \
    --geometryVersion ATLAS-R2-2015-03-01-00 \
    --conditionsTag OFLCOND-RUN12-SDR-31  \
    --inputRDOFile 'RDO_fullHS_fastPU_simdigi.pool.root' \
    --outputAODFile AOD_Split_fullHS_fastPU_simDigi.pool.root \
    --preExec "RAWtoESD:rec.doTrigger.set_Value_and_Lock(False);InDetFlags.doStatistics.set_Value_and_Lock(False);recAlgs.doTrigger.set_Value_and_Lock(False);InDetFlags.pixelClusterSplittingType.set_Value_and_Lock(\"AnalogClus\");InDetFlags.doTIDE_Ambi.set_Value_and_Lock(False);InDetFlags.doStandardPlots.set_Value_and_Lock(True);InDetFlags.doSplitReco.set_Value_and_Lock(True);from InDetRecExample.InDetKeys import InDetKeys; InDetKeys.PixelPUClusters.set_Value_and_Lock('PixelFast_PU_Clusters');InDetKeys.SCT_PU_ClustersTruth.set_Value_and_Lock('PRD_MultiTruthSCT_PU');InDetKeys.TRT_PU_DriftCirclesTruth.set_Value_and_Lock('PRD_MultiTruthTRT_PU');InDetFlags.doSplitReco.set_Value_and_Lock(True);InDetFlags.doTrackSegmentsTRT.set_Value_and_Lock(True); from InDetRecExample.InDetKeys import InDetKeys; InDetKeys.PixelPUClusters.set_Value_and_Lock('PixelFast_PU_Clusters');InDetKeys.PixelPUClustersTruth.set_Value_and_Lock('PRD_MultiTruthPixel_PU');InDetKeys.SCT_PU_ClustersTruth.set_Value_and_Lock('PRD_MultiTruthSCT_PU');InDetKeys.TRT_PU_DriftCirclesTruth.set_Value_and_Lock('PRD_MultiTruthTRT_PU');" \
    --postExec 'RAWtoESD:import AthenaCommon.AlgSequence as acas;job = acas.AlgSequence();del job.InDetSCT_ClusterizationPU;del job.InDetPixelClusterizationPU;del job.InDetPRD_MultiTruthMakerSiPU;del job.InDetPRD_MultiTruthMakerTRTPU;from AthenaCommon.ConfigurationShelve import saveToAscii;saveToAscii("RAWtoESD_config.txt")' \
    --imf False

echo "art-result: $? RDOtoAOD step"
ArtPackage=$1
ArtJobName=$2
art.py compare grid --entries 10 ${ArtPackage} ${ArtJobName}
echo  "art-result: $? regression"
#add an additional payload from the job (corollary file).
# art-output: InDetStandardPlots.root
/cvmfs/atlas.cern.ch/repo/sw/art/dcube/bin/art-dcube TEST_ttFC_reco_Split_fullHS_fastPU_simDigi InDetStandardPlots.root /cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/FastChainPileup/dcube_configs/config/RDOTruthCompare.xml_ttFC_reco_Split_fullHS_fastPU_simDigi /cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/FastChainPileup/InDetStandardPlots_Refs/test_ttFC_reco_Split_fullHS_fastPU_simDigi_InDetStandardPlots.root




# InDetStandardPlots.root -l dcube.log -p -r   -x dcube.xml -s /cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/FastChainPileup/

# art-output: dcube/
# dumps the entire dcube directory to output because we need all of it for plots
echo  "art-result: $? histcomp"
