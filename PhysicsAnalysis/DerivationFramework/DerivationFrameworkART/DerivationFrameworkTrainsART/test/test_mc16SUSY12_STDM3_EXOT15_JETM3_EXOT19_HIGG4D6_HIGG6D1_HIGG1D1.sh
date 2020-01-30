#!/bin/sh

# art-include: 21.2/AthDerivation
# art-description: DAOD building SUSY12 STDM3 EXOT15 JETM3 EXOT19 HIGG4D6 HIGG6D1 HIGG1D1 mc16
# art-type: grid
# art-output: *.pool.root

set -e

Reco_tf.py --inputAODFile /cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/DerivationFrameworkART/AOD.14795494._005958.pool.root.1 --outputDAODFile art.pool.root --reductionConf SUSY12 STDM3 EXOT15 JETM3 EXOT19 HIGG4D6 HIGG6D1 HIGG1D1 --maxEvents 500  --preExec 'rec.doApplyAODFix.set_Value_and_Lock(True);from BTagging.BTaggingFlags import BTaggingFlags;BTaggingFlags.CalibrationTag = "BTagCalibRUN12-08-49" '  --passThrough True 
