#!/bin/sh
#
# art-description: heavy ion reconstruction test from Andrzej Olszewski and Iwona Grabowska-Bold
# art-type: grid

export TRF_ECHO=True; Reco_tf.py --inputBSFile /cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/RecJobTransformTests/data11_hi.00193321.physics_HardProbes.merge.RAW._lb0050._SFO-9._0002.1 --outputESDFile ESD.pool.root --outputAODFile AOD.pool.root --conditionsTag COMCOND-BLKPA-RUN1-06 --geometryVersion ATLAS-R1-2011-02-00-00 --autoConfiguration=everything --maxEvents='25'

