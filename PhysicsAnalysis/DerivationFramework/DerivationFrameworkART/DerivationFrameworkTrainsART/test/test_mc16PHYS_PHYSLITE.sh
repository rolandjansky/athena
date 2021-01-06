#!/bin/sh

# art-include: master/Athena
# art-description: DAOD building PHYS PHYSLITE mc16
# art-type: grid
# art-athena-mt: 8
# art-output: *.pool.root

set -e

export ATHENA_PROC_NUMBER=8
unset ATHENA_CORE_NUMBER # This is not an MT job!

Reco_tf.py --inputAODFile /cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/DerivationFrameworkART/AOD.14795494._005958.pool.root.1 --outputDAODFile art.pool.root --reductionConf PHYS PHYSLITE --maxEvents 100 --sharedWriter True --preExec 'rec.doApplyAODFix.set_Value_and_Lock(True);from BTagging.BTaggingFlags import BTaggingFlags;BTaggingFlags.CalibrationTag = "BTagCalibRUN12-08-49";from AthenaMP.AthenaMPFlags import jobproperties as jps;jps.AthenaMPFlags.UseSharedWriter=True;import AthenaPoolCnvSvc.AthenaPool;ServiceMgr.AthenaPoolCnvSvc.OutputMetadataContainer="MetaData"'
