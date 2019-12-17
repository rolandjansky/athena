#!/usr/bin/env bash
#
# art-description: ttFC_fullHS_fastPU_simDigi
# art-type: grid
# art-include: 21.3/Athena
# art-output: config.txt
# art-output: *.root
# art-output: dcube-rdo-truth

# Run FastChain 'Fast PU, Full HS' and tests: G4HS_FastPileup sim (G4 for HS, Pythia on the fly + FastCaloSim for PU) + fast digi PU/full digi HS + Split reco (truth tracking PU, full HS)
FastChain_tf.py --simulator G4HS_FastPileup \
    --digiSteeringConf "SplitNoMergeSF" \
    --useISF True \
    --randomSeed 123 \
    --enableLooperKiller True \
    --inputEVNTFile /cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/FastChainPileup/ttbar_muplusjets-pythia6-7000.evgen.pool.root \
    --outputRDOFile RDO_fullHS_fastPU_simdigi.pool.root \
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
rc=$?
echo  "art-result: ${rc} EVNTtoRDO"

inputRefDir="/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/FastChainPileup/DCube-refs/${AtlasBuildBranch}/test_ttFC_fullHS_fastPU_simDigi"
inputXmlDir="/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/FastChainPileup/DCube-configs"
art_dcube="/cvmfs/atlas.cern.ch/repo/sw/art/dcube/bin/art-dcube"
dcubeName="ttFC_fullHS_fastPU_simDigi"
dcubeXmlRDO="${inputXmlDir}/dcube_RDO_truth_pileup.xml"
dcubeRefRDO="${inputRefDir}/RDO_truth.root"

rc2=-9999
rc3=-9999
if [ ${rc} -eq 0 ]
then
    # Regression test
    ArtPackage=$1
    ArtJobName=$2
    echo ArtJobName
    echo $ArtJobName
    art.py compare grid --entries 10 ${ArtPackage} ${ArtJobName} --mode=summary
    rc2=$?

    # Histogram comparison with DCube
    bash ${art_dcube} ${dcubeName} RDO_truth.root ${dcubeXmlRDO} ${dcubeRefRDO}
    rc3=$?
    if [ -d "dcube" ]
    then
       mv "dcube" "dcube-rdo-truth"
    fi

fi
echo  "art-result: $rc2 regression"
echo  "art-result: $rc3 dcubeRDO"
