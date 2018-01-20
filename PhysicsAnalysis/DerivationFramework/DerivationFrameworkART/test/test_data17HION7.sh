#!/bin/sh

# art-description: DAOD building HION7 data17
# art-type: grid
# art-output: *.pool.root
# art-output: checkFile.txt
# art-output: checkxAOD.txt

set -e

Reco_tf.py --inputAODFile /cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/DerivationFrameworkART/data17_hi.00338037.physics_MinBias.merge.AOD.f900_m1912._lb0292._0009.1 --outputDAODFile art.pool.root --reductionConf HION7 --maxEvents -1 --preExec 'rec.doApplyAODFix.set_Value_and_Lock(True);from BTagging.BTaggingFlags import BTaggingFlags;BTaggingFlags.CalibrationTag = "BTagCalibRUN12Onl-08-40" ' 

DAODMerge_tf.py --inputDAOD_HION7File DAOD_HION7.art.pool.root --outputDAOD_HION7_MRGFile art_merged.pool.root
checkFile.py DAOD_HION7.art.pool.root > checkFile.txt
checkxAOD.py DAOD_HION7.art.pool.root > checkxAOD.txt