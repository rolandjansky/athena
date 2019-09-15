#
# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
#

def DataQualityToolsConfig(flags):
    from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
    from .DQTDataFlowMonAlg import DQTDataFlowMonAlgConfig
    from .DQTDetSynchMonAlg import DQTDetSynchMonAlgConfig
    
    result = ComponentAccumulator()
    result.merge(DQTDataFlowMonAlgConfig(flags))

    # really should only configure when input is RAW
    result.merge(DQTDetSynchMonAlgConfig(flags))

    return result
