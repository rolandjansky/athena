#!/bin/sh
#
# art-description: Run simulation outside ISF, using Frozen Showers to speed up FCAL simulation, using 2015 geometry and conditions
# art-type: grid

AtlasG4_tf.py --inputEVNTFile '/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/SimCoreTests/e_E50_eta34_49.EVNT.pool.root' --outputHITSFile 'test.HITS.pool.root' --maxEvents 10 --skipEvents 0 --randomSeed 10 --geometryVersion 'ATLAS-R2-2015-03-01-00_VALIDATION' --conditionsTag 'OFLCOND-RUN12-SDR-19' --DataRunNumber '222525' --physicsList 'FTFP_BERT' --preInclude 'SimulationJobOptions/preInclude.FrozenShowersFCalOnly.py' --postInclude 'PyJobTransforms/UseFrontier.py'

ArtPackage=$1
ArtJobName=$2
# TODO This is a regression test I think. We would also need to compare these files to fixed references
art.py compare grid --entries 10 ${ArtPackage} ${ArtJobName}
