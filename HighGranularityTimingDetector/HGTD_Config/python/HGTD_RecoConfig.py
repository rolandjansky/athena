# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator

def HGTD_RecoCfg(flags):
    """Configures HGTD track extension
    (currently only decorates tracks with relevant info) """
    result = ComponentAccumulator()

    from HGTD_Config.HGTD_PrepRawDataFormationConfig import PadClusterizationCfg
    result.merge(PadClusterizationCfg(flags))

    from HGTD_Config.HGTD_TrackTimeExtensionConfig import TrackTimeExtensionCfg
    result.merge(TrackTimeExtensionCfg(flags))

    return result
