#!/bin/sh
#
# art-description: Run simulation outside ISF, using 2012 geometry and conditions, reading single muon events, writing HITS
# art-type: grid

AtlasG4_tf.py --inputEVNTFile '/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/SimCoreTests/mu_E200_eta0-25.evgen.pool.root' --outputHITSFile 'test.HITS.pool.root' --maxEvents '10' --skipEvents '0' --randomSeed '10' --geometryVersion 'ATLAS-R1-2012-03-00-00' --conditionsTag 'OFLCOND-RUN12-SDR-19' --DataRunNumber '212272' --physicsList 'FTFP_BERT' --postInclude 'PyJobTransforms/UseFrontier.py'

ArtPackage=$1
ArtJobName=$2
# TODO This is a regression test I think. We would also need to compare these files to fixed references
art.py compare grid --entries 10 ${ArtPackage} ${ArtJobName}
