#
# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
#

def DataQualityToolsConfig(flags):
    from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
    from .DQTDataFlowMonAlg import DQTDataFlowMonAlgConfig
    
    result = ComponentAccumulator()
    result.merge(DQTDataFlowMonAlgConfig(flags))

    return result
