#!/bin/sh

# art-include: 21.2/AthDerivation
# art-description: DAOD building HION4 data18
# art-type: grid
# art-output: *.pool.root
# art-output: checkFile.txt
# art-output: checkxAOD.txt

set -e

Reco_tf.py --inputAODFile /cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/DerivationFrameworkART/data15_hi.00287866.physics_UPC.merge.AOD.f984_m2025._lb0257._0001.1 --outputDAODFile art.pool.root --reductionConf HION4 --maxEvents -1 --preExec 'rec.doApplyAODFix.set_Value_and_Lock(True);from BTagging.BTaggingFlags import BTaggingFlags;BTaggingFlags.CalibrationTag = "BTagCalibRUN12Onl-08-46"; from AthenaCommon.AlgSequence import AlgSequence; topSequence = AlgSequence(); topSequence += CfgMgr.xAODMaker__DynVarFixerAlg( "BTaggingFixer", Containers=["BTagging_AntiKt4HIAux.", "BTagging_AntiKt4EMTopoAux."] )' 

echo "art-result: $? reco"

DAODMerge_tf.py --inputDAOD_HION4File DAOD_HION4.art.pool.root --outputDAOD_HION4_MRGFile art_merged.pool.root

echo "art-result: $? merge"

checkFile.py DAOD_HION4.art.pool.root > checkFile.txt

echo "art-result: $?  checkfile"

checkxAOD.py DAOD_HION4.art.pool.root > checkxAOD.txt

echo "art-result: $?  checkxAOD"
