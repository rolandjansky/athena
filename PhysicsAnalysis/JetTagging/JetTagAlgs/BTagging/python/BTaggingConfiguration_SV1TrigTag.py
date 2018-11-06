# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# Configuration functions for SV1Tag_Trig
# Author: Wouter van den Wollenberg (2013-2014)
from BTagging.BTaggingFlags import BTaggingFlags

metaSV1Tag_Trig = { 'IsATagger'         : True,
                   'xAODBaseName'      : 'SV1',
                   'DependsOn'         : ['AtlasExtrapolator',
                                          'BTagTrackToVertexTool',
                                          'InDetVKalVxInJetTool_Trig',
                                          'SV1NewLikelihoodTool_Trig'],
                   'PassByPointer'     : {'LikelihoodTool'  : 'SV1NewLikelihoodTool_Trig'},
                   'JetCollectionList' : 'jetCollectionList',
                   'ToolCollection'    : 'SV1Tag_Trig' }

def toolSV1Tag_Trig(name, useBTagFlagsDefaults = True, **options):
    """Sets up a SV1Tag_Trig tool and returns it.

    The following options have BTaggingFlags defaults:

    OutputLevel                         default: BTaggingFlags.OutputLevel
    Runmodus                            default: BTaggingFlags.Runmodus
    referenceType                       default: BTaggingFlags.ReferenceType
    SVAlgType                           default: "SV1"
    jetCollectionList                   default: BTaggingFlags.Jets
    SecVxFinderName                     default: "SV1"
    LikelihoodTool                      default: None
    UseCHypo                            default: True

    input:             name: The name of the tool (should be unique).
      useBTagFlagsDefaults : Whether to use BTaggingFlags defaults for options that are not specified.
                  **options: Python dictionary with options for the tool.
    output: The actual tool, which can then by added to ToolSvc via ToolSvc += output."""
    if useBTagFlagsDefaults:
        defaults = { 'OutputLevel'                      : BTaggingFlags.OutputLevel,
                     'Runmodus'                         : BTaggingFlags.Runmodus,
                     'referenceType'                    : BTaggingFlags.ReferenceType,
                     'SVAlgType'                        : 'SV1',
                     'jetCollectionList'                : BTaggingFlags.Jets,
                     'SecVxFinderName'                  : 'SV1',
                     'LikelihoodTool'                   : None,
                     'UseCHypo'                         : True,
                     }
        for option in defaults:
            options.setdefault(option, defaults[option])
    options['name'] = name
    from JetTagTools.JetTagToolsConf import Analysis__SVTag
    return Analysis__SVTag(**options)

#---------------------------------------------------------------------

metaSV1NewLikelihoodTool_Trig = { 'CalibrationTaggers' : ['SV1',],
                                 'ToolCollection'     : 'SV1Tag_Trig' }

def toolSV1NewLikelihoodTool_Trig(name, useBTagFlagsDefaults = True, **options):
    """Sets up a SV1NewLikelihoodTool_Trig tool and returns it.

    The following options have BTaggingFlags defaults:

    OutputLevel                         default: BTaggingFlags.OutputLevel
    taggerName                          default: "SV1"

    input:             name: The name of the tool (should be unique).
      useBTagFlagsDefaults : Whether to use BTaggingFlags defaults for options that are not specified.
                  **options: Python dictionary with options for the tool.
    output: The actual tool, which can then by added to ToolSvc via ToolSvc += output."""
    if useBTagFlagsDefaults:
        defaults = { 'OutputLevel'                         : BTaggingFlags.OutputLevel,
                     'HistosKey'                           : 'TrigJetTagCalibHistosKey',
                     'taggerName'                          : 'SV1' }
        for option in defaults:
            options.setdefault(option, defaults[option])
    options['name'] = name
    from JetTagTools.JetTagToolsConf import Analysis__NewLikelihoodTool
    return Analysis__NewLikelihoodTool(**options)
