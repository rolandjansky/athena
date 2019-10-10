#
# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
#

def TauMonitoringConfig(flags):
    from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
    result = ComponentAccumulator()

    from .tauMonitorAlgorithm import tauMonitoringConfig
    result.merge(tauMonitoringConfig(flags))

    return result
