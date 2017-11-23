#!/bin/sh

# art-description: DAOD building BPHY10 data16DELAYED
# art-type: grid
# art-output: *.pool.root
# art-output: checkFile.txt

set -e

Reco_tf.py --inputAODFile /cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/DerivationFrameworkART/AOD.11270451._000007.pool.root.1 --outputDAODFile art.pool.root --reductionConf BPHY10 --maxEvents -1 --preExec 'rec.doApplyAODFix.set_Value_and_Lock(True);from BTagging.BTaggingFlags import BTaggingFlags;BTaggingFlags.CalibrationTag = "BTagCalibRUN12Onl-08-40" ' 

DAODMerge_tf.py --inputDAOD_BPHY10File DAOD_BPHY10.art.pool.root --outputDAOD_BPHY10_MRGFile art_merged.pool.root
checkFile.py DAOD_BPHY10.art.pool.root > checkFile.txt