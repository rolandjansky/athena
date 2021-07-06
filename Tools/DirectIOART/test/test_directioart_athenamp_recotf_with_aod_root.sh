#!/bin/bash

# art-description: DirectIOART AthenaMP Reco_tf.py inputFile:AOD protocol=ROOT
# art-type: grid
# art-output: *.pool.root
# art-include: master/Athena
# art-athena-mt: 2

set -e

unset ATHENA_PROC_NUMBER; Reco_tf.py --athenaopts='--nprocs=2' --athenaMPMergeTargetSize 'DAOD_*:0' --inputAODFile root://lcg-lrz-rootd.grid.lrz.de:1094/pnfs/lrz-muenchen.de/data/atlas/dq2/atlasdatadisk/rucio/data18_13TeV/4e/57/data18_13TeV.00349263.physics_Main.merge.AOD.f937_m1972._lb0149._0001.1 --outputDAODFile art.pool.root --reductionConf PHYS --maxEvents 10 --preExec 'from AthenaCommon.DetFlags import DetFlags; DetFlags.detdescr.all_setOff(); DetFlags.BField_setOn(); DetFlags.pileup.all_setOff(); DetFlags.overlay.all_setOff(); from AthenaCommon.AlgSequence import AlgSequence; topSequence = AlgSequence(); topSequence += CfgMgr.xAODMaker__DynVarFixerAlg("BTaggingELFixer", Containers = ["BTagging_AntiKt4EMTopoAux."] ); topSequence += CfgMgr.xAODMaker__DynVarFixerAlg("JetELFixer", Containers = ["AntiKt4EMTopoJetsAux."] );'

echo "art-result: $? DirectIOART_AthenaMP_RecoTF_inputAOD_protocol_ROOT"
