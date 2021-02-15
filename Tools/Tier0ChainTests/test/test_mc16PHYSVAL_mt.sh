#!/bin/sh

# art-include: master/Athena
# art-description: DAOD building PHYSVAL mc16 MT
# art-type: grid
# art-output: *.pool.root
# art-output: checkFile.txt
# art-output: checkxAOD.txt
# art-athena-mt: 1

set -e

Reco_tf.py --athenaopts='--threads=1' --inputAODFile /cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/Tier0ChainTests/mc16_13TeV.361107.PowhegPythia8EvtGen_AZNLOCTEQ6L1_Zmumu.recon.AOD.e3601_s3126_r12305/AOD.23662571._000001.pool.root.1 --outputDAODFile art.pool.root --reductionConf PHYSVAL --maxEvents -1 --preExec 'from AthenaCommon.DetFlags import DetFlags; DetFlags.detdescr.all_setOff(); DetFlags.BField_setOn(); DetFlags.pileup.all_setOff(); DetFlags.overlay.all_setOff(); from AthenaCommon.AlgSequence import AlgSequence; topSequence = AlgSequence(); topSequence += CfgMgr.xAODMaker__DynVarFixerAlg("BTaggingELFixer", Containers = ["BTagging_AntiKt4EMTopoAux."] ); topSequence += CfgMgr.xAODMaker__DynVarFixerAlg("JetELFixer", Containers = ["AntiKt4EMTopoJetsAux."] );' 

echo "art-result: $? reco"

DAODMerge_tf.py --inputDAOD_PHYSVALFile DAOD_PHYSVAL.art.pool.root --outputDAOD_PHYSVAL_MRGFile art_merged.pool.root

echo "art-result: $? merge"

checkFile.py DAOD_PHYSVAL.art.pool.root > checkFile.txt

echo "art-result: $?  checkfile"

checkxAOD.py DAOD_PHYSVAL.art.pool.root > checkxAOD.txt

echo "art-result: $?  checkxAOD"
