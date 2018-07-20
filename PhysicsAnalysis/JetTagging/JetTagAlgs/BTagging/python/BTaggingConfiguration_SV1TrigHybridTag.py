# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# Configuration functions for SV1TrigHybridTag
# 0;115;0c Author: Wouter van den Wollenberg (2013-2014)
from BTagging.BTaggingFlags import BTaggingFlags

metaSV1TrigHybridTag = {'IsATagger': True,
                        'xAODBaseName': 'SV1TrigHybrid',
                        'DependsOn': ['AtlasExtrapolator',
                                      'BTagTrackToVertexTool',
                                      'InDetVKalVxInJetTool',
                                      'SV1TrigHybridNewLikelihoodTool'],
                        'PassByPointer': {'LikelihoodTool': 'SV1TrigHybridNewLikelihoodTool'},
                        #               'PassByName'        : {'SecVxFinderName' : 'InDetVKalVxInJetTool' },
                        'JetCollectionList': 'jetCollectionList',
                        'ToolCollection': 'SV1TrigHybridTag'}


def toolSV1TrigHybridTag(name, useBTagFlagsDefaults=True, **options):
    """Sets up a SV1TrigHybridTag tool and returns it.

    The following options have BTaggingFlags defaults:

    OutputLevel                         default: BTaggingFlags.OutputLevel
    Runmodus                            default: BTaggingFlags.Runmodus
    referenceType                       default: BTaggingFlags.ReferenceType
    SVAlgType                           default: "SV1"
    jetCollectionList                   default: BTaggingFlags.Jets
    LikelihoodTool                      default: None
    SecVxFinderName                     default: "SV1"
    UseCHypo                            default: True

    input:             name: The name of the tool (should be unique).
      useBTagFlagsDefaults : Whether to use BTaggingFlags defaults for options that are not specified.
                  **options: Python dictionary with options for the tool.
    output: The actual tool, which can then by added to ToolSvc via ToolSvc += output."""
    if useBTagFlagsDefaults:
        defaults = {'OutputLevel': BTaggingFlags.OutputLevel,
                    'Runmodus': BTaggingFlags.Runmodus,
                    'referenceType': BTaggingFlags.ReferenceType,
                    'jetPtMinRef': BTaggingFlags.JetPtMinRef,
                    'SVAlgType': 'SV1',
                    'jetCollectionList': BTaggingFlags.Jets,
                    'SecVxFinderName': 'SV1',
                    'UseCHypo': True,
                    #                     'originalTPCollectionName'         : BTaggingFlags.TrackParticleCollectionName,
                    'LikelihoodTool': None}
        for option in defaults:
            options.setdefault(option, defaults[option])
    options['name'] = name
    from JetTagTools.JetTagToolsConf import Analysis__SVTag
    return Analysis__SVTag(**options)

#-----------------------------------------------------------------


metaSV1TrigHybridNewLikelihoodTool = {'CalibrationFolders': ['SV1TrigHybrid', ],
                                      'DependsOn': ['BTagCalibrationBrokerTool', ],
                                      'PassByPointer': {'calibrationTool': 'BTagCalibrationBrokerTool'},
                                      'ToolCollection': 'SV1TrigHybridTag'}


def toolSV1TrigHybridNewLikelihoodTool(name, useBTagFlagsDefaults=True, **options):
    """Sets up a SV1TrigHybridNewLikelihoodTool tool and returns it.

    The following options have BTaggingFlags defaults:

    OutputLevel                         default: BTaggingFlags.OutputLevel
    taggerName                          default: "SV1"

    input:             name: The name of the tool (should be unique).
      useBTagFlagsDefaults : Whether to use BTaggingFlags defaults for options that are not specified.
                  **options: Python dictionary with options for the tool.
    output: The actual tool, which can then by added to ToolSvc via ToolSvc += output."""
    if useBTagFlagsDefaults:
        defaults = {'OutputLevel': BTaggingFlags.OutputLevel,
                    'taggerName': 'SV1TrigHybrid'}
        for option in defaults:
            options.setdefault(option, defaults[option])
    options['name'] = name
    from JetTagTools.JetTagToolsConf import Analysis__NewLikelihoodTool
    return Analysis__NewLikelihoodTool(**options)
