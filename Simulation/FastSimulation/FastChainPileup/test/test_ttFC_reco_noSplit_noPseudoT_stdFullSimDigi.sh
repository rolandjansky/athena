#!/usr/bin/env bash

# art-description: Job ttFC_stdFullSim + ttFC_stdFullSimMerge + ttFC_stdFullSimDigi + ttFC_reco_noSplit_noPseudoT_stdFullSimDigi
# art-type: grid
# art-include: 21.3/Athena
# art-include: master/Athena
# art-output: *.root
# art-output: dcube-truth
# art-output: dcube-id

inputRefDir="/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/FastChainPileup/DCube-refs/${AtlasBuildBranch}/test_ttFC_reco_noSplit_noPseudoT_stdFullSimDigi"
inputXmlDir="/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/FastChainPileup/DCube-configs"
art_dcube="/cvmfs/atlas.cern.ch/repo/sw/art/dcube/bin/art-dcube"
dcubeName="ttFC_reco_noSplit_noPseudoT_stdFullSimDigi"
dcubeXmlID="${inputXmlDir}/dcube_indetplots.xml"
dcubeRefID="${inputRefDir}/InDetStandardPlots.root"
dcubeXmlTruth="${inputXmlDir}/dcube_truth.xml"
dcubeRefTruth="${inputRefDir}/truth.root"


Sim_tf.py --conditionsTag 'default:OFLCOND-RUN12-SDR-19' \
    --physicsList 'FTFP_BERT' \
    --truthStrategy 'MC15aPlus' \
    --simulator 'FullG4' \
    --postInclude 'default:RecJobTransforms/UseFrontier.py,G4AtlasTests/postInclude.DCubeTest.py' \
    --preInclude 'EVNTtoHITS:SimulationJobOptions/preInclude.BeamPipeKill.py,SimulationJobOptions/preInclude.FrozenShowersFCalOnly.py' \
    --DataRunNumber '222525' \
    --geometryVersion 'default:ATLAS-R2-2015-03-01-00_VALIDATION' \
    --inputEVNTFile "/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/FastChainPileup/mc12_valid.110401.PowhegPythia_P2012_ttbar_nonallhad.evgen.EVNT.e3099.01517252._000001.pool.root.1" \
    --outputHITSFile "HITS.pool.root" \
    --maxEvents 50 \
    --imf False

rc1=$?
echo "art-result: ${rc1} EVNTtoHITS"
rc2=-9999
if [ ${rc1} -eq 0 ]
then
    bash ${art_dcube} ${dcubeName} truth.root ${dcubeXmlTruth} ${dcubeRefTruth}
    rc2=$?
    if [ -d "dcube" ]
    then
       mv "dcube" "dcube-truth"
    fi
fi
echo "art-result: ${rc2} dcubeTruth"


# Merging of hits file
HITSMerge_tf.py --inputHITSFile='HITS.pool.root' \
    --outputHITS_MRGFile='Merge.pool.root' \
    --maxEvents=50 \
    --skipEvents='0' \
    --geometryVersion 'ATLAS-R2-2015-03-01-00' \
    --conditionsTag 'OFLCOND-RUN12-SDR-19' \
    --imf False

echo "art-result: $? HITSMerge"
#digi
Digi_tf.py --inputHITSFile 'Merge.pool.root' \
    --outputRDOFile 'RDO.pool.root' \
    --maxEvents '50' \
    --skipEvents '0' \
    --geometryVersion 'ATLAS-R2-2015-03-01-00' \
    --digiSeedOffset1 '123456' \
    --digiSeedOffset2 '2345678' \
    --postInclude 'PyJobTransforms/UseFrontier.py' \
    --AddCaloDigi 'False' \
    --conditionsTag 'OFLCOND-RUN12-SDR-31' \
    --imf False

echo "art-result: $? HITStoRDO"

FastChain_tf.py --maxEvents 50 \
    --skipEvents 0 \
    --geometryVersion ATLAS-R2-2015-03-01-00 \
    --conditionsTag OFLCOND-RUN12-SDR-31 \
    --inputRDOFile RDO.pool.root \
    --outputAODFile AOD_Split_stdFullSimDigi.pool.root \
    --preExec "RAWtoESD:rec.doTrigger.set_Value_and_Lock(False);recAlgs.doTrigger.set_Value_and_Lock(False);from InDetRecExample.InDetJobProperties import InDetFlags;InDetFlags.doStandardPlots.set_Value_and_Lock(True);" \
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
