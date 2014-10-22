# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# Configuration functions for MultiSV
# Author: Wouter van den Wollenberg (2013-2014)
# NOTE: This tool was called MutliSV in the old implementation; I have fixed this
from BTagging.BTaggingFlags import BTaggingFlags

metaMultiSVTag = { 'IsATagger'         : True,
                   'DependsOn'         : ['AtlasExtrapolator',
                                          'BTagTrackToVertexTool',
                                          'InDetVKalMultiVxInJetTool'],
                   'PassByName'        : {'SecVxFinderName' : 'InDetVKalMultiVxInJetTool'},
                   'JetCollectionList' : 'jetCollectionList',
                   'JetWithInfoPlus'   : 'jetWithInfoPlus',
                   'DefaultTracks'     : 'TracksBB',
                   'ToolCollection'    : 'MultiSVTag' }

def toolMultiSVTag(name, useBTagFlagsDefaults = True, **options):
    """Sets up a MultiSVTag tool and returns it.

    The following options have BTaggingFlags defaults:

    OutputLevel                         default: BTaggingFlags.OutputLevel
    Runmodus                            default: BTaggingFlags.Runmodus
    infoPlusName                        default: "MultiSVInfoPlus"
    originalTPCollectionName            default: BTaggingFlags.TrackParticleCollectionName
    jetCollectionList                   default: BTaggingFlags.Jets
    jetWithInfoPlus                     default: BTaggingFlags.JetsWithInfoPlus
    SecVxFinderName                     default: "InDetVKalMultiVxInJetTool"

    input:             name: The name of the tool (should be unique).
      useBTagFlagsDefaults : Whether to use BTaggingFlags defaults for options that are not specified.
                  **options: Python dictionary with options for the tool.
    output: The actual tool, which can then by added to ToolSvc via ToolSvc += output."""
    if useBTagFlagsDefaults:
        defaults = { 'OutputLevel'                      : BTaggingFlags.OutputLevel,
                     'Runmodus'                         : BTaggingFlags.Runmodus,
                     'infoPlusName'                     : 'MultiSVInfoPlus',
                     'originalTPCollectionName'         : BTaggingFlags.TrackParticleCollectionName,
                     'jetCollectionList'                : BTaggingFlags.Jets,
                     'jetWithInfoPlus'                  : BTaggingFlags.JetsWithInfoPlus,
                     'SecVxFinderName'                  : 'InDetVKalMultiVxInJetTool' }
        for option in defaults:
            options.setdefault(option, defaults[option])
    options['name'] = name
    from JetTagTools.JetTagToolsConf import Analysis__MultiSVTag
    return Analysis__MultiSVTag(**options)

#---------------------------------------------------------------------

metaInDetVKalMultiVxInJetTool = { 'IsAVertexFinder' : True,
                                  'OneInTotal'      : True,
                                  'DependsOn'       : ['AtlasExtrapolator',
                                                       'BTagTrackToVertexTool'],
                                  'ToolCollection'  : 'MultiSVTag' }

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
                     'ConeForTag'      : 1.0,
                     'MultiVertex'     : True }
        for option in defaults:
            options.setdefault(option, defaults[option])
    getNegativeTail = options['getNegativeTail']
    ConeForTag = options['ConeForTag']
    MultiVertex = options['MultiVertex']
    OutputLevel = options['OutputLevel']
    del options['getNegativeTail']
    del options['ConeForTag']
    del options['MultiVertex']
    del options['OutputLevel']
    options['name'] = name
    from InDetVKalVxInJetTool.InDetVKalVxInJetFinder import AtlasVKalVxInJetFinder
    tool = AtlasVKalVxInJetFinder(**options)
    # For some reason the following options need to be fed to the tool post-creation
    tool.getNegativeTail = getNegativeTail
    tool.ConeForTag = ConeForTag
    tool.MultiVertex = MultiVertex
    tool.OutputLevel = OutputLevel
    return tool
