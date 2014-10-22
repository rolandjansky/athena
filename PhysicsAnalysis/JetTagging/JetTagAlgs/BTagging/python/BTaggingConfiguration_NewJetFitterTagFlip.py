# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# Configuration functions for NewJetFitterTagFlip
# Author: Wouter van den Wollenberg (2013-2014)
from BTagging.BTaggingFlags import BTaggingFlags

metaNewJetFitterTagFlip = { 'IsATagger'         : True,
                            'DependsOn'         : ['AtlasExtrapolator',
                                                   'BTagTrackToVertexTool',
                                                   'NewJetFitterVxFinderFlip',
                                                   'NewJetFitterVariablesFactoryFlip',
                                                   'NewJetFitterNtupleWriter',
                                                   'NewJetFitterLikelihood'],
                            'PassByPointer'     : {'JetFitterVariablesFactory' : 'NewJetFitterVariablesFactoryFlip',
                                                   'JetFitterLikelihood'       : 'NewJetFitterLikelihood',
                                                   'JetFitterNtupleWriter'     : 'NewJetFitterNtupleWriter'},
                            'PassByName'        : {'SecVxFinderName' : 'NewJetFitterVxFinder'},
                            'JetCollectionList' : 'jetCollectionList',
                            'ToolCollection'    : 'NewJetFitterTagFlip' }

def toolNewJetFitterTagFlip(name, useBTagFlagsDefaults = True, **options):
    """Sets up a NewJetFitterTagFlip tool and returns it.

    The following options have BTaggingFlags defaults:

    OutputLevel                         default: BTaggingFlags.OutputLevel
    Runmodus                            default: BTaggingFlags.Runmodus
    jetCollectionList                   default: BTaggingFlags.Jets
    SecVxFinderName                     default: "NewJetFitterVxFinderFlip"
    useForcedCalibration                default: False

    input:             name: The name of the tool (should be unique).
      useBTagFlagsDefaults : Whether to use BTaggingFlags defaults for options that are not specified.
                  **options: Python dictionary with options for the tool.
    output: The actual tool, which can then by added to ToolSvc via ToolSvc += output."""
    if useBTagFlagsDefaults:
        defaults = { 'OutputLevel'                      : BTaggingFlags.OutputLevel,
                     'Runmodus'                         : BTaggingFlags.Runmodus,
                     'jetCollectionList'                : BTaggingFlags.Jets,
                     'SecVxFinderName'                  : 'NewJetFitterVxFinderFlip',
                     'useForcedCalibration'             : False }
        for option in defaults:
            options.setdefault(option, defaults[option])
    options['name'] = name
    from JetTagTools.JetTagToolsConf import Analysis__JetFitterTag
    return Analysis__JetFitterTag(**options)

#------------------------------------------------------------------------------

metaNewJetFitterVariablesFactoryFlip = { 'PassByName'     : {'JetFitterInstance' : 'JetFitterTagFlip'},
                                         'ToolCollection' : 'NewJetFitterTagFlip' }

def toolNewJetFitterVariablesFactoryFlip(name, useBTagFlagsDefaults = True, **options):
    """Sets up a NewJetFitterVariablesFactoryFlip tool and returns it.

    The following options have BTaggingFlags defaults:

    OutputLevel                         default: BTaggingFlags.OutputLevel
    JetFitterInstance                   default: "JetFitterTagFlip"
    usePtCorrectedMass                  default: True
    revertFromPositiveToNegativeTags    default: True

    input:             name: The name of the tool (should be unique).
      useBTagFlagsDefaults : Whether to use BTaggingFlags defaults for options that are not specified.
                  **options: Python dictionary with options for the tool.
    output: The actual tool, which can then by added to ToolSvc via ToolSvc += output."""
    if useBTagFlagsDefaults:
        defaults = { 'OutputLevel'                      : BTaggingFlags.OutputLevel,
                     'JetFitterInstance'                : 'JetFitterTagFlip',
                     'usePtCorrectedMass'               : True,
                     'revertFromPositiveToNegativeTags' : True }
        for option in defaults:
            options.setdefault(option, defaults[option])
    options['name'] = name
    from JetTagTools.JetTagToolsConf import Analysis__JetFitterVariablesFactory
    return Analysis__JetFitterVariablesFactory(**options)

#------------------------------------------------------------------------------

metaNewJetFitterVxFinderFlip = { 'IsAVertexFinder' : True,
                                 'DependsOn'       : ['AtlasExtrapolator',
                                                      'BTagTrackToVertexTool',
                                                      'InDetJetFitterUtils',
                                                      'ImprovedJetFitterRoutines',
                                                      'TrkDistanceFinderNeutralCharged',
                                                      'TrkDistanceFinderNeutralNeutral',
                                                      'ImprovedJetFitterInitializationHelper',
                                                      'JetFitterSequentialVertexFitter',
                                                      'JetFitterSequentialVertexSmoother',
                                                      'JetFitterMode3dTo1dFinder',
                                                      'JetFitterFullLinearizedTrackFactory',
                                                      'InDetImprovedJetFitterTrackSelectorTool',
                                                      'JetFitterExtrapolator'],
                                 'PassByPointer'   : { 'Mode3dFinder'                  : 'JetFitterMode3dTo1dFinder',
                                                       'JetFitterInitializationHelper' : 'ImprovedJetFitterInitializationHelper',
                                                       'TrackSelector'                 : 'InDetImprovedJetFitterTrackSelectorTool',
                                                       'InDetJetFitterUtils'           : 'InDetJetFitterUtils',
                                                       'Extrapolator'                  : 'JetFitterExtrapolator',
                                                       'SequentialVertexFitter'        : 'JetFitterSequentialVertexFitter',
                                                       'JetFitterRoutines'             : 'ImprovedJetFitterRoutines' },
                                 'ToolCollection'  : 'NewJetFitterTagFlip' }

def toolNewJetFitterVxFinderFlip(name, useBTagFlagsDefaults = True, **options):
    """Sets up a NewJetFitterVxFinderFlip tool and returns it.

    The following options have BTaggingFlags defaults:

    OutputLevel                         default: BTaggingFlags.OutputLevel
    VxPrimaryContainer                  default: InDetKeys.PrimaryVertices()
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
                     'VxPrimaryContainer'                  : InDetKeys.PrimaryVertices(),
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
