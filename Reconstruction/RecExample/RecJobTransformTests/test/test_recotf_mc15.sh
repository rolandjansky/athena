#!/bin/sh
#
# art-description: David Rousseau
# art-athena-mt: 4
# art-type: build
# art-include: 21.0/Athena
# art-include: 21.0-TrigMC/Athena
# art-include: master/Athena
# art-include: 21.3/Athena
# art-include: 21.9/Athena


export TRF_ECHO=True; Reco_tf.py --inputRDOFile /cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/RecJobTransformTests/RDO.pool.root --maxEvents 5 --autoConfiguration everything --preExec 'rec.doDetailedAuditor=True' 'rec.doNameAuditor=True' --outputESDFile my.ESD.pool.root

RES=$?
echo "art-result: $RES Reco"


