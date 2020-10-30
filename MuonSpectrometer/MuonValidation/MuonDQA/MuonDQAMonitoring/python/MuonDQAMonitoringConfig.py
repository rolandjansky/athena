#
#  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
#
def MuonDQAMonitoringConfig(flags):

    from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator

    result = ComponentAccumulator()

    if flags.DQ.Steering.Muon.doRawMon:
        # do not run in RAW->ESD, or AOD-only
        if flags.DQ.Environment not in ('tier0Raw', 'AOD'):
            from MdtRawDataMonitoring.MDTMonitorAlgorithm import MdtMonitoringConfig
            from RpcRawDataMonitoring.RpcMonitorAlgorithm import RpcMonitoringConfig
            from TgcRawDataMonitoring.TgcRawDataMonitorAlgorithm import TgcRawDataMonitoringConfig
            result.merge(MdtMonitoringConfig(flags))
            result.merge(RpcMonitoringConfig(flags))
            result.merge(TgcRawDataMonitoringConfig(flags))
            if flags.Detector.GeometryCSC:
                from CscRawDataMonitoring.CscMonitorAlgorithm import CscMonitoringConfig
                result.merge(CscMonitoringConfig(flags))
            if flags.Detector.GeometryMM:
                from MMRawDataMonitoring.MMMonitorAlgorithm import MMMonitoringConfig
                result.merge(MMMonitoringConfig(flags))

    if flags.DQ.Steering.Muon.doTrackMon:
        # do not run in RAW->ESD
        if flags.DQ.Environment not in ('tier0Raw',):
            from MuonTrackMonitoring.MuonTrackMonitorAlgorithm import MuonTrackConfig
            result.merge(MuonTrackConfig(flags))
    return result
