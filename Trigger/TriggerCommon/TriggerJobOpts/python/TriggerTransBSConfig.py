# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
'''
ComponentAccumulator configuration for producing transient ByteStream,
which is required when running HLT selection algorithms on MC RDO inputs
'''


from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from ByteStreamCnvSvc.ByteStreamConfig import TransientByteStreamCfg

def triggerTransBSCfg(flags, seqName="AthAlgSeq"):
    acc = ComponentAccumulator(seqName)

    itemList = []  # Objects to be written to transient BS
    typeNames = []  # Objects to be read from transient BS
    extraInputs = []  # Objects to be produced before writing transient BS (scheduler dependency)

    # --------------------------------------------------
    # Level-1 Trigger
    # --------------------------------------------------
    from TrigT1ResultByteStream.TrigT1ResultByteStreamConfig import L1TriggerByteStreamEncoderCfg
    acc.merge(L1TriggerByteStreamEncoderCfg(flags))

    if flags.Trigger.enableL1CaloLegacy or not flags.Trigger.enableL1Phase1:
        itemList += ["ROIB::RoIBResult#RoIBResult"]
        typeNames += ["MuCTPI_RDO/MUCTPI_RDO"]
        extraInputs += [('ROIB::RoIBResult', 'StoreGateSvc+RoIBResult')]

    if flags.Trigger.enableL1Phase1:
        itemList += ["xAOD::TrigCompositeContainer#L1TriggerResult",
                     "xAOD::TrigCompositeAuxContainer#L1TriggerResultAux."]
        extraInputs += [('xAOD::TrigCompositeContainer', 'StoreGateSvc+L1TriggerResult')]

    # --------------------------------------------------
    # ID
    # --------------------------------------------------
    if flags.Trigger.doID:
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

    # --------------------------------------------------
    # Calo
    # --------------------------------------------------
    if flags.Trigger.doCalo:
        # LAr
        from LArByteStream.LArByteStreamConfig import LArRawDataContByteStreamToolConfig
        lar_tool = LArRawDataContByteStreamToolConfig(InitializeForWriting=True)
        acc.addPublicTool(lar_tool)
        itemList += ["LArRawChannelContainer#*"]
        extraInputs += [('CaloNoise', 'ConditionStore+totalNoise'),
                        ('LArOnOffIdMapping', 'ConditionStore+LArOnOffIdMap'),
                        ('LArFebRodMapping', 'ConditionStore+LArFebRodMap')]
        # Tile
        itemList += ["TileRawChannelContainer#*"]
        typeNames += ["TileCellIDC/TileCellIDC"]
        extraInputs += [('TileBadChannels','ConditionStore+TileBadChannels')]

    # --------------------------------------------------
    # Muon
    # --------------------------------------------------
    if flags.Trigger.doMuon:
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

    # --------------------------------------------------
    # Final configuration
    # --------------------------------------------------
    transBSCfg = TransientByteStreamCfg(
        flags,
        item_list=itemList,
        type_names=typeNames,
        extra_inputs=extraInputs)
    acc.merge(transBSCfg, sequenceName=seqName)

    return acc
