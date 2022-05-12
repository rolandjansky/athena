# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
'''
ComponentAccumulator configuration for producing transient ByteStream,
which is required when running HLT selection algorithms on MC RDO inputs
'''

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from ByteStreamCnvSvc.ByteStreamConfig import TransientByteStreamCfg

# ----------------------------------------------------------
# Base function
# ----------------------------------------------------------
def triggerTransBSCfg_Base(flags, itemList, typeNames, extraInputs, seqName="AthAlgSeq"):
    '''
    Base function for system-specific transient BS configurations.
    Arguments:
      flags - Configuration AthConfigFlags
      itemList - Objects to be written to transient BS
      typeNames - Objects to be read from transient BS
      extraInputs - Objects to be produced before writing transient BS (scheduler dependency)
      seqName - The sequence where transient BS writing algorithm should be placed
    '''
    acc = ComponentAccumulator(seqName)
    transBSCfg = TransientByteStreamCfg(
        flags,
        item_list=itemList,
        type_names=typeNames,
        extra_inputs=extraInputs)

    acc.merge(transBSCfg, sequenceName=seqName)
    return acc

# ----------------------------------------------------------
# Level-1 Trigger
# ----------------------------------------------------------
def triggerTransBSCfg_LVL1(flags, seqName="AthAlgSeq"):
    itemList = []
    typeNames = []
    extraInputs = []

    if flags.Trigger.enableL1CaloLegacy or not flags.Trigger.enableL1MuonPhase1:
        itemList += ["ROIB::RoIBResult#RoIBResult"]
        extraInputs += [('ROIB::RoIBResult', 'StoreGateSvc+RoIBResult')]

    if flags.Trigger.enableL1MuonPhase1 or flags.Trigger.enableL1CaloPhase1:
        itemList += ["xAOD::TrigCompositeContainer#L1TriggerResult"]
        extraInputs += [('xAOD::TrigCompositeContainer', 'StoreGateSvc+L1TriggerResult')]

    acc = triggerTransBSCfg_Base(flags, itemList, typeNames, extraInputs, seqName)

    from TrigT1ResultByteStream.TrigT1ResultByteStreamConfig import L1TriggerByteStreamEncoderCfg
    acc.merge(L1TriggerByteStreamEncoderCfg(flags))

    return acc

# ----------------------------------------------------------
# ID
# ----------------------------------------------------------
def triggerTransBSCfg_ID(flags, seqName="AthAlgSeq"):
    itemList = []
    typeNames = []
    extraInputs = []

    # Pixel
    itemList += ["PixelRDO_Container#*"]
    typeNames += ["InDet::PixelClusterContainer/PixelOnlineClusters"]
    extraInputs += [('PixelHitDiscCnfgData','ConditionStore+PixelHitDiscCnfgData')]
    extraInputs += [('PixelCablingCondData','ConditionStore+PixelCablingCondData')]
    # SCT
    itemList += ["SCT_RDO_Container#*"]
    typeNames += ["InDet::SCT_ClusterContainer/SCT_OnlineClusters"]
    extraInputs += [('SCT_CablingData','ConditionStore+SCT_CablingData')]
    # TRT
    itemList += ["TRT_RDO_Container#*"]
    typeNames += ["InDet::TRT_DriftCircleContainer/TRT_DriftCircle"]

    return triggerTransBSCfg_Base(flags, itemList, typeNames, extraInputs, seqName)


# ----------------------------------------------------------
# Calo
# ----------------------------------------------------------
def triggerTransBSCfg_Calo(flags, seqName="AthAlgSeq"):
    itemList = []
    typeNames = []
    extraInputs = []

    # LAr
    from LArByteStream.LArByteStreamConfig import LArRawDataContByteStreamToolCfg
    acc, larExtraInputs = LArRawDataContByteStreamToolCfg(flags, InitializeForWriting=True)
    itemList += ["LArRawChannelContainer#*"]
    extraInputs += larExtraInputs
    # Tile
    itemList += ["TileRawChannelContainer#*"]
    typeNames += ["TileCellIDC/TileCellIDC"]
    extraInputs += [('TileBadChannels','ConditionStore+TileBadChannels')]

    acc.merge(triggerTransBSCfg_Base(flags, itemList, typeNames, extraInputs, seqName))
    return acc


# ----------------------------------------------------------
# Muon
# ----------------------------------------------------------
def triggerTransBSCfg_Muon(flags, seqName="AthAlgSeq"):
    itemList = []
    typeNames = []
    extraInputs = []

    # MDT
    itemList += ["MdtCsmContainer#*"]
    typeNames += ["MdtDigitContainer/MDT_DIGITS",
                    "MdtCsmContainer/MDTCSM"]
    extraInputs += [('MuonMDT_CablingMap','ConditionStore+MuonMDT_CablingMap')]
    # RPC
    itemList += ["RpcPadContainer#*"]
    typeNames += ["RpcDigitContainer/RPC_DIGITS",
                    "RpcPadContainer/RPCPAD"]
    # TGC
    itemList += ["TgcRdoContainer#*"]
    typeNames += ["TgcDigitContainer/TGC_DIGITS",
                    "TgcRdoContainer/TGCRDO"]
    # CSC
    itemList += ["CscRawDataContainer#*"]
    typeNames += ["CscDigitContainer/CSC_DIGITS",
                    "CscRawDataContainer/CSCRDO"]

    return triggerTransBSCfg_Base(flags, itemList, typeNames, extraInputs, seqName)

# ----------------------------------------------------------
# Combined configuration with all sub-subsystems
# ----------------------------------------------------------
def triggerTransBSCfg(flags, seqName="AthAlgSeq"):
    acc = triggerTransBSCfg_LVL1(flags, seqName)
    if flags.Trigger.doID:
        acc.merge(triggerTransBSCfg_ID(flags, seqName), sequenceName=seqName)
    if flags.Trigger.doCalo:
        acc.merge(triggerTransBSCfg_Calo(flags, seqName), sequenceName=seqName)
    if flags.Trigger.doMuon:
        acc.merge(triggerTransBSCfg_Muon(flags, seqName), sequenceName=seqName)
    return acc
