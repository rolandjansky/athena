# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory

def RoIBResultToxAODCfg(flags):
    if (flags.Trigger.enableL1MuonPhase1 and flags.Trigger.enableL1CaloPhase1) and not flags.Trigger.enableL1CaloLegacy:
        # No Run-2 L1 RoIs -> nothing to covert to xAOD -> don't add RoIBResultToxAOD
        return ComponentAccumulator()

    acc = ComponentAccumulator()
    alg = CompFactory.RoIBResultToxAOD('RoIBResultToxAOD')
    alg.DoMuon = not flags.Trigger.enableL1MuonPhase1
    alg.DoCalo = flags.Trigger.enableL1CaloLegacy
    acc.addEventAlgo(alg)

    if flags.Input.Format == 'BS':
        from TrigT1CaloByteStream.LVL1CaloRun2ByteStreamConfig import LVL1CaloRun2ReadBSCfg
        acc.merge(LVL1CaloRun2ReadBSCfg(flags, forRoIBResultToxAOD=True))

    # Create output list to return for use by the caller
    outputList = []
    if alg.DoMuon:
        outputList += [
            ("xAOD::MuonRoIContainer",  str(alg.xAODKeyMuon))
        ]
    if alg.DoCalo:
        outputList += [
            ("xAOD::EmTauRoIContainer", str(alg.xAODKeyEmTau)),
            ("xAOD::EnergySumRoI",      str(alg.xAODKeyEsum)),
            ("xAOD::JetEtRoI",          str(alg.xAODKeyJetEt)),
            ("xAOD::JetRoIContainer",   str(alg.xAODKeyJet))
        ]

    return acc, outputList
