#!/bin/sh

# art-description: DAOD building JETM7 BPHY5 MUON3 EXOT6 EGAM2 HIGG4D6 BPHY8 data17
# art-type: grid
# art-output: *.pool.root

set -e

Reco_tf.py --inputAODFile /cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/DerivationFrameworkART/AOD.12818484._004694.pool.root.1 --outputDAODFile art.pool.root --reductionConf JETM7 BPHY5 MUON3 EXOT6 EGAM2 HIGG4D6 BPHY8 --maxEvents 500  --preExec 'rec.doApplyAODFix.set_Value_and_Lock(True);from BTagging.BTaggingFlags import BTaggingFlags;BTaggingFlags.CalibrationTag = "BTagCalibRUN12Onl-08-40" '  --passThrough True 
