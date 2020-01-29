#!/usr/bin/env bash

# art-description: test job ttFC_fullSim_fullDigi
# art-type: grid
# art-include: 21.3/Athena
# art-output: config.txt
# art-output: RAWtoESD_config.txt
# art-output: *.root
# art-output: dcube-rdo-truth
# art-output: dcube-id

inputRefDir="/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/FastChainPileup/DCube-refs/${AtlasBuildBranch}/test_stdReco_fullSim_fullDigi"
inputXmlDir="/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/FastChainPileup/DCube-configs/${AtlasBuildBranch}"
art_dcube="/cvmfs/atlas.cern.ch/repo/sw/art/dcube/bin/art-dcube"
dcubeName="stdReco_fullSim_fullDigi"
dcubeXmlID="${inputXmlDir}/dcube_ID.xml"
dcubeRefID="${inputRefDir}/InDetStandardPlots.root"
dcubeXmlRDO="${inputXmlDir}/RDOTruthCompare.xml"
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
    bash ${art_dcube} ${dcubeName} RDO_truth.root ${dcubeXmlRDO} ${dcubeRefRDO}
    rc2=$?
    if [ -d "dcube" ]
    then
       mv "dcube" "dcube-rdo-truth"
    fi
fi
echo "art-result: ${rc2} dcubeRDO"

Reco_tf.py --inputRDOFile=RDO_pileup_fullsim_fulldigi.pool.root \
    --outputAODFile=AOD_fullSim_fullDigi.pool.root \
    --autoConfiguration=everything \
    --maxEvents=500 \
    --preExec "RAWtoESD:rec.doTrigger.set_Value_and_Lock(False);recAlgs.doTrigger.set_Value_and_Lock(False);InDetFlags.doStandardPlots.set_Value_and_Lock(True)" \
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
