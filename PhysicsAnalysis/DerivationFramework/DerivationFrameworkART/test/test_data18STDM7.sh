#!/bin/sh

# art-description: DAOD building STDM7 data18
# art-type: grid
# art-output: *.pool.root
# art-output: checkFile.txt
# art-output: checkxAOD.txt

set -e

Reco_tf.py --inputAODFile /cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/DerivationFrameworkART/data18_13TeV.00348403.physics_Main.merge.AOD.f920_m1947._lb0829._0001.1 --outputDAODFile art.pool.root --reductionConf STDM7 --maxEvents -1 --preExec 'rec.doApplyAODFix.set_Value_and_Lock(True);from BTagging.BTaggingFlags import BTaggingFlags;BTaggingFlags.CalibrationTag = "BTagCalibRUN12Onl-08-40"; from AthenaCommon.AlgSequence import AlgSequence; topSequence = AlgSequence(); topSequence += CfgMgr.xAODMaker__DynVarFixerAlg( "InDetTrackParticlesFixer", Containers = [ "InDetTrackParticlesAux." ] )' 

echo "art-result: $? reco"

DAODMerge_tf.py --inputDAOD_STDM7File DAOD_STDM7.art.pool.root --outputDAOD_STDM7_MRGFile art_merged.pool.root

echo "art-result: $? merge"

checkFile.py DAOD_STDM7.art.pool.root > checkFile.txt

echo "art-result: $?  checkfile"

checkxAOD.py DAOD_STDM7.art.pool.root > checkxAOD.txt

echo "art-result: $?  checkxAOD"
