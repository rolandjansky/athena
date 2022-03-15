#!/bin/sh

# art-include: 21.2/AthDerivation
# art-description: DAOD building BPHY10 mc16
# art-type: grid
# art-output: *.pool.root
# art-output: checkFile.txt
# art-output: checkxAOD.txt

set -e

Reco_tf.py --inputAODFile /cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/DerivationFrameworkART/AOD.14795494._005958.pool.root.1 --outputDAODFile art.pool.root --reductionConf BPHY10 --maxEvents -1 --preExec 'rec.doApplyAODFix.set_Value_and_Lock(True) ' 

echo "art-result: $? reco"

DAODMerge_tf.py --inputDAOD_BPHY10File DAOD_BPHY10.art.pool.root --outputDAOD_BPHY10_MRGFile art_merged.pool.root

echo "art-result: $? merge"

checkFile.py DAOD_BPHY10.art.pool.root > checkFile.txt

echo "art-result: $?  checkfile"

checkxAOD.py DAOD_BPHY10.art.pool.root > checkxAOD.txt

echo "art-result: $?  checkxAOD"
