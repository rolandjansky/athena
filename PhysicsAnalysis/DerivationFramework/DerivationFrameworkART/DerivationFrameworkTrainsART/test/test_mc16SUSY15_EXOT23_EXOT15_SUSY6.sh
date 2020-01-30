#!/bin/sh

# art-include: 21.2/AthDerivation
# art-description: DAOD building SUSY15 EXOT23 EXOT15 SUSY6 mc16
# art-type: grid
# art-output: *.pool.root

set -e

Reco_tf.py --inputAODFile /cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/DerivationFrameworkART/DAOD_RPVLL.15268048._000134.pool.root.1 --outputDAODFile art.pool.root --reductionConf SUSY15 EXOT23 EXOT15 SUSY6 --maxEvents 500  --preExec 'rec.doApplyAODFix.set_Value_and_Lock(True);from BTagging.BTaggingFlags import BTaggingFlags;BTaggingFlags.CalibrationTag = "BTagCalibRUN12-08-49" '  --passThrough True 
