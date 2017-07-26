#!/bin/sh
#
# art-description: Reading in single particle gen events, writing out full CaloCalibrationHit information, using 2015 geometry and conditions
# art-type: grid

#TODO Need to copy input file to CVMFS
AtlasG4_tf.py --inputEVNTFile '/afs/cern.ch/atlas/offline/ProdData/16.6.X/16.6.7.Y/pi_E50_eta0-60.evgen.pool.root' --outputHITSFile 'test.HITS.pool.root' --maxEvents '10' --skipEvents '0' --randomSeed '10' --geometryVersion 'ATLAS-R2-2015-03-01-00_VALIDATION' --conditionsTag 'OFLCOND-RUN12-SDR-19' --DataRunNumber '222525' --physicsList 'FTFP_BERT' --preInclude 'SimulationJobOptions/preInclude.CalHits.py,SimulationJobOptions/preInclude.ParticleID.py' --postInclude 'PyJobTransforms/UseFrontier.py' 'AtlasG4Tf:G4AtlasTests/postInclude.DCubeTest_CaloCalibHits.py' --preExec 'AtlasG4Tf:simFlags.ReleaseGeoModel=False;'

#TODO This is a regression test I think. We would also need to compare these files to fixed references and run DCube
art.py compare grid $NIGHTLY_RELEASE $PROJECT $PLATFORM $NIGHTLY_TAG $PACKAGE $TEST_NAME test.HITS.pool.root
