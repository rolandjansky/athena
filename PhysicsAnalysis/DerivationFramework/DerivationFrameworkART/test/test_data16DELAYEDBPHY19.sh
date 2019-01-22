#!/bin/sh

# art-include: 21.2/AthDerivation
# art-description: DAOD building BPHY19 data16DELAYED
# art-type: grid
# art-output: *.pool.root
# art-output: checkFile.txt
# art-output: checkxAOD.txt

set -e

Reco_tf.py --inputAODFile /cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/DerivationFrameworkART/AOD.11270451._000007.pool.root.1 --outputDAODFile art.pool.root --reductionConf BPHY19 --maxEvents -1 --preExec 'rec.doApplyAODFix.set_Value_and_Lock(True);from BTagging.BTaggingFlags import BTaggingFlags;BTaggingFlags.CalibrationTag = "BTagCalibRUN12Onl-08-47"; from AthenaCommon.AlgSequence import AlgSequence; topSequence = AlgSequence(); topSequence += CfgMgr.xAODMaker__DynVarFixerAlg( "InDetTrackParticlesFixer", Containers = [ "InDetTrackParticlesAux." ] )' 

echo "art-result: $? reco"

DAODMerge_tf.py --inputDAOD_BPHY19File DAOD_BPHY19.art.pool.root --outputDAOD_BPHY19_MRGFile art_merged.pool.root

echo "art-result: $? merge"

checkFile.py DAOD_BPHY19.art.pool.root > checkFile.txt

echo "art-result: $?  checkfile"

checkxAOD.py DAOD_BPHY19.art.pool.root > checkxAOD.txt

echo "art-result: $?  checkxAOD"
