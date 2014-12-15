# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# Configuration functions for NewJetFitterTag
# Author: Wouter van den Wollenberg (2013-2014)
# Containing the modified JetFitter tools required for JetFitterCOMBNNIP3DNeg
# Depends heavily on NewJetFitterCollection
from BTagging.BTaggingFlags import BTaggingFlags

metaJetFitterTagCOMBNNIP3DNeg = { 'IsATagger'         : True,
                                  'DependsOn'         : ['AtlasExtrapolator',
                                                         'BTagTrackToVertexTool',
                                                         'JetFitterTagNNFlip',
                                                         'IP3DNegTag',
                                                         'InDetVKalVxInJetTool',
                                                         'BTagFullLinearizedTrackFactory',
                                                         'BTagTrackToVertexIPEstimator',
                                                         'JetFitterCOMBNNTool',
                                                         'NeuralNetworkToHistoToolCOMBNN',
                                                         'JetFitterNtupleWriterCOMBNN'],
                                  'PassByPointer'     : { 'jetfitterClassifier'       : 'JetFitterCOMBNNTool',
                                                          'jetfitterNtupleWriter'     : 'JetFitterNtupleWriterCOMBNN'},
                                  'JetCollectionList' : 'jetCollectionList',
                                  'ToolCollection'    : 'JetFitterIP3DNegCollection' }

def toolJetFitterTagCOMBNNIP3DNeg(name, useBTagFlagsDefaults = True, **options):
    """Sets up a JetFitterTagCOMBNNIP3D tool and returns it.

    The following options have BTaggingFlags defaults:

    OutputLevel                         default: BTaggingFlags.OutputLevel
    Runmodus                            default: BTaggingFlags.Runmodus
    jetCollectionList                   default: BTaggingFlags.Jets
    SecVxFinderName                     default: "JetFitter"
    useForcedCalibration                default: False
    xAODBaseName                        default: "JetFitterCombNNIP3DNeg"
    ipinfoTaggerName                    default: "IP3DNeg"

    input:             name: The name of the tool (should be unique).
      useBTagFlagsDefaults : Whether to use BTaggingFlags defaults for options that are not specified.
                  **options: Python dictionary with options for the tool.
    output: The actual tool, which can then by added to ToolSvc via ToolSvc += output."""
    if useBTagFlagsDefaults:
        defaults = { 'OutputLevel'                      : BTaggingFlags.OutputLevel,
                     'Runmodus'                         : BTaggingFlags.Runmodus,
                     'jetCollectionList'                : BTaggingFlags.Jets,
                     'SecVxFinderName'                  : 'JetFitter',
                     'useForcedCalibration'             : False,
                     'xAODBaseName'                     : 'JetFitterCombNNIP3D',
                     'ipinfoTaggerName'                 : 'IP3DNeg',
                     }
        for option in defaults:
            options.setdefault(option, defaults[option])
    options['name'] = name
    from JetTagTools.JetTagToolsConf import Analysis__JetFitterTag
    return Analysis__JetFitterTag(**options)

#------------------------------------------------------------------------------------

metaJetFitterTagNNFlip = { 'IsATagger'         : True,
                           'DependsOn'         : ['AtlasExtrapolator',
                                                  'BTagTrackToVertexTool',
                                                  'NewJetFitterVxFinderFlip',
                                                  'JetFitterNNTool',
                                                  'NeuralNetworkToHistoToolNN',
                                                  'JetFitterNtupleWriterNN'],
                           'PassByPointer'     : { 'jetfitterClassifier'       : 'JetFitterNNTool',
                                                   'jetfitterNtupleWriter'     : 'JetFitterNtupleWriterNN' },
                           'JetCollectionList' : 'jetCollectionList',
                           'ToolCollection'    : 'JetFitterIP3DNegCollection' }

def toolJetFitterTagNNFlip(name, useBTagFlagsDefaults = True, **options):
    """Sets up a JetFitterTagNNFlip tool and returns it.

    The following options have BTaggingFlags defaults:

    OutputLevel                         default: BTaggingFlags.OutputLevel
    Runmodus                            default: BTaggingFlags.Runmodus
    jetCollectionList                   default: BTaggingFlags.Jets
    SecVxFinderName                     default: "JetFitterFlip"
    useForcedCalibration                default: False
    ipinfoTaggerName                    default: ""
    xAODBaseName                        default: "JetFitterFlip"

    input:             name: The name of the tool (should be unique).
      useBTagFlagsDefaults : Whether to use BTaggingFlags defaults for options that are not specified.
                  **options: Python dictionary with options for the tool.
    output: The actual tool, which can then by added to ToolSvc via ToolSvc += output."""
    if useBTagFlagsDefaults:
        defaults = { 'OutputLevel'                      : BTaggingFlags.OutputLevel,
                     'Runmodus'                         : BTaggingFlags.Runmodus,
                     'jetCollectionList'                : BTaggingFlags.Jets,
                     'SecVxFinderName'                  : 'JetFitterFlip',
                     'useForcedCalibration'             : False,
                     'ipinfoTaggerName'                 : "",
                     'xAODBaseName'                     : 'JetFitterFlip', }
        for option in defaults:
            options.setdefault(option, defaults[option])
    options['name'] = name
    from JetTagTools.JetTagToolsConf import Analysis__JetFitterTag
    return Analysis__JetFitterTag(**options)

#------------------------------------------------------------------------------------

metaNewJetFitterVxFinderFlip = { 'IsAVertexFinder' : True,
                                 'VertexFinderxAODBaseName' : 'JetFitterFlip',
                                 'VertexFinderPassByPointer': {'JetFitterVariableFactory' : 'JetFitterVariablesFactory' },
                                 'DependsOn'       : ['BTagTrackToVertexTool',
                                                      'InDetJetFitterUtils',
                                                      'ImprovedJetFitterRoutines',
                                                      'TrkDistanceFinderNeutralCharged',
                                                      'TrkDistanceFinderNeutralNeutral',
                                                      'ImprovedJetFitterInitializationHelper',
                                                      'JetFitterSequentialVertexFitter',
                                                      'JetFitterSequentialVertexSmoother',
                                                      'JetFitterMode3dTo1dFinder',
                                                      'InDetImprovedJetFitterTrackSelectorTool',
                                                      'JetFitterExtrapolator',
                                                      'JetFitterFullLinearizedTrackFactory',
                                                      'JetFitterVariablesFactory',
                                                      'VxInternalEdmFactory'],
                                 'PassByPointer'    : { 'Mode3dFinder'                  : 'JetFitterMode3dTo1dFinder',
                                                        'JetFitterInitializationHelper' : 'ImprovedJetFitterInitializationHelper',
                                                        'TrackSelector'                 : 'InDetImprovedJetFitterTrackSelectorTool',
                                                        'InDetJetFitterUtils'           : 'InDetJetFitterUtils',
                                                        'Extrapolator'                  : 'JetFitterExtrapolator',
                                                        'SequentialVertexFitter'        : 'JetFitterSequentialVertexFitter',
                                                        'JetFitterRoutines'             : 'ImprovedJetFitterRoutines',
                                                        'VertexEdmFactory'              : 'VxInternalEdmFactory' },
                                 'ToolCollection'   : 'JetFitterIP3DNewCollection' }

def toolNewJetFitterVxFinderFlip(name, useBTagFlagsDefaults = True, **options):
    """Sets up a NewJetFitterVxFinderFlip tool and returns it.

    The following options have BTaggingFlags defaults:

    OutputLevel                         default: BTaggingFlags.OutputLevel
    VxPrimaryContainer                  default: BTaggingFlags.PrimaryVertexCollectionName
    MaxNumDeleteIterations              default: 30
    VertexProbCut                       default: 0.001
    MaxClusteringIterations             default: 30
    VertexClusteringProbabilityCut      default: 0.005
    revertFromPositiveToNegativeTags    default: True

    input:             name: The name of the tool (should be unique).
      useBTagFlagsDefaults : Whether to use BTaggingFlags defaults for options that are not specified.
                  **options: Python dictionary with options for the tool.
    output: The actual tool, which can then by added to ToolSvc via ToolSvc += output."""
    if useBTagFlagsDefaults:
        if not 'InDetKeys' in dir():
            from InDetRecExample.InDetKeys import InDetKeys
        defaults = { 'OutputLevel'                         : BTaggingFlags.OutputLevel,
                     'VxPrimaryContainer'                  : BTaggingFlags.PrimaryVertexCollectionName,
                     'MaxNumDeleteIterations'              : 30,
                     'VertexProbCut'                       : 0.001,
                     'MaxClusteringIterations'             : 30,
                     'VertexClusteringProbabilityCut'      : 0.005,
                     'revertFromPositiveToNegativeTags'    : True }
        for option in defaults:
            options.setdefault(option, defaults[option])
    options['name'] = name
    from InDetSecVxFinderTool.InDetSecVxFinderToolConf import InDet__InDetImprovedJetFitterVxFinder
    return InDet__InDetImprovedJetFitterVxFinder(**options)

