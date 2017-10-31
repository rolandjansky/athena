#!/bin/sh

# art-description: DAOD building STDM9 HIGG5D3 BPHY9 HIGG5D1 BPHY6 mc16
# art-type: grid
# art-output: *.pool.root

set -e

Reco_tf.py --inputAODFile /cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/DerivationFrameworkART/AOD.11866988._000378.pool.root.1 --outputDAODFile art.pool.root --reductionConf STDM9 HIGG5D3 BPHY9 HIGG5D1 BPHY6 --maxEvents 500 --preExec 'rec.doApplyAODFix.set_Value_and_Lock(True);from BTagging.BTaggingFlags import BTaggingFlags;BTaggingFlags.CalibrationTag = "BTagCalibRUN12-08-40" '  --passThrough True 
