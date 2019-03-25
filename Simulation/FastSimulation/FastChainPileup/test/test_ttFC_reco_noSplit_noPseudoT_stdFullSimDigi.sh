#!/usr/bin/env bash
# art-description: Job ttFC_stdFullSim + ttFC_stdFullSimMerge + ttFC_stdFullSimDigi + ttFC_reco_noSplit_noPseudoT_stdFullSimDigi
# art-type: grid
# 

# specify branches of athena that are being targeted:

# art-include: 21.3/Athena
# job 1: Simulation from evgen

Sim_tf.py --conditionsTag 'default:OFLCOND-RUN12-SDR-19' \
    --physicsList 'FTFP_BERT' \
    --truthStrategy 'MC15aPlus' \
    --simulator 'FullG4' \
    --postInclude 'default:RecJobTransforms/UseFrontier.py,G4AtlasTests/postInclude.DCubeTest.py' \
    --preInclude 'EVNTtoHITS:SimulationJobOptions/preInclude.BeamPipeKill.py,SimulationJobOptions/preInclude.FrozenShowersFCalOnly.py' \
    --DataRunNumber '222525' \
    --geometryVersion 'default:ATLAS-R2-2015-03-01-00_VALIDATION' \
    --inputEVNTFile "/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/FastChainPileup/mc12_valid.110401.PowhegPythia_P2012_ttbar_nonallhad.evgen.EVNT.e3099.01517252._000001.pool.root.1" \
    --outputHITSFile "Hits.pool.root" \
    --maxEvents 50

echo "art-result: $? evgen step"

#merging of hits file

HITSMerge_tf.py --inputHITSFile='Hits.pool.root' \
    --outputHITS_MRGFile='Merge.pool.root' \
    --maxEvents=50 \
    --skipEvents='0' \
    --geometryVersion 'ATLAS-R2-2015-03-01-00' \
    --conditionsTag 'OFLCOND-RUN12-SDR-19'

echo "art-result: $? Merge step"
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
    --conditionsTag 'OFLCOND-RUN12-SDR-31'

echo "art-result: $? Digi step"

FastChain_tf.py --maxEvents 50 \
    --skipEvents 0 \
    --geometryVersion ATLAS-R2-2015-03-01-00 \
    --conditionsTag OFLCOND-RUN12-SDR-31 \
    --inputRDOFile RDO.pool.root \
    --outputAODFile AOD_Split_stdFullSimDigi.pool.root \
    --preExec "RAWtoESD:rec.doTrigger.set_Value_and_Lock(False);recAlgs.doTrigger.set_Value_and_Lock(False);from InDetRecExample.InDetJobProperties import InDetFlags;InDetFlags.doStandardPlots.set_Value_and_Lock(True);"
#end of job
echo "art-result: $? AOD step"
#add an additional payload from the job (corollary file).
# art-output: InDetStandardPlots.root

ArtPackage=$1
ArtJobName=$2
art.py compare grid --entries 10 --imf=False ${ArtPackage} ${ArtJobName}
echo  "art-result: $? regression"


/cvmfs/atlas.cern.ch/repo/sw/art/dcube/bin/art-dcube TEST_ttFC_reco_noSplit_noPseudoT_stdFullSimDigi InDetStandardPlots.root /cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/FastChainPileup/dcube_configs/config/dcube_indetplots_no_pseudotracks.xml /cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/FastChainPileup/InDetStandardPlots_TEST.root




# InDetStandardPlots.root -l dcube.log -p -r   -x dcube.xml -s /cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/FastChainPileup/

# art-output: dcube/dcube.xml
# art-output: dcube/dcube.log
# art-output: dcube/dcubelog.xml
# art-output: dcube/dcube.xml.php

echo  "art-result: $? histcomp test"
