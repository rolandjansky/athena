#!/usr/bin/env bash

# art-description: test for job configuration ttFC_fastSim_fulldigi (Sim/Digi job)
# art-type: grid
# art-include: 21.3/Athena
# art-output: config.txt
# art-output: *.root
# art-output: dcube-rdo-truth

FastChain_tf.py --simulator ATLFASTIIF_PileUp \
    --digiSteeringConf "SplitNoMerge" \
    --useISF True \
    --randomSeed 123 \
    --enableLooperKiller True \
    --inputEVNTFile /cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/FastChainPileup/ttbar_muplusjets-pythia6-7000.evgen.pool.root --outputRDOFile RDO_pileup_fastsim_fulldigi.pool.root \
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
rc=$?
echo  "art-result: ${rc} EVNTtoRDO"

inputRefDir="/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/FastChainPileup/DCube-refs/${AtlasBuildBranch}/test_ttFC_fastSim_fullDigi"
inputXmlDir="/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/FastChainPileup/DCube-configs"
art_dcube="/cvmfs/atlas.cern.ch/repo/sw/art/dcube/bin/art-dcube"
dcubeName="ttFC_fastSim_fullDigi"
dcubeXmlRDO="${inputXmlDir}/dcube_RDO_truth.xml"
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
