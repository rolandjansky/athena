# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory

def RoIBResultToxAODCfg(flags, seqName=''):
    if flags.Trigger.enableL1Phase1 and not flags.Trigger.enableL1CaloLegacy:
        # No Run-2 L1 RoIs -> nothing to covert to xAOD -> don't add RoIBResultToxAOD
        return ComponentAccumulator()

    acc = ComponentAccumulator(sequenceName=seqName)
    alg = CompFactory.RoIBResultToxAOD('RoIBResultToxAOD')
    alg.DoMuon = not flags.Trigger.enableL1Phase1
    alg.DoCalo = flags.Trigger.enableL1CaloLegacy
    acc.addEventAlgo(alg, sequenceName=seqName)

    if flags.Input.Format == 'BS':
        from ByteStreamCnvSvc.ByteStreamConfig import ByteStreamReadCfg
        typeNames = [
            'xAOD::JetElementContainer/JetElements', 'xAOD::JetElementAuxContainer/JetElementsAux.',
            'xAOD::CPMTowerContainer/CPMTowers', 'xAOD::CPMTowerAuxContainer/CPMTowersAux.']
        acc.merge(ByteStreamReadCfg(flags, typeNames))

    # Create output list to return for use by the caller
    outputList = []
    if alg.DoMuon:
        outputList += [
            ("xAOD::MuonRoIContainer",  alg.xAODKeyMuon)
        ]
    if alg.DoCalo:
        outputList += [
            ("xAOD::EmTauRoIContainer", alg.xAODKeyEmTau),
            ("xAOD::EnergySumRoI",      alg.xAODKeyEsum),
            ("xAOD::JetEtRoI",          alg.xAODKeyJetEt),
            ("xAOD::JetRoIContainer",   alg.xAODKeyJet)
        ]

    return acc, outputList
