# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from AthenaCommon.Logging import logging
_log = logging.getLogger('AnalysisTriggerAlgsCAConfig.py')

def RoIBResultToxAODCfg(flags):
    if flags.Trigger.enableL1MuonPhase1 and not flags.Trigger.enableL1CaloLegacy:
        # No Run-2 L1 RoIs -> nothing to covert to xAOD -> don't add RoIBResultToxAOD
        _log.debug('Not adding RoIBResultToxAOD because no Run-2 L1 system is enabled by flags')
        return ComponentAccumulator()

    acc = ComponentAccumulator()
    alg = CompFactory.RoIBResultToxAOD('RoIBResultToxAOD')
    alg.DoMuon = flags.Detector.EnableMuon and not flags.Trigger.enableL1MuonPhase1
    alg.DoCalo = flags.Detector.EnableCalo and flags.Trigger.enableL1CaloLegacy

    if flags.Input.Format == 'POOL':
        if str(alg.xAODKeyMuon) in flags.Input.Collections:
            _log.debug('L1Muon xAOD already in the input file, setting RoIBResultToxAOD.DoMuon=False')
            alg.DoMuon = False
        allCaloOutputs = [k.Path for k in [alg.xAODKeyEmTau, alg.xAODKeyEsum, alg.xAODKeyJetEt, alg.xAODKeyJet]]
        if all([k in flags.Input.Collections for k in allCaloOutputs]):
            _log.debug('L1Calo xAOD already in the input file, setting RoIBResultToxAOD.DoCalo=False')
            alg.DoCalo = False

    if not alg.DoMuon and not alg.DoCalo:
        _log.debug('Not adding RoIBResultToxAOD because both DoMuon and DoCalo properties are False')
        return ComponentAccumulator()

    acc.addEventAlgo(alg)

    if flags.Input.Format == 'BS':
        from TrigT1CaloByteStream.LVL1CaloRun2ByteStreamConfig import LVL1CaloRun2ReadBSCfg
        acc.merge(LVL1CaloRun2ReadBSCfg(flags, forRoIBResultToxAOD=True))

    # Create output list to return for use by the caller
    outputList = []
    if alg.DoMuon:
        outputList += [
            (alg.xAODKeyMuon.Type,  alg.xAODKeyMuon.Path)
        ]
    if alg.DoCalo:
        outputList += [
            (alg.xAODKeyEmTau.Type, alg.xAODKeyEmTau.Path),
            (alg.xAODKeyEsum.Type,  alg.xAODKeyEsum.Path),
            (alg.xAODKeyJetEt.Type, alg.xAODKeyJetEt.Path),
            (alg.xAODKeyJet.Type,   alg.xAODKeyJet.Path)
        ]

    return acc, outputList
