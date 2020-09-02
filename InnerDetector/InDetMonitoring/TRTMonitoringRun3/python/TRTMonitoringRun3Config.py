
#
# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
#

def TRTMonitoringRun3Cfg(flags):
    from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
    from .TRTMonitoringRun3ESD_Alg import TRTMonitoringRun3ESD_AlgConfig
    from .TRTMonitoringRun3RAW_Alg import TRTMonitoringRun3RAW_AlgConfig

    result = ComponentAccumulator()

    # do not run in RAW->ESD (if two step) or AOD-only
    if flags.DQ.Environment not in ('tier0Raw', 'AOD'):
        result.merge(TRTMonitoringRun3ESD_AlgConfig(flags))
        
    # only when input is RAW
    if flags.DQ.Environment in ('online', 'tier0', 'tier0Raw'):
        result.merge(TRTMonitoringRun3RAW_AlgConfig(flags))

    return result
