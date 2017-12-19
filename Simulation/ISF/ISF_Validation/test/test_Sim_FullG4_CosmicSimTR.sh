#!/bin/sh
#
# art-description: Run cosmics simulation using ISF with the FullG4 simulator, using TrackRecords as input, using 2015 geometry and conditions
# art-type: grid

Sim_tf.py --conditionsTag 'OFLCOND-RUN12-SDR-19'  --physicsList 'QGSP_BERT' --truthStrategy 'MC12' --simulator 'FullG4' --inputEVNT_COSMICSFile '/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/CommonInputs/Cosmics.TR.pool.root' --outputHITSFile 'test.HITS.pool.root' --maxEvents -1 --randomSeed 1234 --DataRunNumber '10' --geometryVersion 'ATLAS-R2-2015-03-01-00_VALIDATION' --firstEvent 0 --preInclude 'SimulationJobOptions/preInclude.Cosmics.py' --beamType 'cosmics'

ArtPackage=$1
ArtJobName=$2
# TODO This is a regression test I think. We would also need to compare these files to fixed references
art.py compare grid --entries 10 ${ArtPackage} ${ArtJobName}
