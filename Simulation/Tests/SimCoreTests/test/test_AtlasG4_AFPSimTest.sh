#!/bin/sh
#
# art-description: Tests ATLAS + AFP simulation, generating events on-the-fly
# art-type: grid

# FIXME need an AFP-specific single particle generator configuration
AtlasG4_tf.py --preInclude 'G4AtlasTests/preInclude.ALFA_SingleParticle.py' --outputHITSFile 'test.HITS.pool.root' --maxEvents '3' --randomSeed '10' --geometryVersion 'ATLAS-R2-2015-03-01-00_VALIDATION' --conditionsTag 'OFLCOND-RUN12-SDR-19' --DataRunNumber '222525' --physicsList 'FTFP_BERT' --AFPOn 'True' --postInclude 'PyJobTransforms/UseFrontier.py'

ArtPackage=$1
ArtJobName=$2
# TODO This is a regression test I think. We would also need to compare these files to fixed references and run DCube
+art.py compare grid --entries 10 ${ArtPackage} ${ArtJobName}
