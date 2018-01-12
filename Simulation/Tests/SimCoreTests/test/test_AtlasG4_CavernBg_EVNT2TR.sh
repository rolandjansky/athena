#!/bin/sh
#
# art-description: Run simulation outside ISF, reading min bias events, write cavern background track records, using 2015 geometry and conditions
# art-type: grid

AtlasG4_tf.py --inputEVNTFile '/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/SimCoreTests/minbias_Inelastic_low-pythia8-7000.evgen.pool.root' --outputHITSFile 'discard.HITS.pool.root' --outputEVNT_TRFile 'test.EVNT.pool.root' --maxEvents '2' --skipEvents '0' --randomSeed '5678' --geometryVersion 'ATLAS-R2-2015-03-01-00_VALIDATION' --conditionsTag 'OFLCOND-RUN12-SDR-19' --DataRunNumber '222525' --physicsList 'QGSP_BERT_HP' --postInclude 'PyJobTransforms/UseFrontier.py'


ArtPackage=$1
ArtJobName=$2
# TODO This is a regression test I think. We would also need to compare these files to fixed references
art.py compare grid --entries 10 ${ArtPackage} ${ArtJobName}
