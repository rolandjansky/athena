# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# Configuration functions for NewJetFitterTag
# Author: Wouter van den Wollenberg (2013-2014)
# Because there are some many different JetFitter tools floating around
# now, this file was created to contain them all.
from BTagging.BTaggingFlags import BTaggingFlags

###
#### VD: I deleted the extrapolator since one is enough

###
#### VD: I deleted the track selector since one is enough

###
#### VD: I deleted the Linearised trackfactory since one is enough

###
#### VD: I deleted the Mode3dto1 since one is enough

###
#### VD: I deleted the SequentialVertexSmoother since one is enough

###
#### VD: I deleted the InternalEdmFactory since one is enough

###
#### VD: I deleted the SequentialVertexFitter since one is enough

###
#### VD: I deleted the InitialisationHelper since one is enough

###
#### VD: I deleted the TrkDistanceFinderNeutralNutral since one is enough

###
#### VD: I deleted the TrkDistanceFinderNeutralCharged since one is enough

###
#### VD: I deleted the ImprovedFitterRoutine since one is enough

###
#### VD: I deleted the InDetJetFitterUtils since one is enough

#--------------------------------------------------------------------------
metaNewJetFitterVxFinderFlip = { 'IsAVertexFinder' : True,
                                 'VertexFinderxAODBaseName' : 'JetFitterFlip',
                                 #'VertexFinderPassByPointer': {'JetFitterVariableFactory' : 'JFVarFactoryFlip' },
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
                                                  #'JFVarFactoryFlip',
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
                             'ToolCollection'   : 'JetFitterIP3DNegCollection' }

def toolNewJetFitterVxFinderFlip(name, useBTagFlagsDefaults = True, **options):
    """Sets up a NewJetFitterVxFinder tool with Flip enabled!! and returns it.

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


#--------------------------------------------------------------------------
#metaJFVarFactoryFlip = { 'ToolCollection' : 'JetFitterIP3DNegCollection' }

#def toolJFVarFactoryFlip(name, useBTagFlagsDefaults = True, **options):
#    """Sets up a NewJetFitterVariablesFactory tool and returns it.
#
#    The following options have BTaggingFlags defaults:
#
#    OutputLevel                         default: BTaggingFlags.OutputLevel
#    JetFitterInstance                   default: "JetFitterTag"
#    secVxFinderName                     default: "JetFitterVxFinder"
#
#    input:             name: The name of the tool (should be unique).
#      useBTagFlagsDefaults : Whether to use BTaggingFlags defaults for options that are not specified.
#                  **options: Python dictionary with options for the tool.
#    output: The actual tool, which can then by added to ToolSvc via ToolSvc += output."""
#    if useBTagFlagsDefaults:
#        defaults = { 'OutputLevel'                         : BTaggingFlags.OutputLevel,
#                     'JetFitterInstance'                   : 'JetFitterTagFlip',
#                     'secVxFinderName'                     : 'JetFitterFlip',
#                     'revertFromPositiveToNegativeTags'    : True }
#        for option in defaults:
#            options.setdefault(option, defaults[option])
#    options['name'] = name
#    from JetTagTools.JetTagToolsConf import Analysis__JetFitterVariablesFactory
#    return Analysis__JetFitterVariablesFactory(**options)


###
#### VD: I deleted the NtupleWriter since one is enough

###
#### VD: I deleted the NNtoHistoTool since one is enough

###
#### VD: I deleted the NNTool since one is enough

#-------------------------------------------------------------------------

metaJetFitterTagNNFlip = { 'IsATagger'         : True,
                           'xAODBaseName'      : 'JetFitterFlip',
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
    """Sets up a JetFitterTagNN tool and returns it.

    The following options have BTaggingFlags defaults:

    OutputLevel                         default: BTaggingFlags.OutputLevel
    Runmodus                            default: BTaggingFlags.Runmodus
    jetCollectionList                   default: BTaggingFlags.Jets
    SecVxFinderName                     default: "JetFitter"
    useForcedCalibration                default: False
    ipinfoTaggerName                    default: ""

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
                     }
        for option in defaults:
            options.setdefault(option, defaults[option])
    options['name'] = name
    from JetTagTools.JetTagToolsConf import Analysis__JetFitterTag
    return Analysis__JetFitterTag(**options)

#-------------------------------------------------------------------------

###
#### VD: I deleted the JFNtupleWriterCOMBNN since one is enough

###
#### VD: I deleted the NNtoHistoCOMBNN since one is enough

###
#### VD: I deleted the JetFitterCombNNTool since it's not used

#-----------------------------------------------------------------------------

metaJetFitterTagCOMBNNIP3DNeg = { 'IsATagger'         : True,
                                  'xAODBaseName'      : 'TagCOMBNNIP3DNeg',
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
    """Sets up a JetFitterTagCOMBNN tool and returns it.

    The following options have BTaggingFlags defaults:

    OutputLevel                         default: BTaggingFlags.OutputLevel
    Runmodus                            default: BTaggingFlags.Runmodus
    jetCollectionList                   default: BTaggingFlags.Jets
    SecVxFinderName                     default: "JetFitter"
    useForcedCalibration                default: False
    ipinfoTaggerName                    default: "IP3D"

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
                     'ipinfoTaggerName'                 : 'IP3DNeg',
                     }
        for option in defaults:
            options.setdefault(option, defaults[option])
    options['name'] = name
    from JetTagTools.JetTagToolsConf import Analysis__JetFitterTag
    return Analysis__JetFitterTag(**options)

###
#### VD: I deleted the JetFitterTagCOMBNN since it's not used

