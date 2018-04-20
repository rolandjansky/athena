#!/bin/sh

# art-description: DAOD building JETM5 data17
# art-type: grid
# art-output: *.pool.root
# art-output: checkFile.txt
# art-output: checkxAOD.txt

set -e

Reco_tf.py --inputAODFile /cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/DerivationFrameworkART/data17_13TeV.00327057.physics_ZeroBias.merge.AOD.f832_m1812._lb0120-lb0129._0001.1 --outputDAODFile art.pool.root --reductionConf JETM5 --maxEvents -1 --preExec 'rec.doApplyAODFix.set_Value_and_Lock(True);from BTagging.BTaggingFlags import BTaggingFlags;BTaggingFlags.CalibrationTag = "BTagCalibRUN12Onl-08-40" ' 

echo "art-result: $? reco"

DAODMerge_tf.py --inputDAOD_JETM5File DAOD_JETM5.art.pool.root --outputDAOD_JETM5_MRGFile art_merged.pool.root

echo "art-result: $? merge"

checkFile.py DAOD_JETM5.art.pool.root > checkFile.txt

echo "art-result: $?  checkfile"

checkxAOD.py DAOD_JETM5.art.pool.root > checkxAOD.txt

echo "art-result: $?  checkxAOD"
