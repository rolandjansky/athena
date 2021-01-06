#!/bin/sh

# art-include: master/Athena
# art-description: DAOD building PHYS PHYSLITE data18
# art-type: grid
# art-athena-mt: 8
# art-output: *.pool.root

set -e

export ATHENA_PROC_NUMBER=8
unset ATHENA_CORE_NUMBER # This is not an MT job!

Reco_tf.py --inputAODFile /cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/DerivationFrameworkART/data18_13TeV.00364292.physics_Main.merge.AOD.f1002_m2037._lb0163._0006.1 --outputDAODFile art.pool.root --reductionConf PHYS PHYSLITE --maxEvents 100 --sharedWriter True --preExec 'rec.doApplyAODFix.set_Value_and_Lock(True); from BTagging.BTaggingFlags import BTaggingFlags;BTaggingFlags.CalibrationTag = "BTagCalibRUN12Onl-08-49"; from AthenaCommon.AlgSequence import AlgSequence; topSequence = AlgSequence(); topSequence += CfgMgr.xAODMaker__DynVarFixerAlg( "InDetTrackParticlesFixer", Containers = [ "InDetTrackParticlesAux." ] );from AthenaMP.AthenaMPFlags import jobproperties as jps;jps.AthenaMPFlags.UseSharedWriter=True;import AthenaPoolCnvSvc.AthenaPool;ServiceMgr.AthenaPoolCnvSvc.OutputMetadataContainer="MetaData"'
