#!/usr/bin/env bash
#
# art-description: test job ttFC_fullSim_fullDigi + ttFC_reco_noSplit_noPseudoT_fullSim_fullDigi
# art-type: grid
# specify branches of athena that are being targeted:

# art-include: 21.0/Athena
# art-include: 21.3/Athena
# art-output: config.txt
# art-output: RAWtoESD_config.txt

FastChain_tf.py --simulator ATLFASTII \
    --digiSteeringConf "SplitNoMerge" \
    --useISF True \
    --randomSeed 123 \
    --enableLooperKiller True \
    --inputEVNTFile /cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/FastChainPileup/ttbar_muplusjets-pythia6-7000.evgen.pool.root \
    --outputRDOFile RDO_pileup_fullsim_fulldigi.pool.root \
    --maxEvents 50 \
    --skipEvents 0 \
    --geometryVersion ATLAS-R2-2015-03-01-00 \
    --conditionsTag OFLCOND-RUN12-SDR-31 \
    --preSimExec 'from TrkDetDescrSvc.TrkDetDescrJobProperties import TrkDetFlags;TrkDetFlags.TRT_BuildStrawLayers=True' \
    --postInclude='PyJobTransforms/UseFrontier.py,G4AtlasTests/postInclude.DCubeTest.py,DigitizationTests/postInclude.RDO_Plots.py' \
    --postExec 'from AthenaCommon.ConfigurationShelve import saveToAscii;saveToAscii("config.txt")' \
    --DataRunNumber '284500' \
    --imf False

echo "art-result: $? EVNTtoRDO step"

FastChain_tf.py --maxEvents 500 \
    --skipEvents 0 \
    --geometryVersion ATLAS-R2-2015-03-01-00 \
    --conditionsTag OFLCOND-RUN12-SDR-31 \
    --inputRDOFile RDO_pileup_fullsim_fulldigi.pool.root \
    --outputAODFile AOD_noSplit_noPseudoT_fullSim_fullDigi.pool.root \
    --preExec "RAWtoESD:rec.doTrigger.set_Value_and_Lock(False);recAlgs.doTrigger.set_Value_and_Lock(False);" "InDetFlags.doStandardPlots.set_Value_and_Lock(True)" \
    --postExec 'RAWtoESD:from AthenaCommon.ConfigurationShelve import saveToAscii;saveToAscii("RAWtoESD_config.txt")' \
    --imf False

echo "art-result: $? RDOtoAOD step"

ArtPackage=$1
ArtJobName=$2
art.py compare grid --entries 10 ${ArtPackage} ${ArtJobName}
echo  "art-result: $? regression"
#add an additional payload from the job (corollary file).
# art-output: InDetStandardPlots.root
/cvmfs/atlas.cern.ch/repo/sw/art/dcube/bin/art-dcube TEST_ttFC_reco_noSplit_noPseudoT_fullSim_fullDigi InDetStandardPlots.root /cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/FastChainPileup/dcube_configs/config/InDetStandardPlotCompare.xml /cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/FastChainPileup/InDetStandardPlots_Refs/test_ttFC_reco_noSplit_noPseudoT_fullSim_fullDigi_InDetStandardPlots.root




# InDetStandardPlots.root -l dcube.log -p -r   -x dcube.xml -s /cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/FastChainPileup/

# art-output: dcube/
echo  "art-result: $? histcomp"
