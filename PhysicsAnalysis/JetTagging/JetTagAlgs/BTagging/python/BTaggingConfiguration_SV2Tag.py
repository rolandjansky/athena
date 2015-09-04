# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# Configuration functions for SV2Tag
# Author: Wouter van den Wollenberg (2013-2014)
from BTagging.BTaggingFlags import BTaggingFlags

metaSV2Tag = { 'IsATagger'         : True,
               'xAODBaseName'      : 'SV2',
               'DependsOn'         : ['AtlasExtrapolator',
                                      'BTagTrackToVertexTool',
                                      'InDetVKalVxInJetTool',
                                      'SV2NewLikelihoodTool'],
               'PassByPointer'     : {'LikelihoodTool'  : 'SV2NewLikelihoodTool'},
#               'PassByName'        : {'SecVxFinderName' : 'InDetVKalVxInJetTool'},
               'JetCollectionList' : 'jetCollectionList',
               'ToolCollection'    : 'SV2Tag' }

def toolSV2Tag(name, useBTagFlagsDefaults = True, **options):
    """Sets up a SV2Tag tool and returns it.

    The following options have BTaggingFlags defaults:

    OutputLevel                         default: BTaggingFlags.OutputLevel
    Runmodus                            default: BTaggingFlags.Runmodus
    referenceType                       default: BTaggingFlags.ReferenceType
    SVAlgType                           default: "SV2"
    jetCollectionList                   default: BTaggingFlags.Jets
    LikelihoodTool                      default: None
    SecVxFinderName                     default: "SV1"
    UseCHypo                            default: True

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
                     'SecVxFinderName'                  : 'SV1',
                     'UseCHypo'                         : True,
#                     'originalTPCollectionName'         : BTaggingFlags.TrackParticleCollectionName,
                     'LikelihoodTool'                   : None }
        for option in defaults:
            options.setdefault(option, defaults[option])
    options['name'] = name
    from JetTagTools.JetTagToolsConf import Analysis__SVTag
    return Analysis__SVTag(**options)

#-----------------------------------------------------------------

metaSV2NewLikelihoodTool = { 'CalibrationFolders' : ['SV2',],
                             'DependsOn'          : ['BTagCalibrationBrokerTool',],
                             'PassByPointer'      : {'calibrationTool' : 'BTagCalibrationBrokerTool' },
                             'ToolCollection'     : 'SV2Tag' }

def toolSV2NewLikelihoodTool(name, useBTagFlagsDefaults = True, **options):
    """Sets up a SV2NewLikelihoodTool tool and returns it.

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
