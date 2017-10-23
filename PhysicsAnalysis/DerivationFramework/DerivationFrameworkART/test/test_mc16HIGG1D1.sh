#!/bin/sh

# art-description: DAOD building HIGG1D1 mc16
# art-type: grid
# art-output: *.pool.root
# art-output: checkFile.txt

set -e

Reco_tf.py --inputAODFile /cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/DerivationFrameworkART/AOD.11866988._000378.pool.root.1 --outputDAODFile art.pool.root --reductionConf HIGG1D1 --maxEvents 10000 --preExec 'rec.doApplyAODFix.set_Value_and_Lock(True);from BTagging.BTaggingFlags import BTaggingFlags;BTaggingFlags.CalibrationTag = "BTagCalibRUN12-08-40" ' 

DAODMerge_tf.py --maxEvents 5 --inputDAOD_HIGG1D1File DAOD_HIGG1D1.art.pool.root --outputDAOD_HIGG1D1_MRGFile art_merged.pool.root
checkFile.py DAOD_HIGG1D1.art.pool.root > checkFile.txt