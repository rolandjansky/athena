#!/usr/bin/env bash

# art-description: test ttFC_fastSim_fastDigi + ttFC_reco_noSplit_noPseudoT_fastSim_fastDigi
# art-type: grid
# art-include: master/Athena
# art-output: config.txt
# art-output: RAWtoESD_config.txt
# art-output: *.root

# RDO input from 21.3/Nov 13 Nightly test_ttFC_reco_noSplit_noPseudoT_fastSim_fastDigi.sh
FastChain_tf.py --maxEvents 500 \
    --skipEvents 0 \
    --geometryVersion ATLAS-R2-2015-03-01-00 \
    --conditionsTag OFLCOND-RUN12-SDR-31 \
    --inputRDOFile '/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/FastChainPileup/RecoInputRefs/RDO_pileup_fastsim_fastdigi.pool.root' \
    --outputAODFile AOD_noSplit_noPseudoT_fastSim_fastDigi.pool.root \
    --preExec "all:rec.doTrigger.set_Value_and_Lock(False);recAlgs.doTrigger.set_Value_and_Lock(False);InDetFlags.doPixelClusterSplitting.set_Value_and_Lock(False);InDetFlags.doTIDE_Ambi.set_Value_and_Lock(False);InDetFlags.doStandardPlots.set_Value_and_Lock(True)" \
    --postExec 'RAWtoESD:from AthenaCommon.ConfigurationShelve import saveToAscii;saveToAscii("RAWtoESD_config.txt");ToolSvc.InDetPixelClusterOnTrackTool.ErrorStrategy=1' \
    --imf False

rc=$?
rc2=-9999
echo  "art-result: $rc RDOtoAOD"
if [ $rc -eq 0 ]
then
    ArtPackage=$1
    ArtJobName=$2
    art.py compare grid --entries 10 ${ArtPackage} ${ArtJobName} --mode=summary
    rc2=$?
fi

echo  "art-result: $rc2 regression"
