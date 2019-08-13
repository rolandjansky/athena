#!/bin/sh

# art-include: 21.2/AthDerivation
# art-description: DAOD building MUON2 HIGG4D4 JETM7 BPHY7 EXOT17 BPHY5 EGAM7 HIGG1D2 mc16
# art-type: grid
# art-output: *.pool.root

set -e

Reco_tf.py --inputAODFile /cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/DerivationFrameworkART/AOD.14795494._005958.pool.root.1 --outputDAODFile art.pool.root --reductionConf MUON2 HIGG4D4 JETM7 BPHY7 EXOT17 BPHY5 EGAM7 HIGG1D2 --maxEvents 500  --preExec 'rec.doApplyAODFix.set_Value_and_Lock(True);from BTagging.BTaggingFlags import BTaggingFlags;BTaggingFlags.CalibrationTag = "BTagCalibRUN12-08-49" '  --passThrough True 
