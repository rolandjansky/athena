#!/bin/sh
#
# art-description: Run simulation outside ISF, reading min bias events, write cavern background track records, using 2015 geometry and conditions
# art-include: 21.0/Athena
# art-include: 21.0/AthSimulation
# art-include: 21.3/Athena
# art-include: 21.9/Athena
# art-include: master/Athena
# art-include: master/AthSimulation
# art-type: grid
# art-output: test.EVNT.pool.root

AtlasG4_tf.py \
--inputEVNTFile '/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/SimCoreTests/minbias_Inelastic_low-pythia8-7000.evgen.pool.root' \
--outputHITSFile 'discard.HITS.pool.root' \
--outputEVNT_TRFile 'test.EVNT.pool.root' \
--maxEvents '2' \
--skipEvents '0' \
--randomSeed '5678' \
--geometryVersion 'ATLAS-R2-2015-03-01-00_VALIDATION' \
--conditionsTag 'OFLCOND-RUN12-SDR-19' \
--DataRunNumber '222525' \
--physicsList 'QGSP_BERT_HP' \
--postInclude 'PyJobTransforms/UseFrontier.py' \
--imf False

rc=$?

# remove HITS file so it does not trigger regression
rm discard.HITS.pool.root

rc2=-9999
echo  "art-result: $rc simulation"
if [ $rc -eq 0 ]
then
    ArtPackage=$1
    ArtJobName=$2
    art.py compare grid --entries 2 ${ArtPackage} ${ArtJobName} --mode=semi-detailed --diff-root --file=*EVNT.pool.root
    rc2=$?
fi

echo  "art-result: $rc2 regression"
