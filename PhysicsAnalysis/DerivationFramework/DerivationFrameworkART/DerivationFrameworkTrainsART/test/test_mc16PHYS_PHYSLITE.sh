#!/bin/sh

# art-include: master/Athena
# art-description: DAOD building PHYS and PHYSLITE mc16
# art-type: grid
# art-athena-mt: 8
# art-output: *.pool.root

set -e

export ATHENA_PROC_NUMBER=8
unset ATHENA_CORE_NUMBER # This is not an MT job!

Reco_tf.py --inputAODFile /cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/Tier0ChainTests/mc16_13TeV.361107.PowhegPythia8EvtGen_AZNLOCTEQ6L1_Zmumu.recon.AOD.e3601_s3126_r12305/AOD.23662571._000001.pool.root.1 --outputDAODFile art.pool.root --reductionConf PHYS PHYSLITE --maxEvents -1 --sharedWriter True --preExec 'from AthenaCommon.DetFlags import DetFlags; DetFlags.detdescr.all_setOff(); DetFlags.BField_setOn(); DetFlags.pileup.all_setOff(); DetFlags.overlay.all_setOff(); from AthenaCommon.AlgSequence import AlgSequence; topSequence = AlgSequence(); topSequence += CfgMgr.xAODMaker__DynVarFixerAlg("BTaggingELFixer", Containers = ["BTagging_AntiKt4EMTopoAux."] ); topSequence += CfgMgr.xAODMaker__DynVarFixerAlg("JetELFixer", Containers = ["AntiKt4EMTopoJetsAux."] );'
