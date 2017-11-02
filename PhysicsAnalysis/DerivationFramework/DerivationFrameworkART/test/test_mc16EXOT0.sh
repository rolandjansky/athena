#!/bin/sh

# art-description: DAOD building EXOT0 mc16
# art-type: grid
# art-output: *.pool.root
# art-output: checkFile.txt

set -e

Reco_tf.py --inputAODFile /cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/DerivationFrameworkART/AOD.11866988._000378.pool.root.1 --outputDAODFile art.pool.root --reductionConf EXOT0 --maxEvents 10000 --preExec 'rec.doApplyAODFix.set_Value_and_Lock(True);from BTagging.BTaggingFlags import BTaggingFlags;BTaggingFlags.CalibrationTag = "BTagCalibRUN12-08-40" ' 

DAODMerge_tf.py --maxEvents 5 --inputDAOD_EXOT0File DAOD_EXOT0.art.pool.root --outputDAOD_EXOT0_MRGFile art_merged.pool.root
checkFile.py DAOD_EXOT0.art.pool.root > checkFile.txt