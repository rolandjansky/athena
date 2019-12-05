#
#  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
#

def MuonDQAMonitoringConfig(flags):
    from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator

    result = ComponentAccumulator()

    if flags.DQ.Steering.Muon.doRawMon:
        from TgcRawDataMonitoring.TgcRawDataMonitorAlgorithm import TgcRawDataMonitoringConfig
        from MdtRawDataMonitoring.MDTMonitorAlgorithm import MdtMonitoringConfig
    
        result.merge(TgcRawDataMonitoringConfig(flags))
        result.merge(MdtMonitoringConfig(flags))
        
    return result
