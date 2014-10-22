# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# Configuration functions for GbbNNTag
# Author: Wouter van den Wollenberg (2013-2014)
from BTagging.BTaggingFlags import BTaggingFlags

metaGbbNNTag = { 'IsATagger'       : True,
                 'DependsOn'       : ['AtlasExtrapolator',
                                      'BTagTrackToVertexTool',
                                      'GbbNNTagTrackSelector'],
                 'PassByPointer'   : {'trackSelectorTool' : 'GbbNNTagTrackSelector'},
                 'PassTracksAs'    : 'trackAssociation',
                 'TypeCollection'  : 'GbbNNTag' }

def toolGbbNNTag(name, useBTagFlagsDefaults = True, **options):
    """Sets up a GbbNNTag tool and returns it.

    The following options have BTaggingFlags defaults:

    OutputLevel                         default: BTaggingFlags.OutputLevel
    Runmodus                            default: BTaggingFlags.Runmodus
    NNweightsFileName                   default: "GbbNNweightsFile.txt"
    trackAssociation                    default: "Tracks"

    input:             name: The name of the tool (should be unique).
      useBTagFlagsDefaults : Whether to use BTaggingFlags defaults for options that are not specified.
                  **options: Python dictionary with options for the tool.
    output: The actual tool, which can then by added to ToolSvc via ToolSvc += output."""
    if useBTagFlagsDefaults:
        defaults = { 'OutputLevel'                      : BTaggingFlags.OutputLevel,
                     'Runmodus'                         : BTaggingFlags.Runmodus,
                     'NNweightsFileName'                : 'GbbNNweightsFile.txt',
                     'trackAssociation'                 : "Tracks" }
        for option in defaults:
            options.setdefault(option, defaults[option])
    options['name'] = name
    from JetTagTools.JetTagToolsConf import Analysis__GbbNNTag
    return Analysis__GbbNNTag(**options)

#-------------------------------------------------------------------

metaGbbNNTagTrackSelector = { 'DependsOn'      : ['BTagTrackToVertexTool',],
                              'PassByPointer'  : {'trackToVertexTool' : 'BTagTrackToVertexTool'},
                              'TypeCollection' : 'GbbNNTag' }

def toolGbbNNTagTrackSelector(name, useBTagFlagsDefaults = True, **options):
    """Sets up a GbbNNTagTrackSelector tool and returns it.

    The following options have BTaggingFlags defaults:

    OutputLevel                         default: BTaggingFlags.OutputLevel

    input:             name: The name of the tool (should be unique).
      useBTagFlagsDefaults : Whether to use BTaggingFlags defaults for options that are not specified.
                  **options: Python dictionary with options for the tool.
    output: The actual tool, which can then by added to ToolSvc via ToolSvc += output."""
    if useBTagFlagsDefaults:
        defaults = { 'OutputLevel'                      : BTaggingFlags.OutputLevel }
        for option in defaults:
            options.setdefault(option, defaults[option])
    options['name'] = name
    from JetTagTools.JetTagToolsConf import Analysis__TrackSelector
    return Analysis__TrackSelector(**options)
