#!/bin/sh

# art-include: 21.2/AthDerivation
# art-description: DAOD building HION6 mc16
# art-type: grid
# art-output: *.pool.root
# art-output: checkFile.txt
# art-output: checkxAOD.txt

set -e

Reco_tf.py --inputAODFile /cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/DerivationFrameworkART/mc16_pPb8TeV.501710.aMCPy8EG_ttbar_hdamp258p75_nonallhad.merge.AOD.e8411_d1603_r12235_r11932/AOD.27613367._000001.pool.root.1 --outputDAODFile art.pool.root --reductionConf HION6 --maxEvents -1 --preExec 'rec.doApplyAODFix.set_Value_and_Lock(True); from BTagging.BTaggingFlags import BTaggingFlags; BTaggingFlags.CalibrationTag = "BTagCalibRUN12Onl-08-49"; from AthenaCommon.AlgSequence import AlgSequence; topSequence = AlgSequence(); topSequence += CfgMgr.xAODMaker__DynVarFixerAlg( "BTaggingELFixer", Containers=["BTagging_AntiKt4HIAux.", "BTagging_AntiKt4EMTopoAux."] )'

echo "art-result: $? reco"

DAODMerge_tf.py --inputDAOD_HION6File DAOD_HION6.art.pool.root --outputDAOD_HION6_MRGFile art_merged.pool.root --preExec 'rec.doHeavyIon.set_Value_and_Lock(False)'

echo "art-result: $? merge"

checkFile.py DAOD_HION6.art.pool.root > checkFile.txt

echo "art-result: $?  checkfile"

checkxAOD.py DAOD_HION6.art.pool.root > checkxAOD.txt

echo "art-result: $?  checkxAOD"
