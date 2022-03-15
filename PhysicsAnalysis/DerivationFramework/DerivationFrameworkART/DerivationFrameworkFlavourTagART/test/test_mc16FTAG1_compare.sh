#!/bin/sh

# art-include: 21.2/AthDerivation
# art-description: DAOD compare to ref FTAG1 mc16
# art-type: grid
# art-output: *.pool.root

set -e

Reco_tf.py --inputAODFile /cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/DerivationFrameworkART/AOD.14795494._005958.pool.root.1 --outputDAODFile art.pool.root --reductionConf FTAG1 --maxEvents 500 --preExec 'rec.doApplyAODFix.set_Value_and_Lock(True) ' 

echo "art-result: $? reco"

cp DAOD_FTAG1.art.pool.root DAOD.pool.root

art-diff.py ./DAOD.pool.root /cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/DerivationFrameworkART/ReferenceFiles/21.2.34.0/mc16_13TeV.361023.e3668_s3126_r10201/DAOD_ftag1.mc16_dijet.pool.root --diff-type=diff-pool 

echo  "art-result: $? diff-pool"

rm -f DAOD.pool.root
