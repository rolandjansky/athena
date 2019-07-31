# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator

def AthenaMonitoringCfg(flags):
    import logging
    local_logger = logging.getLogger('AthenaMonitoringCfg')
    result = ComponentAccumulator()

    if flags.DQ.Steering.doSCTMon:
        local_logger.info('Set up SCT monitoring')
        from SCT_Monitoring.SCTLorentzMonAlg import SCTLorentzMonAlgConfig
        result.merge(SCTLorentzMonAlgConfig(flags))

    if flags.DQ.Steering.doTileMon:
        local_logger.info('Set up Tile monitoring')
        from TileMonitoring.TileJetMonitorAlgorithm import TileJetMonitoringConfig
        result.merge(TileJetMonitoringConfig(flags))

    if flags.DQ.Steering.doHLTMon:
        local_logger.info('Set up HLT monitoring')
        from TrigHLTMonitoring.TrigHLTMonitoringConfig import TrigHLTMonitoringConfig
        result.merge(TrigHLTMonitoringConfig(flags))

    if flags.DQ.Steering.doGlobalMon:
        local_logger.info('Set up Global monitoring')
        from DataQualityTools.DataQualityToolsConfig import DataQualityToolsConfig
        result.merge(DataQualityToolsConfig(flags))

    return result
