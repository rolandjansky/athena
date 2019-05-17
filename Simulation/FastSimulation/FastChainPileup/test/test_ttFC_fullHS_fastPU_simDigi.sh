#!/usr/bin/env bash
#
# art-description: ttFC_fullHS_fastPU_simDigi
# art-type: grid
#
# Run FastChain 'Fast PU, Full HS' and tests: G4HS_FastPileup sim (G4 for HS, Pythia on the fly + FastCaloSim for PU) + fast digi PU/full digi HS + Split reco (truth tracking PU, full HS)
#
# specify branches of athena that are being targeted: 
# art-include: 21.3/Athena
#                                                                           

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
    --postExec 'RAWtoESD:from AthenaCommon.ConfigurationShelve import saveToAscii;saveToAscii("config.txt")' \
    --DataRunNumber '284500' \
    --postSimExec='genSeq.Pythia8.NCollPerEvent=10;' \
    --preDigiInclude="FastTRT_Digitization/preInclude.FastTRT_Digi.Validation.py"

echo "art-result: $? Job"
ArtPackage=$1
ArtJobName=$2
art.py compare grid --imf=False --entries 10 ${ArtPackage} ${ArtJobName}  
echo  "art-result: $? regression"
#add an additional payload from the job (corollary file).                                                           
# art-output: InDetStandardPlots.root  
/cvmfs/atlas.cern.ch/repo/sw/art/dcube/bin/art-dcube TEST_ttFC_fullHS_fastPU InDetStandardPlots.root /cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/FastChainPileup/dcube_configs/config/dcube_indetplots_no_pseudotracks.xml /cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/FastChainPileup/InDetStandardPlots_TEST.root

# art-output: dcube/dcube.xml
# art-output: dcube/dcube.log
# art-output: dcube/dcubelog.xml
# art-output: dcube/dcube.xml.php
echo  "art-result: $? histcomp"
