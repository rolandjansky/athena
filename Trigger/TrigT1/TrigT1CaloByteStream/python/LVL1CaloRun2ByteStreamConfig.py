# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory


def typeNamesToDecodeAll():
    type_names = [
    # ===== PPM ================================================================
        "xAOD::TriggerTowerContainer/xAODTriggerTowers",
        "xAOD::TriggerTowerAuxContainer/xAODTriggerTowersAux.",
        "xAOD::TriggerTowerContainer/xAODTriggerTowersMuon",
        "xAOD::TriggerTowerAuxContainer/xAODTriggerTowersMuonAux.",
        "xAOD::TriggerTowerContainer/xAODTriggerTowersSpare",
        "xAOD::TriggerTowerAuxContainer/xAODTriggerTowersSpareAux.",
        # without prefix
        "xAOD::TriggerTowerContainer/TriggerTowers",
        "xAOD::TriggerTowerAuxContainer/TriggerTowersAux.",
        "xAOD::TriggerTowerContainer/TriggerTowersMuon",
        "xAOD::TriggerTowerAuxContainer/TriggerTowersMuonAux.",
        "xAOD::TriggerTowerContainer/TriggerTowersSpare",
        "xAOD::TriggerTowerAuxContainer/TriggerTowersSpareAux.",
        # ZDC
        "xAOD::TriggerTowerContainer/ZdcTriggerTowers",
        "xAOD::TriggerTowerAuxContainer/ZdcTriggerTowersAux.",
    # ===== CPM ================================================================
        "xAOD::CPMTowerContainer/CPMTowers",
        "xAOD::CPMTowerAuxContainer/CPMTowersAux.",
        "xAOD::CPMTowerContainer/CPMTowersOverlap",
        "xAOD::CPMTowerAuxContainer/CPMTowersOverlapAux.",
    # ===== CMXCPTOB ===========================================================
        "xAOD::CMXCPTobContainer/CMXCPTobs",
        "xAOD::CMXCPTobAuxContainer/CMXCPTobsAux.",
    # ===== CMXCPHITS ==========================================================
        "xAOD::CMXCPHitsContainer/CMXCPHits",
        "xAOD::CMXCPHitsAuxContainer/CMXCPHitsAux.",
    # ===== CMXJETTOB ==========================================================
        "xAOD::CMXJetTobContainer/CMXJetTobs",
        "xAOD::CMXJetTobAuxContainer/CMXJetTobsAux.",
    # ===== CMXJETHITS =========================================================
        "xAOD::CMXJetHitsContainer/CMXJetHits",
        "xAOD::CMXJetHitsAuxContainer/CMXJetHitsAux.",
    # ===== CMXETSUMS ==========================================================
        "xAOD::CMXEtSumsContainer/CMXEtSums",
        "xAOD::CMXEtSumsAuxContainer/CMXEtSumsAux.",
    # ===== JEMETSUMS ==========================================================
        "xAOD::JEMEtSumsContainer/JEMEtSums",
        "xAOD::JEMEtSumsAuxContainer/JEMEtSumsAux.",
    # ===== CPMTOBROIS =========================================================
        "xAOD::CPMTobRoIContainer/CPMTobRoIs",
        "xAOD::CPMTobRoIAuxContainer/CPMTobRoIsAux.",
        "xAOD::CPMTobRoIContainer/CPMTobRoIsRoIB",
        "xAOD::CPMTobRoIAuxContainer/CPMTobRoIsRoIBAux.",
    # ===== JEMTOBROIS =========================================================
        "xAOD::JEMTobRoIContainer/JEMTobRoIs",
        "xAOD::JEMTobRoIAuxContainer/JEMTobRoIsAux.",
        "xAOD::JEMTobRoIContainer/JEMTobRoIsRoIB",
        "xAOD::JEMTobRoIAuxContainer/JEMTobRoIsRoIBAux.",
    # ===== JETELEMENT =========================================================
        "xAOD::JetElementContainer/JetElements",
        "xAOD::JetElementAuxContainer/JetElementsAux.",
        "xAOD::JetElementContainer/JetElementsOverlap",
        "xAOD::JetElementAuxContainer/JetElementsOverlapAux.",
    # ===== CMXROI =============================================================
        "xAOD::CMXRoIContainer/CMXRoIs",
        "xAOD::CMXRoIAuxContainer/CMXRoIsAux.",
        "xAOD::CMXRoIContainer/CMXRoIsRoIB",
        "xAOD::CMXRoIAuxContainer/CMXRoIsRoIBAux.",
    # ===== RODHEADER ==========================================================
        "xAOD::RODHeaderContainer/RODHeaders",
        "xAOD::RODHeaderAuxContainer/RODHeadersAux.",
        "xAOD::RODHeaderContainer/RODHeadersPP",
        "xAOD::RODHeaderAuxContainer/RODHeadersPPAux.",
        "xAOD::RODHeaderContainer/RODHeadersCP",
        "xAOD::RODHeaderAuxContainer/RODHeadersCPAux.",
        "xAOD::RODHeaderContainer/RODHeadersCPRoI",
        "xAOD::RODHeaderAuxContainer/RODHeadersCPRoIAux.",
        "xAOD::RODHeaderContainer/RODHeadersJEP",
        "xAOD::RODHeaderAuxContainer/RODHeadersJEPAux.",
        "xAOD::RODHeaderContainer/RODHeadersJEPRoI",
        "xAOD::RODHeaderAuxContainer/RODHeadersJEPRoIAux.",
        "xAOD::RODHeaderContainer/RODHeadersCPRoIB",
        "xAOD::RODHeaderAuxContainer/RODHeadersCPRoIBAux.",
        "xAOD::RODHeaderContainer/RODHeadersJEPRoIB",
        "xAOD::RODHeaderAuxContainer/RODHeadersJEPRoIBAux.",
        "std::vector<unsigned int>/L1CaloUnpackingErrors",
    ]
    return type_names


def typeNamesToDecodeForRerunLVL1():
    type_names = [
    # ===== CPM ================================================================
        "xAOD::CPMTowerContainer/CPMTowers",
        "xAOD::CPMTowerAuxContainer/CPMTowersAux.",
    # ===== JETELEMENT =========================================================
        "xAOD::JetElementContainer/JetElements",
        "xAOD::JetElementAuxContainer/JetElementsAux.",
    ]
    return type_names


def readOnlyToolsForRerunLVL1(flags):
    acc = ComponentAccumulator()
    cpmReadTool = CompFactory.getComp('LVL1BS::CpByteStreamV2Tool')(
        name='CpByteStreamV2Tool',
        enableEncoding = False)
    acc.addPublicTool(cpmReadTool)
    jepReadTool = CompFactory.getComp('LVL1BS::JepByteStreamV2Tool')(
        name='JepByteStreamV2Tool',
        enableEncoding = False)
    acc.addPublicTool(jepReadTool)
    return acc


def LVL1CaloRun2ReadBSCfg(flags):
    acc = ComponentAccumulator()
    typeNamesToDecode = []

    if flags.Trigger.doLVL1:
        # Rerun L1Calo simulation on data - only need a few inputs from data
        typeNamesToDecode = typeNamesToDecodeForRerunLVL1()
    else:
        # Configure the full list of objects to decode
        typeNamesToDecode = typeNamesToDecodeAll()

    if flags.Trigger.Online.isPartition:
        from TrigByteStreamCnvSvc.TrigByteStreamConfig import TrigByteStreamCfg
        servicesCfgFunction = TrigByteStreamCfg
        toolsCfg = readOnlyToolsForRerunLVL1(flags)
        acc.merge(toolsCfg)
    else:
        from ByteStreamCnvSvc.ByteStreamConfig import ByteStreamReadCfg
        servicesCfgFunction = ByteStreamReadCfg
        # In principle could configure all public tools for conversion
        # explicitly here, but they all use default properties, so no need to

    servicesCfg = servicesCfgFunction(flags, type_names=typeNamesToDecode)
    acc.merge(servicesCfg)
    return acc

