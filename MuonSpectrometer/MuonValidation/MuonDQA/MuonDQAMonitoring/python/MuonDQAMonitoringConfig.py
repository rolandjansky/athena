#
#  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
#

def MuonDQAMonitoringConfig(flags):
    from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator

    result = ComponentAccumulator()

    if flags.DQ.Steering.Muon.doRawMon:
        from TgcRawDataMonitoring.TgcRawDataMonitorAlgorithm import TgcRawDataMonitoringConfig
    
        result.merge(TgcRawDataMonitoringConfig(flags))
        
    return result
