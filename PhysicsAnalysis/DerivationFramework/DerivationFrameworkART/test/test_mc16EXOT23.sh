#!/bin/sh

# art-description: DAOD building EXOT23 mc16
# art-type: grid
# art-output: *.pool.root
# art-output: checkFile.txt

set -e

Reco_tf.py --inputAODFile /cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/DerivationFrameworkART/user.ctreado.11717804.EXT0._000056.DAOD_RPVLL.pool.root --outputDAODFile art.pool.root --reductionConf EXOT23 --maxEvents -1 --preExec 'rec.doApplyAODFix.set_Value_and_Lock(True);from BTagging.BTaggingFlags import BTaggingFlags;BTaggingFlags.CalibrationTag = "BTagCalibRUN12-08-40" ' 

DAODMerge_tf.py --inputDAOD_EXOT23File DAOD_EXOT23.art.pool.root --outputDAOD_EXOT23_MRGFile art_merged.pool.root
checkFile.py DAOD_EXOT23.art.pool.root > checkFile.txt
