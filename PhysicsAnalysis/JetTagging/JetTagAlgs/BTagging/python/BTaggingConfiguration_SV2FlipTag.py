# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# Configuration functions for SV2FlipTag
# Author: Wouter van den Wollenberg (2013-2014)
from BTagging.BTaggingFlags import BTaggingFlags

metaSV2FlipTag = { 'IsATagger'         : True,
                   'DependsOn'         : ['AtlasExtrapolator',
                                          'BTagTrackToVertexTool',
                                          'InDetVKalVxNegativeTagInJetTool',
                                          'SV2FlipNewLikelihoodTool'],
                   'PassByPointer'     : {'LikelihoodTool'  : 'SV2FlipNewLikelihoodTool'},
                   'PassByName'        : {'SecVxFinderName' : 'InDetVKalVxNegativeTagInJetTool'},
                   'JetCollectionList' : 'jetCollectionList',
                   'JetWithInfoPlus'   : 'jetWithInfoPlus',
                   'ToolCollection'    : 'SV2FlipTag' }

def toolSV2FlipTag(name, useBTagFlagsDefaults = True, **options):
    """Sets up a SV2FlipTag tool and returns it.

    The following options have BTaggingFlags defaults:

    OutputLevel                         default: BTaggingFlags.OutputLevel
    Runmodus                            default: BTaggingFlags.Runmodus
    referenceType                       default: BTaggingFlags.ReferenceType
    SVAlgType                           default: "SV2"
    jetCollectionList                   default: BTaggingFlags.Jets
    jetWithInfoPlus                     default: BTaggingFlags.JetsWithInfoPlus
    originalTPCollectionName            default: BTaggingFlags.TrackParticleCollectionName
    writeInfoPlus                       default: False
    SecVxFinderName                     default: "InDetVKalVxNegativeTagInJetTool"
    LikelihoodTool                      default: None

    input:             name: The name of the tool (should be unique).
      useBTagFlagsDefaults : Whether to use BTaggingFlags defaults for options that are not specified.
                  **options: Python dictionary with options for the tool.
    output: The actual tool, which can then by added to ToolSvc via ToolSvc += output."""
    if useBTagFlagsDefaults:
        defaults = { 'OutputLevel'                      : BTaggingFlags.OutputLevel,
                     'Runmodus'                         : BTaggingFlags.Runmodus,
                     'referenceType'                    : BTaggingFlags.ReferenceType,
                     'SVAlgType'                        : 'SV2',
                     'jetCollectionList'                : BTaggingFlags.Jets,
                     'jetWithInfoPlus'                  : BTaggingFlags.JetsWithInfoPlus,
                     'originalTPCollectionName'         : BTaggingFlags.TrackParticleCollectionName,
                     'writeInfoPlus'                    : False,
                     'SecVxFinderName'                  : 'InDetVKalVxNegativeTagInJetTool',
                     'LikelihoodTool'                   : None }
        for option in defaults:
            options.setdefault(option, defaults[option])
    options['name'] = name
    from JetTagTools.JetTagToolsConf import Analysis__SVTag
    return Analysis__SVTag(**options)

#---------------------------------------------------------------------

metaSV2FlipNewLikelihoodTool = { 'CalibrationFolders' : ['SV2',],
                                 'DependsOn'          : ['BTagCalibrationBrokerTool',],
                                 'PassByPointer'      : {'calibrationTool' : 'BTagCalibrationBrokerTool' },
                                 'ToolCollection'     : 'SV2FlipTag' }

def toolSV2FlipNewLikelihoodTool(name, useBTagFlagsDefaults = True, **options):
    """Sets up a SV2FlipNewLikelihoodTool tool and returns it.

    The following options have BTaggingFlags defaults:

    OutputLevel                         default: BTaggingFlags.OutputLevel
    taggerName                          default: "SV2"

    input:             name: The name of the tool (should be unique).
      useBTagFlagsDefaults : Whether to use BTaggingFlags defaults for options that are not specified.
                  **options: Python dictionary with options for the tool.
    output: The actual tool, which can then by added to ToolSvc via ToolSvc += output."""
    if useBTagFlagsDefaults:
        defaults = { 'OutputLevel'                         : BTaggingFlags.OutputLevel,
                     'taggerName'                          : 'SV2' }
        for option in defaults:
            options.setdefault(option, defaults[option])
    options['name'] = name
    from JetTagTools.JetTagToolsConf import Analysis__NewLikelihoodTool
    return Analysis__NewLikelihoodTool(**options)
