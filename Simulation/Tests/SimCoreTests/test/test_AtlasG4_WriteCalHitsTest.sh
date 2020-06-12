#!/bin/sh
#
# art-description: Reading in single particle gen events, writing out full CaloCalibrationHit information, using 2015 geometry and conditions
# art-include: 21.0/Athena
# art-include: 21.3/Athena
# art-include: 21.9/Athena
# art-include: master/Athena
# art-type: grid
# art-output: test.HITS.pool.root
# art-output: truth.root

AtlasG4_tf.py \
--inputEVNTFile '/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/SimCoreTests/pi_E50_eta0-60.evgen.pool.root' \
--outputHITSFile 'test.HITS.pool.root' \
--maxEvents '10' \
--skipEvents '0' \
--randomSeed '10' \
--geometryVersion 'ATLAS-R2-2015-03-01-00_VALIDATION' \
--conditionsTag 'OFLCOND-RUN12-SDR-19' \
--DataRunNumber '222525' \
--physicsList 'FTFP_BERT' \
--preInclude 'SimulationJobOptions/preInclude.CalHits.py,SimulationJobOptions/preInclude.ParticleID.py' \
--postInclude 'PyJobTransforms/UseFrontier.py' 'AtlasG4Tf:G4AtlasTests/postInclude.DCubeTest_CaloCalibHits.py' \
--preExec 'AtlasG4Tf:simFlags.ReleaseGeoModel=False;' \
--imf False

rc=$?
rc2=-9999
echo  "art-result: $rc simulation"
if [ $rc -eq 0 ]
then
    ArtPackage=$1
    ArtJobName=$2
    art.py compare grid --entries 10 ${ArtPackage} ${ArtJobName} --mode=semi-detailed
    rc2=$?
fi

echo  "art-result: $rc2 regression"
