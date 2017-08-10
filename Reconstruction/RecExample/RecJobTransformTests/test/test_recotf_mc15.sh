#!/bin/sh
#
# art-description: David Rousseau
# art-type: build


export TRF_ECHO=True; Reco_tf.py --inputRDOFile /cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/RecJobTransformTests/RDO.pool.root --maxEvents 5 --autoConfiguration everything --preExec 'rec.doDetailedAuditor=True' 'rec.doNameAuditor=True' --outputESDFile my.ESD.pool.root

