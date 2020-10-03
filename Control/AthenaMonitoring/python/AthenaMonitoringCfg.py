# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator

def AthenaMonitoringCfg(flags):
    import logging
    local_logger = logging.getLogger('AthenaMonitoringCfg')
    info = local_logger.info
    result = ComponentAccumulator()

    if flags.DQ.Steering.doPixelMon:
        info('Set up Pixel monitoring')
        from PixelMonitoring.PixelMonitoringConfig import PixelMonitoringConfig
        result.merge(PixelMonitoringConfig(flags))
    
    if flags.DQ.Steering.doSCTMon:
        info('Set up SCT monitoring')
        from SCT_Monitoring.SCTMonitoringConfig import SCTMonitoringConfig
        result.merge(SCTMonitoringConfig(flags))

    if flags.DQ.Steering.doTRTMon:
        info('Set up TRT monitoring')
        from TRTMonitoringRun3.TRTMonitoringRun3Config import TRTMonitoringRun3Cfg
        result.merge(TRTMonitoringRun3Cfg(flags))
    
    if flags.DQ.Steering.doInDetMon:
        info('Set up InDet Global monitoring')
        from InDetGlobalMonitoringRun3Test.InDetGlobalMonitoringRun3TestConfig import InDetGlobalMonitoringRun3TestConfig
        result.merge(InDetGlobalMonitoringRun3TestConfig(flags))

    if flags.DQ.Steering.doLArMon:
        info('Set up LAr monitoring')
        from LArConfiguration.LArMonitoringConfig import LArMonitoringConfig
        result.merge(LArMonitoringConfig(flags))

    if flags.DQ.Steering.doTileMon:
        info('Set up Tile monitoring')
        from TileMonitoring.TileMonitoringConfig import TileMonitoringCfg
        result.merge(TileMonitoringCfg(flags))

    if flags.DQ.Steering.doCaloGlobalMon:
        info('Set up Calo monitoring')
        from CaloMonitoring.CaloMonitoringConfig import CaloMonitoringCfg
        result.merge(CaloMonitoringCfg(flags))

    if flags.DQ.Steering.doMuonMon:
        info('Set up Muon monitoring')
        from MuonDQAMonitoring.MuonDQAMonitoringConfig import MuonDQAMonitoringConfig
        result.merge(MuonDQAMonitoringConfig(flags))

    if flags.DQ.Steering.doHLTMon:
        info('Set up HLT monitoring')
        from TrigHLTMonitoring.TrigHLTMonitorAlgorithm import TrigHLTMonTopConfig
        result.merge(TrigHLTMonTopConfig(flags))

    if flags.DQ.Steering.doJetTagMon:
        info('Set up JetTagging monitoring')
        from JetTagMonitoring.JetTagMonitorAlgorithm import JetTagMonitorConfig
        result.merge(JetTagMonitorConfig(flags))

    if flags.DQ.Steering.doEgammaMon:
        info('Set up Egamma monitoring')
        from egammaPerformance.egammaMonitoringConfig import egammaMonitoringConfig
        result.merge(egammaMonitoringConfig(flags))

    if flags.DQ.Steering.doJetMon:
        info('Set up Jet monitoring')
        from JetMonitoring.JetMonitoringStandard import standardJetMonitoring
        result.merge(standardJetMonitoring(flags))

    if flags.DQ.Steering.doMissingEtMon:
        info("Set up MET monitoring")
        from MissingETMonitoring.METMonitorAlgorithm import METMonitoringConfig
        result.merge(METMonitoringConfig(flags))

    if flags.DQ.Steering.doGlobalMon:
        info('Set up Global monitoring')
        from DataQualityTools.DataQualityToolsConfig import DataQualityToolsConfig
        result.merge(DataQualityToolsConfig(flags))

    if flags.DQ.Steering.doTauMon:
        info('Set up Tau monitoring')
        from tauMonitoring.TauMonitoringConfig import TauMonitoringConfig
        result.merge(TauMonitoringConfig(flags))

    if flags.DQ.Steering.doAFPMon:
        info('Set up AFP monitoring')
        from Run3AFPMonitoring.Run3AFPExampleMonitorAlgorithm import Run3AFPExampleMonitoringConfig
        result.merge(Run3AFPExampleMonitoringConfig(flags))

    if flags.DQ.Steering.doLVL1CaloMon:
        info('Set up LVL1Calo monitoring')
        from TrigT1CaloMonitoring.LVL1CaloMonitoringConfig import LVL1CaloMonitoringConfig
        result.merge(LVL1CaloMonitoringConfig(flags))
        
    return result
