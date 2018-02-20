#!/bin/sh

# art-description: DAOD building BPHY4 BPHY7 HIGG4D5 EXOT15 EXOT9 BPHY1 EGAM7 mc16
# art-type: grid
# art-output: *.pool.root

set -e

Reco_tf.py --inputAODFile /cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/DerivationFrameworkART/AOD.12169019._004055.pool.root.1 --outputDAODFile art.pool.root --reductionConf BPHY4 BPHY7 HIGG4D5 EXOT15 EXOT9 BPHY1 EGAM7 --maxEvents 500  --preExec 'rec.doApplyAODFix.set_Value_and_Lock(True);from BTagging.BTaggingFlags import BTaggingFlags;BTaggingFlags.CalibrationTag = "BTagCalibRUN12-08-40" '  --passThrough True 
