#!/bin/sh

# art-include: 21.2/AthDerivation
# art-description: DAOD building EXOT22 SUSY4 JETM11 EXOT21 SUSY1 STDM7 SUSY8 SUSY10 data18
# art-type: grid
# art-output: *.pool.root

set -e

Reco_tf.py --inputAODFile /cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/DerivationFrameworkART/data18_13TeV.00348403.physics_Main.merge.AOD.f920_m1947._lb0829._0001.1 --outputDAODFile art.pool.root --reductionConf EXOT22 SUSY4 JETM11 EXOT21 SUSY1 STDM7 SUSY8 SUSY10 --maxEvents 500  --preExec 'rec.doApplyAODFix.set_Value_and_Lock(True); from BTagging.BTaggingFlags import BTaggingFlags;BTaggingFlags.CalibrationTag = "BTagCalibRUN12Onl-08-49"; from AthenaCommon.AlgSequence import AlgSequence; topSequence = AlgSequence(); topSequence += CfgMgr.xAODMaker__DynVarFixerAlg( "InDetTrackParticlesFixer", Containers = [ "InDetTrackParticlesAux." ] )'  --passThrough True 
