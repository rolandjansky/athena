#!/usr/bin/env bash

# art-description: test job ttFC_fullSim_fullDigi + ttFC_reco_newTracking_PseudoT_fullSim_fullDigi
# art-type: grid
# art-include: master/Athena
# art-output: config.txt
# art-output: *.root
# art-output: dcube-rdo-truth
# art-output: dcube-id
# art-html: dcube-id

inputRefDir="/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/FastChainPileup/DCube-refs/${AtlasBuildBranch}/test_ttFC_reco_newTracking_PseudoT_fullSim_fullDigi"
inputXmlDir="/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/FastChainPileup/DCube-configs/${AtlasBuildBranch}"
dcubeXmlID="${inputXmlDir}/physval-newTracking_PseudoT_fullSim_fullDigi.xml"
dcubeRefID="${inputRefDir}/physval-newTracking_PseudoT_fullSim_fullDigi.root"
dcubeXmlRDO="${inputXmlDir}/dcube_RDO_truth_compare.xml"
dcubeRefRDO="${inputRefDir}/RDO_truth.root"


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
rc1=$?
echo "art-result: ${rc1} EVNTtoRDO"

rc2=-9999
if [ ${rc1} -eq 0 ]
then
    # Histogram comparison with DCube
    $ATLAS_LOCAL_ROOT/dcube/current/DCubeClient/python/dcube.py \
    -p -x dcube-rdo-truth \
    -c ${dcubeXmlRDO} -r ${dcubeRefRDO} RDO_truth.root
    rc2=$?
fi
echo "art-result: ${rc2} dcubeRDO"

# Reconstruction
FastChain_tf.py --maxEvents 500 \
    --skipEvents 0 \
    --geometryVersion ATLAS-R2-2015-03-01-00 \
    --conditionsTag OFLCOND-RUN12-SDR-31  \
    --inputRDOFile RDO_pileup_fullsim_fulldigi.pool.root \
    --outputAODFile AOD_newTracking_pseudoTracking_fullSim_fullDigi.pool.root \
    --preExec "RAWtoESD:from InDetRecExample.InDetJobProperties import InDetFlags;InDetFlags.doPseudoTracking.set_Value_and_Lock(True);InDetFlags.doNewTracking.set_Value_and_Lock(True);rec.doTrigger.set_Value_and_Lock(False);recAlgs.doTrigger.set_Value_and_Lock(False);InDetFlags.doTrackSegmentsTRT.set_Value_and_Lock(True);" \
    --outputNTUP_PHYSVALFile 'physval-newTracking_PseudoT_fullSim_fullDigi.root' \
    --validationFlags 'doInDet' \
    --valid 'True' \
    --imf False
rc3=$?

rc4=-9999
rc5=-9999
if [ ${rc1} -eq 0 ]
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
-c ${dcubeXmlID} -r ${dcubeRefID} physval-newTracking_PseudoT_fullSim_fullDigi.root
rc5=$?


echo  "art-result: ${rc3} RDOtoAOD"
echo  "art-result: ${rc4} regression"
echo  "art-result: ${rc5} dcubeID"
