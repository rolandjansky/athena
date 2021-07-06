#
#  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
#

'''@file TrackingConfigurationWorkaround.py
@author Peter Onyisi
@date 2019-12-17
@brief Centralize the DQ workarounds for setting up ID tracking tools in new-style config, until this is provided centrally
'''
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from InDetConfig import TrackingCommonConfig

def TrackSummaryToolWorkaround(flags):
    result = ComponentAccumulator()
    result.setPrivateTools( result.popToolsAndMerge(TrackingCommonConfig.InDetTrackSummaryToolCfg(flags) ) )
    return result
