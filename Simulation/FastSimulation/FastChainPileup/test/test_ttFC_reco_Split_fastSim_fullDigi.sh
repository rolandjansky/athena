#!/usr/bin/env bash

# art-description: test for job configuration ttFC_fastSim_fulldigi + ttFC_reco_Split_fastSim_fullDigi
# art-type: grid
# art-include: 21.3/Athena
# art-output: config.txt
# art-output: RAWtoESD_config.txt
# art-output: *.root
# art-output: dcube-rdo-truth
# art-output: dcube-id

inputRefDir="/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/FastChainPileup/DCube-refs/${AtlasBuildBranch}/test_ttFC_reco_Split_fastSim_fullDigi"
inputXmlDir="/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/FastChainPileup/DCube-configs"
art_dcube="/cvmfs/atlas.cern.ch/repo/sw/art/dcube/bin/art-dcube"
dcubeName="ttFC_reco_Split_fastSim_fullDigi"
dcubeXmlID="${inputXmlDir}/dcube_indetplots.xml"
dcubeRefID="${inputRefDir}/InDetStandardPlots.root"
dcubeXmlRDO="${inputXmlDir}/dcube_RDO_truth_pileup.xml"
dcubeRefRDO="${inputRefDir}/RDO_truth.root"

FastChain_tf.py --simulator ATLFASTIIF_PileUp \
    --digiSteeringConf "SplitNoMerge" --useISF True \
    --randomSeed 123 \
    --enableLooperKiller True \
    --inputEVNTFile /cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/FastChainPileup/ttbar_muplusjets-pythia6-7000.evgen.pool.root \
    --outputRDOFile RDO_pileup_fastsim_fulldigi.pool.root \
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


echo "art-result: $? EVNTtoRDO"

FastChain_tf.py --maxEvents 500 \
    --skipEvents 0 \
    --geometryVersion ATLAS-R2-2015-03-01-00 \
    --conditionsTag OFLCOND-RUN12-SDR-31  \
    --inputRDOFile 'RDO_pileup_fastsim_fulldigi.pool.root' \
    --outputAODFile AOD_Split_fastSim_fullDigi.pool.root \
    --preExec "RAWtoESD:from InDetRecExample.InDetJobProperties import InDetFlags;InDetFlags.doStandardPlots.set_Value_and_Lock(True);InDetFlags.doSplitReco.set_Value_and_Lock(True);InDetFlags.doTrackSegmentsTRT.set_Value_and_Lock(True); from InDetRecExample.InDetKeys import InDetKeys; InDetKeys.PixelPUClusters.set_Value_and_Lock('PixelFast_PU_Clusters');InDetKeys.PixelPUClustersTruth.set_Value_and_Lock('PRD_MultiTruthPixel_PU');InDetKeys.SCT_PU_ClustersTruth.set_Value_and_Lock('PRD_MultiTruthSCT_PU');InDetKeys.TRT_PU_DriftCirclesTruth.set_Value_and_Lock('PRD_MultiTruthTRT_PU');rec.doTrigger.set_Value_and_Lock(False);recAlgs.doTrigger.set_Value_and_Lock(False);" \
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
