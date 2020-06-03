#
# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
#

def DataQualityToolsConfig(flags):
    from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
    from .DQTDataFlowMonAlg import DQTDataFlowMonAlgConfig
    from .DQTLumiMonAlg import DQTLumiMonAlgConfig
    from .DQTBackgroundMon import DQTBackgroundMonAlgConfig
    from .DQTDetSynchMonAlg import DQTDetSynchMonAlgConfig

    result = ComponentAccumulator()
    # the following should run in all configurations
    result.merge(DQTDataFlowMonAlgConfig(flags))

    # the following should not run in RAW to ESD, if we're in two-step
    if flags.DQ.Environment != 'tier0Raw':
        if flags.DQ.DataType != 'cosmics':
            result.merge(DQTLumiMonAlgConfig(flags))

    # only when input is RAW
    if flags.DQ.Environment in ('online', 'tier0', 'tier0Raw'):
        result.merge(DQTDetSynchMonAlgConfig(flags))
        result.merge(DQTBackgroundMonAlgConfig(flags))

    return result
