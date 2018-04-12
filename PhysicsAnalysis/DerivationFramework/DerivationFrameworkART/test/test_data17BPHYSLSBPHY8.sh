#!/bin/sh

# art-description: DAOD building BPHY8 data17BPHYSLS
# art-type: grid
# art-output: *.pool.root
# art-output: checkFile.txt
# art-output: checkxAOD.txt

set -e

Reco_tf.py --inputAODFile /cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/DerivationFrameworkART/data17_13TeV.00337491.physics_BphysLS.merge.AOD.f873_m1885._lb0100._0001.1 --outputDAODFile art.pool.root --reductionConf BPHY8 --maxEvents 1000 --preExec 'rec.doApplyAODFix.set_Value_and_Lock(True);from BTagging.BTaggingFlags import BTaggingFlags;BTaggingFlags.CalibrationTag = "BTagCalibRUN12Onl-08-40" ' 

DAODMerge_tf.py --inputDAOD_BPHY8File DAOD_BPHY8.art.pool.root --outputDAOD_BPHY8_MRGFile art_merged.pool.root
checkFile.py DAOD_BPHY8.art.pool.root > checkFile.txt
checkxAOD.py DAOD_BPHY8.art.pool.root > checkxAOD.txt