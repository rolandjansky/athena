#!/bin/sh

# art-include: 21.2/AthDerivation
# art-description: DAOD building FTAG6 validation inputs
# art-type: grid
# art-output: *.pool.root
# art-output: checkFile.txt
# art-output: checkxAOD.txt

set -e

Reco_tf.py --inputAODFile /cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/DerivationFrameworkART/AOD.14795494._005958.pool.root.1 --outputDAODFile art.pool.root --reductionConf FTAG6 --maxEvents 5000 --preExec 'from AthenaCommon.DetFlags import DetFlags;DetFlags.detdescr.all_setOff(); DetFlags.BField_setOn()'

echo "art-result: $? reco"

checkFile.py DAOD_FTAG6.art.pool.root > checkFile.txt

echo "art-result: $?  checkfile"

checkxAOD.py DAOD_FTAG6.art.pool.root > checkxAOD.txt

echo "art-result: $?  checkxAOD"


