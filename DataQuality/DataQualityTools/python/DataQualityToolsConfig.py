#
# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
#

def DataQualityToolsConfig(flags):
    from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
    from .DQTDataFlowMonAlg import DQTDataFlowMonAlgConfig
    from .DQTLumiMonTool import DQTLumiMonAlgConfig
    from .DQTBackgroundMon import DQTBackgroundMonAlgConfig
    from .DQTDetSynchMonAlg import DQTDetSynchMonAlgConfig

    result = ComponentAccumulator()
    result.merge(DQTDataFlowMonAlgConfig(flags))
    result.merge(DQTLumiMonAlgConfig(flags))
    result.merge(DQTBackgroundMonAlgConfig(flags))

    # only when input is RAW
    if flags.DQ.Environment in ('online', 'tier0', 'tier0Raw'):
        result.merge(DQTDetSynchMonAlgConfig(flags))

    return result
