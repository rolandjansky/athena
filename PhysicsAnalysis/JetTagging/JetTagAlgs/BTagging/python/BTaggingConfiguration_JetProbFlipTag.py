# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# Configuration functions for JetProbFlipTag
# Author: Wouter van den Wollenberg (2013-2014)
# NOTE: This tagger depends quite a bit on the tools being set up by JetProbTag.
from BTagging.BTaggingFlags import BTaggingFlags

metaJetProbFlipTag = { 'IsATagger'          : True,
                       'DependsOn'          : ['AtlasExtrapolator',
                                               'BTagTrackToVertexTool',
                                               'InDetVKalVxInJetTool',
                                               'BTagFullLinearizedTrackFactory',
                                               'BTagTrackToVertexIPEstimator',
                                               'JetProbBasicTrackGradeFactory',
                                               'SVForIPTool_JetProb',
                                               'JetProbTrackSelector',
                                               'BTagCalibrationBrokerTool',
                                               'JetProbTag'],
                       'PassByPointer'      : {'SVForIPTool'              : 'SVForIPTool_JetProb',
                                               'trackSelectorTool'        : 'JetProbTrackSelector',
                                               'trackGradeFactory'        : 'JetProbBasicTrackGradeFactory',
                                               'trackToVertexTool'        : 'BTagTrackToVertexTool',
                                               'TrackToVertexIPEstimator' : 'BTagTrackToVertexIPEstimator',
                                               'calibrationTool'          : 'BTagCalibrationBrokerTool'},
                       'PassByName'         : {'SecVxFinderNameForV0Removal' : 'InDetVKalVxInJetTool',
                                               'SecVxFinderNameForIPSign'    : 'InDetVKalVxInJetTool'},
                       'PassTracksAs'       : 'trackAssociation',
                       'CalibrationFolders' : ['JetProb',],
                       'JetCollectionList'  : 'jetCollectionList',
                       'JetWithInfoPlus'    : 'jetWithInfoPlus',
                       'ToolCollection'     : 'JetProbFlipTag' }

def toolJetProbFlipTag(name, useBTagFlagsDefaults = True, **options):
    """Sets up a JetProbFlipTag tool and returns it.

    The following options have BTaggingFlags defaults:

    OutputLevel                         default: BTaggingFlags.OutputLevel
    Runmodus                            default: BTaggingFlags.Runmodus
    impactParameterView                 default: "2D"
    trackGradePartitions                default: [ "Good", "Shared" ]
    UsePosIP                            default: False
    UseNegIP                            default: True
    flipIPSign                          default: True
    RejectBadTracks                     default: False
    SecVxFinderNameForV0Removal         default: "InDetVKalVxInJetTool"
    SecVxFinderNameForIPSign            default: "InDetVKalVxInJetTool"
    originalTPCollectionName            default: BTaggingFlags.TrackParticleCollectionName
    jetCollectionList                   default: BTaggingFlags.Jets
    jetWithInfoPlus                     default: BTaggingFlags.JetsWithInfoPlus
    unbiasIPEstimation                  default: False (switch to true (better!) when creating new PDFs)
    InputType                           default: globalflags.DataSource()
    trackAssociation                    default: "Tracks"

    input:             name: The name of the tool (should be unique).
      useBTagFlagsDefaults : Whether to use BTaggingFlags defaults for options that are not specified.
                  **options: Python dictionary with options for the tool.
    output: The actual tool, which can then by added to ToolSvc via ToolSvc += output."""
    if useBTagFlagsDefaults:
        from AthenaCommon.GlobalFlags import globalflags
        defaults = { 'OutputLevel'                      : BTaggingFlags.OutputLevel,
                     'Runmodus'                         : BTaggingFlags.Runmodus,
                     'impactParameterView'              : '2D',
                     'trackGradePartitions'             : [ "Good", "Shared" ],
                     'UsePosIP'                         : False,
                     'UseNegIP'                         : True,
                     'flipIPSign'                       : True,
                     'RejectBadTracks'                  : False,
                     'SecVxFinderNameForV0Removal'      : 'InDetVKalVxInJetTool',
                     'SecVxFinderNameForIPSign'         : 'InDetVKalVxInJetTool',
                     'originalTPCollectionName'         : BTaggingFlags.TrackParticleCollectionName,
                     'jetCollectionList'                : BTaggingFlags.Jets,
                     'jetWithInfoPlus'                  : BTaggingFlags.JetsWithInfoPlus,
                     'unbiasIPEstimation'               : False,
                     'InputType'                        : globalflags.DataSource(),
                     'trackAssociation'                 : "Tracks" }
        for option in defaults:
            options.setdefault(option, defaults[option])
    options['name'] = name
    from JetTagTools.JetTagToolsConf import Analysis__JetProbTag
    return Analysis__JetProbTag(**options)
