#!/bin/sh

# art-description: DAOD building HIGG4D2 mc16
# art-type: grid
# art-output: *.pool.root
# art-output: checkFile.txt

set -e

Reco_tf.py --inputAODFile /cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/DerivationFrameworkART/AOD.12169019._004055.pool.root.1 --outputDAODFile art.pool.root --reductionConf HIGG4D2 --maxEvents -1 --preExec 'rec.doApplyAODFix.set_Value_and_Lock(True);from BTagging.BTaggingFlags import BTaggingFlags;BTaggingFlags.CalibrationTag = "BTagCalibRUN12-08-40" ' 

DAODMerge_tf.py --inputDAOD_HIGG4D2File DAOD_HIGG4D2.art.pool.root --outputDAOD_HIGG4D2_MRGFile art_merged.pool.root
checkFile.py DAOD_HIGG4D2.art.pool.root > checkFile.txt