#!/bin/sh

# art-include: 21.2/AthDerivation
# art-description: DAOD building HION12 data18
# art-type: grid
# art-output: *.pool.root
# art-output: checkFile.txt
# art-output: checkxAOD.txt

set -e

Reco_tf.py --inputAODFile /cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/DerivationFrameworkART/AOD.25764336._000976.pool.root.1 --outputDAODFile art.pool.root --reductionConf HION12 --maxEvents -1 --preExec 'rec.doApplyAODFix.set_Value_and_Lock(True); from BTagging.BTaggingFlags import BTaggingFlags; BTaggingFlags.CalibrationTag = "BTagCalibRUN12Onl-08-49"; from AthenaCommon.AlgSequence import AlgSequence; topSequence = AlgSequence(); topSequence += CfgMgr.xAODMaker__DynVarFixerAlg( "BTaggingFixer", Containers=["BTagging_AntiKt4HIAux.", "BTagging_AntiKt4EMTopoAux."] ); from JetRec.JetRecFlags import jetFlags; jetFlags.useTracks.set_Value(True)' 

echo "art-result: $? reco"

DAODMerge_tf.py --inputDAOD_HION12File DAOD_HION12.art.pool.root --outputDAOD_HION12_MRGFile art_merged.pool.root --preExec 'rec.doHeavyIon.set_Value_and_Lock(False)' 

echo "art-result: $? merge"

checkFile.py DAOD_HION12.art.pool.root > checkFile.txt

echo "art-result: $?  checkfile"

checkxAOD.py DAOD_HION12.art.pool.root > checkxAOD.txt

echo "art-result: $?  checkxAOD"
