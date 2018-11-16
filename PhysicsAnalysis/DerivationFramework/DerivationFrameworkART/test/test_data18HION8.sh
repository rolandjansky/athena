#!/bin/sh

# art-include: 21.2/AthDerivation
# art-description: DAOD building HION8 data18
# art-type: grid
# art-output: *.pool.root
# art-output: checkFile.txt
# art-output: checkxAOD.txt

set -e

Reco_tf.py --inputAODFile /cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/DerivationFrameworkART/AOD.15763788._016518.pool.root.1 --outputDAODFile art.pool.root --reductionConf HION8 --maxEvents -1 --preExec 'rec.doApplyAODFix.set_Value_and_Lock(False); from BTagging.BTaggingFlags import BTaggingFlags; BTaggingFlags.CalibrationTag = "BTagCalibRUN12Onl-08-47"; from AthenaCommon.AlgSequence import AlgSequence; topSequence = AlgSequence(); topSequence += CfgMgr.xAODMaker__DynVarFixerAlg( "BTaggingFixer", Containers=["BTagging_AntiKt4HIAux."] )' 

echo "art-result: $? reco"

DAODMerge_tf.py --inputDAOD_HION8File DAOD_HION8.art.pool.root --outputDAOD_HION8_MRGFile art_merged.pool.root --preExec 'rec.doHeavyIon.set_Value_and_Lock(False)' 

echo "art-result: $? merge"

checkFile.py DAOD_HION8.art.pool.root > checkFile.txt

echo "art-result: $?  checkfile"

checkxAOD.py DAOD_HION8.art.pool.root > checkxAOD.txt

echo "art-result: $?  checkxAOD"
