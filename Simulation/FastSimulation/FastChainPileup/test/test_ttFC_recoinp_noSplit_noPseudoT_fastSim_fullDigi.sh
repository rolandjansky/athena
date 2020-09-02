#!/usr/bin/env bash

# art-description: test for job configuration ttFC_fastSim_fulldigi _ttFC_reco_noSplit_noPseudoT_fastSim_fullDigi
# art-type: grid
# art-include: master/Athena
# art-output: config.txt
# art-output: RAWtoESD_config.txt
# art-output: *.root
# art-output: dcube-id
# art-html: dcube-id

inputRefDir="/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/FastChainPileup/DCube-refs/${AtlasBuildBranch}/test_ttFC_recoinp_noSplit_noPseudoT_fastSim_fullDigi"
inputXmlDir="/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/FastChainPileup/DCube-configs/${AtlasBuildBranch}"
dcubeXmlID="${inputXmlDir}/physval-noSplit_noPseudoT_fastSim_fullDigi.xml"
dcubeRefID="${inputRefDir}/physval-noSplit_noPseudoT_fastSim_fullDigi.root"

# RDO input from 21.3/Apr 27 Nightly test_ttFC_reco_noSplit_noPseudoT_fastSim_fullDigi.sh
FastChain_tf.py --maxEvents 500 \
    --skipEvents 0 \
    --geometryVersion ATLAS-R2-2015-03-01-00 \
    --conditionsTag OFLCOND-RUN12-SDR-31 \
    --inputRDOFile '/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/FastChainPileup/RecoInputRefs/RDO_pileup_fastsim_fulldigi.pool.root' \
    --outputAODFile AOD_noSplit_noPseudoT_fastSim_fullDigi.pool.root \
    --preExec "all:rec.doTrigger.set_Value_and_Lock(False);recAlgs.doTrigger.set_Value_and_Lock(False);" \
    --postExec 'RAWtoESD:from AthenaCommon.ConfigurationShelve import saveToAscii;saveToAscii("RAWtoESD_config.txt")' \
    --outputNTUP_PHYSVALFile 'physval-noSplit_noPseudoT_fastSim_fullDigi.root' \
    --validationFlags 'doInDet' \
    --valid 'True' \
    --imf False
rc3=$?

rc4=-9999
rc5=-9999
# return to this check after physval returns proper exit_code
# if [ ${rc3} -eq 0 ]
if [ -f "AOD_noSplit_noPseudoT_fastSim_fullDigi.pool.root" ]
then
    # Regression test
    ArtPackage=$1
    ArtJobName=$2
    art.py compare grid --entries 10 ${ArtPackage} ${ArtJobName} --mode=summary
    rc4=$?
fi

# Histogram comparison with DCube
$ATLAS_LOCAL_ROOT/dcube/current/DCubeClient/python/dcube.py \
-p -x dcube-id \
-c ${dcubeXmlID} -r ${dcubeRefID} physval-noSplit_noPseudoT_fastSim_fullDigi.root
rc5=$?


echo  "art-result: ${rc3} RDOtoAOD"
echo  "art-result: ${rc4} regression"
echo  "art-result: ${rc5} dcubeID"
