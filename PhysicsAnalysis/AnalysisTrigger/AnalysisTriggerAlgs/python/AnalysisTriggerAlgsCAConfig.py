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

    if flags.Input.Format == 'BS':
        from TrigT1CaloByteStream.LVL1CaloRun2ByteStreamConfig import LVL1CaloRun2ReadBSCfg
        acc.merge(LVL1CaloRun2ReadBSCfg(flags, forRoIBResultToxAOD=True))

    # Create output list to return for use by the caller
    outputList = []
    if alg.DoMuon:
        outputList += [
            (alg.xAODKeyMuon.Type,  alg.xAODKeyMuon.Path)
        ]
        from MuonConfig.MuonGeometryConfig import MuonGeoModelCfg
        acc.merge(MuonGeoModelCfg(flags))
        # RPC
        from MuonConfig.MuonCablingConfig import RPCCablingConfigCfg
        acc.merge(RPCCablingConfigCfg(flags))
        #TGC
        from MuonConfig.MuonCablingConfig import TGCCablingConfigCfg
        acc.merge(TGCCablingConfigCfg(flags))

    if alg.DoCalo:
        outputList += [
            (alg.xAODKeyEmTau.Type, alg.xAODKeyEmTau.Path),
            (alg.xAODKeyEsum.Type,  alg.xAODKeyEsum.Path),
            (alg.xAODKeyJetEt.Type, alg.xAODKeyJetEt.Path),
            (alg.xAODKeyJet.Type,   alg.xAODKeyJet.Path)
        ]
        from  TrigConfigSvc.TrigConfigSvcCfg import L1ConfigSvcCfg
        acc.merge(L1ConfigSvcCfg(flags))

    acc.addEventAlgo(alg)

    return acc, outputList


if __name__ == "__main__":
    from AthenaConfiguration.MainServicesConfig import MainServicesCfg
    from AthenaCommon import Configurable
    Configurable.ConfigurableRun3Behavior = 1
    from AthenaConfiguration.AllConfigFlags import ConfigFlags as flags
    from AthenaConfiguration.TestDefaults import defaultTestFiles

    flags.Input.Files = defaultTestFiles.RAW
    flags.Exec.MaxEvents = 20
    flags.lock()

    topCA = MainServicesCfg(flags)
    from TriggerJobOpts.TriggerByteStreamConfig import ByteStreamReadCfg
    topCA.merge(ByteStreamReadCfg(flags))
    from TrigT1ResultByteStream.TrigT1ResultByteStreamConfig import L1TriggerByteStreamDecoderCfg
    topCA.merge(L1TriggerByteStreamDecoderCfg(flags))
    ca, output = RoIBResultToxAODCfg(flags)
    topCA.merge(ca)
    status = topCA.run()
    if status.isFailure():
        import sys
        sys.exit(-1)
