#!/bin/sh

# art-description: DAOD building HION10 data18
# art-type: grid
# art-output: *.pool.root
# art-output: checkFile.txt
# art-output: checkxAOD.txt

set -e

Reco_tf.py --inputAODFile /cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/DerivationFrameworkART/data17_hi.00338037.physics_MinBias.merge.AOD.f900_m1912._lb0292._0009.1 --outputDAODFile art.pool.root --reductionConf HION10 --maxEvents -1 --preExec 'rec.doApplyAODFix.set_Value_and_Lock(True);from BTagging.BTaggingFlags import BTaggingFlags;BTaggingFlags.CalibrationTag = "BTagCalibRUN12Onl-08-40"; from AthenaCommon.AlgSequence import AlgSequence; topSequence = AlgSequence(); topSequence += CfgMgr.xAODMaker__DynVarFixerAlg( "InDetTrackParticlesFixer", Containers = [ "InDetTrackParticlesAux." ] )' 

echo "art-result: $? reco"

DAODMerge_tf.py --inputDAOD_HION10File DAOD_HION10.art.pool.root --outputDAOD_HION10_MRGFile art_merged.pool.root

echo "art-result: $? merge"

checkFile.py DAOD_HION10.art.pool.root > checkFile.txt

echo "art-result: $?  checkfile"

checkxAOD.py DAOD_HION10.art.pool.root > checkxAOD.txt

echo "art-result: $?  checkxAOD"
