#!/bin/sh

# art-include: master/Athena
# art-description: DAOD building PHYS and PHYSLITE data18
# art-type: grid
# art-athena-mt: 8
# art-output: *.pool.root

set -e

export ATHENA_PROC_NUMBER=8
unset ATHENA_CORE_NUMBER # This is not an MT job!

Reco_tf.py --inputAODFile /cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/Tier0ChainTests/data18_13TeV.00357750.physics_Main.merge.AOD.f1058_m2064/data18_13TeV.00357750.physics_Main.merge.AOD.f1058_m2064._lb0100._0010.1 --outputDAODFile art.pool.root --reductionConf PHYS PHYSLITE --maxEvents -1 --sharedWriter True --preExec 'from AthenaCommon.DetFlags import DetFlags; DetFlags.detdescr.all_setOff(); DetFlags.detdescr.Calo_setOn(); DetFlags.BField_setOn(); DetFlags.pileup.all_setOff(); DetFlags.overlay.all_setOff(); from AthenaCommon.AlgSequence import AlgSequence; topSequence = AlgSequence(); topSequence += CfgMgr.xAODMaker__DynVarFixerAlg("BTaggingELFixer", Containers = ["BTagging_AntiKt4EMTopoAux."] ); topSequence += CfgMgr.xAODMaker__DynVarFixerAlg("JetELFixer", Containers = ["AntiKt4EMTopoJetsAux."] );'
