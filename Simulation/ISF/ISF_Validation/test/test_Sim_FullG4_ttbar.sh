#!/bin/sh
#
# art-description: Run simulation  using ISF with the FullG4 simulator, reading ttbar events, writing HITS, using 2015 geometry and conditions
# art-type: grid

AtlasG4_tf.py --inputEVNTFile '/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/CommonInputs/ttbar_muplusjets-pythia6-7000.evgen.pool.root' --outputHITSFile 'test.HITS.pool.root' --maxEvents '4' --skipEvents '0' --randomSeed '10' --geometryVersion 'ATLAS-R2-2015-03-01-00_VALIDATION' --conditionsTag 'OFLCOND-RUN12-SDR-19' --DataRunNumber '222525' --physicsList 'FTFP_BERT' --postInclude 'PyJobTransforms/UseFrontier.py' 'AtlasG4Tf:G4AtlasTests/postInclude.DCubeTest.py' --preExec 'AtlasG4Tf:simFlags.ReleaseGeoModel=False;'

ArtPackage=$1
ArtJobName=$2
# TODO This is a regression test I think. We would also need to compare these files to fixed references
art.py compare grid --entries 10 ${ArtPackage} ${ArtJobName}
