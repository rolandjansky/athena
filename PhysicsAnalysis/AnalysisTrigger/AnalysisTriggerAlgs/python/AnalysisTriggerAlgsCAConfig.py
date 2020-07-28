# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory

def RoIBResultToxAODCfg(flags, seqName=''):
    acc = ComponentAccumulator(sequenceName=seqName)
    alg = CompFactory.RoIBResultToxAOD('RoIBResultToxAOD')
    acc.addEventAlgo(alg, sequenceName=seqName)

    if flags.Input.Format == 'BS':
        from ByteStreamCnvSvc.ByteStreamConfig import ByteStreamReadCfg
        typeNames = [
            'xAOD::JetElementContainer/JetElements', 'xAOD::JetElementAuxContainer/JetElementsAux.',
            'xAOD::CPMTowerContainer/CPMTowers', 'xAOD::CPMTowerAuxContainer/CPMTowersAux.']
        acc.merge(ByteStreamReadCfg(flags, typeNames))

    # Create output list to return for use by the caller
    outputList = [
        ("xAOD::MuonRoIContainer",  alg.xAODKeyMuon),
        ("xAOD::EmTauRoIContainer", alg.xAODKeyEmTau),
        ("xAOD::EnergySumRoI",      alg.xAODKeyEsum),
        ("xAOD::JetEtRoI",          alg.xAODKeyJetEt),
        ("xAOD::JetRoIContainer",   alg.xAODKeyJet),
    ]

    return acc, outputList
