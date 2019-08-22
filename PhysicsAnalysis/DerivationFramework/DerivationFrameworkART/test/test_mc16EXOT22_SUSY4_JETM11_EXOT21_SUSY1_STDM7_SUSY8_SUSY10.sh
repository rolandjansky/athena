#!/bin/sh

# art-include: 21.2/AthDerivation
# art-description: DAOD building EXOT22 SUSY4 JETM11 EXOT21 SUSY1 STDM7 SUSY8 SUSY10 mc16
# art-type: grid
# art-output: *.pool.root

set -e

Reco_tf.py --inputAODFile /cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/DerivationFrameworkART/AOD.14795494._005958.pool.root.1 --outputDAODFile art.pool.root --reductionConf EXOT22 SUSY4 JETM11 EXOT21 SUSY1 STDM7 SUSY8 SUSY10 --maxEvents 500  --preExec 'rec.doApplyAODFix.set_Value_and_Lock(True);from BTagging.BTaggingFlags import BTaggingFlags;BTaggingFlags.CalibrationTag = "BTagCalibRUN12-08-49" '  --passThrough True 
