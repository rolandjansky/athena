# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from AtlasGeoModel.InDetGMJobProperties import InDetGeometryFlags as geoFlags

# import the InDetVKalVxInJetTool configurable
InDet__InDetVKalVxInJetTool=CompFactory.InDet.InDetVKalVxInJetTool

def InDetVKalVxInJetToolCfg(name, MSV = False, useBTagFlagsDefaults = True, **options):
    """Sets up a InDetVKalVxInJetTool tool and returns it.

    The following options have BTaggingFlags defaults:

    TrackSummaryTool                    default: None (not needed for b-tagging)

    input:             name: The name of the tool (should be unique).
      useBTagFlagsDefaults : Whether to use BTaggingFlags defaults for options that are not specified.
                  **options: Python dictionary with options for the tool.
    output: The actual tool, which can then by added to ToolSvc via ToolSvc += output. Note however
    that this tool also needs to be added to the main B-tagging tool."""
    acc = ComponentAccumulator()
    if useBTagFlagsDefaults:
        if MSV:
            defaults = {
                     'getNegativeTail' : False,
                     'ExistIBL'        : geoFlags.isIBL(),
                     'ConeForTag'      : 1.0,
                     'MultiVertex'     : True,
                     }
        else:
            defaults = {
                     'ExistIBL'         : geoFlags.isIBL(),
                     }
        for option in defaults:
            options.setdefault(option, defaults[option])
    options['name'] = name
    acc.setPrivateTools(InDet__InDetVKalVxInJetTool(**options))

    return acc
