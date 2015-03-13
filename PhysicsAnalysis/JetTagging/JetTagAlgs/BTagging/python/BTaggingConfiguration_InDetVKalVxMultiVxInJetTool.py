# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# Configuration functions for InDetVKalVxMultiVxInJetTool
# Author: Wouter van den Wollenberg (2014)
# Note: This tool has its own file because it is common to many different taggers
# so deletion of the one tagger that has this function won't break the others.
from BTagging.BTaggingFlags import BTaggingFlags
from AtlasGeoModel.InDetGMJobProperties import GeometryFlags as geoFlags

metaInDetVKalMultiVxInJetTool = { 'IsAVertexFinder' : True,
                                  'VertexFinderxAODBaseName' : 'MSV',
                                  'OneInTotal'      : True,
                                  'DependsOn'       : ['AtlasExtrapolator',
                                                       'BTagTrackToVertexTool',
                                                       'MSVVariablesFactory'],
                                  }

def toolInDetVKalMultiVxInJetTool(name, useBTagFlagsDefaults = True, **options):
    """Sets up a InDetVKalMultiVxInJetTool tool and returns it.

    The following options have BTaggingFlags defaults:

    OutputLevel                         default: BTaggingFlags.OutputLevel
    getNegativeTail                     default: False
    ConeForTag                          default: 1.0
    MultiVertex                         default: True

    input:             name: The name of the tool (should be unique).
      useBTagFlagsDefaults : Whether to use BTaggingFlags defaults for options that are not specified.
                  **options: Python dictionary with options for the tool.
    output: The actual tool, which can then by added to ToolSvc via ToolSvc += output. Note however
    that this tool also needs to be added to the main B-tagging tool."""
    if useBTagFlagsDefaults:
        defaults = { 'OutputLevel'     : BTaggingFlags.OutputLevel,
                     'getNegativeTail' : False,
                     'ExistIBL'        : geoFlags.isIBL(),
                     'ConeForTag'      : 1.0,
                     'MultiVertex'     : True }
        for option in defaults:
            options.setdefault(option, defaults[option])
    options['name'] = name
    from InDetVKalVxInJetTool.InDetVKalVxInJetToolConf import InDet__InDetVKalVxInJetTool
    return InDet__InDetVKalVxInJetTool(**options)

#------------------------------------------------------------------

metaMSVVariablesFactory = { 'OneInTotal' : True, }

def toolMSVVariablesFactory(name, useBTagFlagsDefaults = True, **options):
    """Sets up a MSVVariablesFactory tool and returns it.

    The following options have BTaggingFlags defaults:

    OutputLevel                         default: BTaggingFlags.OutputLevel

    input:             name: The name of the tool (should be unique).
      useBTagFlagsDefaults : Whether to use BTaggingFlags defaults for options that are not specified.
                  **options: Python dictionary with options for the tool.
    output: The actual tool, which can then by added to ToolSvc via ToolSvc += output. Note however
    that this tool also needs to be added to the main B-tagging tool."""
    if useBTagFlagsDefaults:
        defaults = { 'OutputLevel'     : BTaggingFlags.OutputLevel, }
        for option in defaults:
            options.setdefault(option, defaults[option])
    options['name'] = name
    from JetTagTools.JetTagToolsConf import Analysis__MSVVariablesFactory
    return Analysis__MSVVariablesFactory(**options)
