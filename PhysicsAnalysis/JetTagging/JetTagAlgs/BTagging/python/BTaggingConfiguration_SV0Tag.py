# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# Configuration functions for SV0Tag
# Author: Wouter van den Wollenberg (2013-2014)
from BTagging.BTaggingFlags import BTaggingFlags

metaSV0Tag = { 'IsATagger'         : True,
               'xAODBaseName'      : 'SV0',
               'DependsOn'         : ['AtlasExtrapolator',
                                      'BTagTrackToVertexTool',
                                      'InDetVKalVxNegInJetTool'],
               #'PassByPointer'     : {'LikelihoodTool'  : 'NewLikelihoodTool'}, (apparently not used for SV0)
#$               'PassByName'        : {'SecVxFinderName' : 'InDetVKalVxNegInJetTool'},
               'JetCollectionList' : 'jetCollectionList',
               'ToolCollection'    : 'SV0Tag' }

def toolSV0Tag(name, useBTagFlagsDefaults = True, **options):
    """Sets up a SV0Tag tool and returns it.

    The following options have BTaggingFlags defaults:

    OutputLevel                         default: BTaggingFlags.OutputLevel
    Runmodus                            default: BTaggingFlags.Runmodus
    referenceType                       default: BTaggingFlags.ReferenceType
    SVAlgType                           default: "SV0"
    jetCollectionList                   default: BTaggingFlags.Jets
    SecVxFinderName                     default: "SV0"

    input:             name: The name of the tool (should be unique).
      useBTagFlagsDefaults : Whether to use BTaggingFlags defaults for options that are not specified.
                  **options: Python dictionary with options for the tool.
    output: The actual tool, which can then by added to ToolSvc via ToolSvc += output."""
    if useBTagFlagsDefaults:
        defaults = { 'OutputLevel'                      : BTaggingFlags.OutputLevel,
                     'Runmodus'                         : BTaggingFlags.Runmodus,
                     'referenceType'                    : BTaggingFlags.ReferenceType,
                     'SVAlgType'                        : 'SV0',
                     'jetCollectionList'                : BTaggingFlags.Jets,
#                     'originalTPCollectionName'         : BTaggingFlags.TrackParticleCollectionName,
#                     'infoPlusName'                     : 'SV0InfoPlus',
                     'SecVxFinderName'                  : 'SV0',
                     }
        for option in defaults:
            options.setdefault(option, defaults[option])
    options['name'] = name
    from JetTagTools.JetTagToolsConf import Analysis__SVTag
    return Analysis__SVTag(**options)

#-----------------------------------------------------------------

metaInDetVKalVxNegInJetTool = { 'IsAVertexFinder'          : True,
                                'VertexFinderxAODBaseName' : 'SV0',
                                'OneInTotal'               : True,
                                'DependsOn'                : ['AtlasExtrapolator',
                                                              'BTagTrackToVertexTool'],
                                'ToolCollection'           : 'SV0Tag' }

def toolInDetVKalVxNegInJetTool(name, useBTagFlagsDefaults = True, **options):
    """Sets up a InDetVKalVxNegInJetTool tool and returns it.

    The following options have BTaggingFlags defaults:

    OutputLevel                         default: BTaggingFlags.OutputLevel
    getNegativeTail                     default: True
    CutA0                               default: 2.0 (new tuned cuts for SV0 (Sara))
    CutZVrt                             default: 2.0
    CutPixelHits                        default: 2

    input:             name: The name of the tool (should be unique).
      useBTagFlagsDefaults : Whether to use BTaggingFlags defaults for options that are not specified.
                  **options: Python dictionary with options for the tool.
    output: The actual tool, which can then by added to ToolSvc via ToolSvc += output. Note however
    that this tool also needs to be added to the main B-tagging tool."""
    if useBTagFlagsDefaults:
        defaults = { 'OutputLevel'      : BTaggingFlags.OutputLevel,
#                     'TrackSummaryTool' : None ,
                     'getNegativeTail'  : True,
                     'CutA0'            : 2.0,
                     'CutZVrt'          : 2.0,
                     'CutPixelHits'     : 2 }
        for option in defaults:
            options.setdefault(option, defaults[option])
    options['name'] = name
    from InDetVKalVxInJetTool.InDetVKalVxInJetToolConf import InDet__InDetVKalVxInJetTool
    return InDet__InDetVKalVxInJetTool(**options)
