#!/bin/sh

# art-include
# art-description: DAOD building MUON2 mc16
# art-type: grid
# art-output: *.pool.root
# art-output: checkFile.txt
# art-output: checkxAOD.txt

set -e

Reco_tf.py --inputAODFile /cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/DerivationFrameworkART/AOD.12169019._004055.pool.root.1 --outputDAODFile art.pool.root --reductionConf MUON2 --maxEvents -1 --preExec 'rec.doApplyAODFix.set_Value_and_Lock(True);from BTagging.BTaggingFlags import BTaggingFlags;BTaggingFlags.CalibrationTag = "BTagCalibRUN12-08-40" ' 

echo "art-result: $? reco"

DAODMerge_tf.py --inputDAOD_MUON2File DAOD_MUON2.art.pool.root --outputDAOD_MUON2_MRGFile art_merged.pool.root

echo "art-result: $? merge"

checkFile.py DAOD_MUON2.art.pool.root > checkFile.txt

echo "art-result: $?  checkfile"

checkxAOD.py DAOD_MUON2.art.pool.root > checkxAOD.txt

echo "art-result: $?  checkxAOD"
