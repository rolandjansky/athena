#!/bin/sh

# art-description: DAOD building EXOT22 SUSY1 HIGG2D4 JETM1 EXOT3 BPHY8 SUSY4 BPHY1 data18
# art-type: grid
# art-output: *.pool.root

set -e

Reco_tf.py --inputAODFile /cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/DerivationFrameworkART/data18_13TeV.00348403.physics_Main.merge.AOD.f920_m1947._lb0829._0001.1 --outputDAODFile art.pool.root --reductionConf EXOT22 SUSY1 HIGG2D4 JETM1 EXOT3 BPHY8 SUSY4 BPHY1 --maxEvents 500  --preExec 'rec.doApplyAODFix.set_Value_and_Lock(True);from BTagging.BTaggingFlags import BTaggingFlags;BTaggingFlags.CalibrationTag = "BTagCalibRUN12Onl-08-40" '  --passThrough True 
