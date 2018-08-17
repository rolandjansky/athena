#!/bin/sh

# art-include
# art-description: DAOD compare to ref FTAG1 mc16
# art-type: grid

set -e

Reco_tf.py --inputAODFile /cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/DerivationFrameworkART/AOD.12169019._004055.pool.root.1 --outputDAODFile art.pool.root --reductionConf FTAG1 --maxEvents 500 --preExec 'rec.doApplyAODFix.set_Value_and_Lock(True);from BTagging.BTaggingFlags import BTaggingFlags;BTaggingFlags.CalibrationTag = "BTagCalibRUN12-08-40" ' 


art-diff.py ./DAOD_FTAG1.art.pool.root /cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/DerivationFrameworkART/ReferenceFiles/21.2.34.0/mc16_13TeV.361023.e3668_s3126_r10201/DAOD_FTAG1.mc16_dijet.pool.root 


