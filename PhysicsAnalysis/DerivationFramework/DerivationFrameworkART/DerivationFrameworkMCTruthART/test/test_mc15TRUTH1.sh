#!/bin/sh

# art-include: master/AthDerivation
# art-description: DAOD building TRUTH1 mc15
# art-type: grid
# art-output: *.pool.root
# art-output: checkFile.txt
# art-output: checkxAOD.txt
# art-output: checkIndexRefs.txt

set -e

Derivation_tf.py --CA --inputEVNTFile /cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/DerivationFrameworkART/mc16_13TeV.410637.PhPy8EG_A14_ttbar_hdamp258p75_nonallhad_2000_14000.merge.EVNT.e6685_e5984/EVNT.15803543._000001.pool.root.1 --outputDAODFile art.pool.root --formats TRUTH1 --maxEvents 1000

echo "art-result: $? reco"

checkFile.py DAOD_TRUTH1.art.pool.root > checkFile.txt

echo "art-result: $?  checkfile"

checkxAOD.py DAOD_TRUTH1.art.pool.root > checkxAOD.txt

echo "art-result: $?  checkxAOD"

checkIndexRefs.py DAOD_TRUTH1.art.pool.root > checkIndexRefs.txt

echo "art-result: $?  checkIndexRefs"
