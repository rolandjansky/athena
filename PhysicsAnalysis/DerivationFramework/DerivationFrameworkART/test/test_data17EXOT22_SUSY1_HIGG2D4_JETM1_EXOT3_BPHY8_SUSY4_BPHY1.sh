#!/bin/sh

# art-description: DAOD building EXOT22 SUSY1 HIGG2D4 JETM1 EXOT3 BPHY8 SUSY4 BPHY1 data17
# art-type: grid
# art-output: *.pool.root

set -e

Reco_tf.py --inputAODFile /cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/DerivationFrameworkART/AOD.12818484._004694.pool.root.1 --outputDAODFile art.pool.root --reductionConf EXOT22 SUSY1 HIGG2D4 JETM1 EXOT3 BPHY8 SUSY4 BPHY1 --maxEvents 500  --preExec 'rec.doApplyAODFix.set_Value_and_Lock(True);from BTagging.BTaggingFlags import BTaggingFlags;BTaggingFlags.CalibrationTag = "BTagCalibRUN12Onl-08-40" '  --passThrough True 
