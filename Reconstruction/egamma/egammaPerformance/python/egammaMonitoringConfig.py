#
# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
#

def egammaMonitoringConfig(flags):
    from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
    result = ComponentAccumulator()

    # don't run in RAW -> ESD
    if flags.DQ.Environment in ('tier0', 'tier0ESD',' online', 'AOD'):
        from egammaPerformance.SetupEgammaMonitoring import MonitorElectronConfig, MonitorPhotonConfig, MonitorTnPConfig, MonitorForwardElectronConfig

        result.merge(MonitorElectronConfig(flags))
        result.merge(MonitorPhotonConfig(flags))
        result.merge(MonitorTnPConfig(flags))
        result.merge(MonitorForwardElectronConfig(flags))

    return result
