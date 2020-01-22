
#
# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
#

def TRTMonitoringRun3Cfg(flags):
    from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
    from .TRTMonitoringRun3_Alg import TRTMonitoringRun3_AlgConfig

    result = ComponentAccumulator()

    # do not run in RAW->ESD (if two step) or AOD-only
    if flags.DQ.Environment not in ('tier0Raw', 'AOD'):
        result.merge(TRTMonitoringRun3_AlgConfig(flags))

    return result
