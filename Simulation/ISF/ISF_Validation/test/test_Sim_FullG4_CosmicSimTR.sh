#!/bin/sh
#
# art-description: Run cosmics simulation using ISF with the FullG4 simulator, using TrackRecords as input, using 2015 geometry and conditions
# art-include: 21.0/Athena
# art-include: 21.3/Athena
# art-include: 21.9/Athena
# art-include: master/Athena
# art-type: grid
# art-output: test.HITS.pool.root

Sim_tf.py \
--conditionsTag 'OFLCOND-RUN12-SDR-19'  \
--physicsList 'QGSP_BERT' \
--truthStrategy 'MC15aPlus' \
--simulator 'FullG4' \
--inputEVNT_TRFile '/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/ISF_Validation/Cosmics.TR.pool.root' \
--outputHITSFile 'test.HITS.pool.root' \
--maxEvents -1 \
--randomSeed 1234 \
--DataRunNumber '10' \
--geometryVersion 'ATLAS-R2-2015-03-01-00' \
--firstEvent 0 \
--preInclude 'SimulationJobOptions/preInclude.Cosmics.py' \
--beamType 'cosmics' \
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
