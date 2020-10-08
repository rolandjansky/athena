#
# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
#

def TauMonitoringConfig(flags):
    from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
    result = ComponentAccumulator()

    # the following should not run in RAW to ESD, if we're in two-step
    if flags.DQ.Environment != 'tier0Raw':
        from .tauMonitorAlgorithm import tauMonitoringConfig
        result.merge(tauMonitoringConfig(flags))

    return result
