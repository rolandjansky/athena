# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# Configuration functions for NewJetFitterTag
# Author: Wouter van den Wollenberg (2013-2014)
from BTagging.BTaggingFlags import BTaggingFlags

metaNewJetFitterTag = { 'IsATagger'         : True,
                        'DependsOn'         : ['AtlasExtrapolator',
                                               'BTagTrackToVertexTool',
                                               'NewJetFitterVxFinder',
                                               'NewJetFitterLikelihood',
                                               'NewJetFitterVariablesFactory',
                                               'NewJetFitterNtupleWriter'],
                        'PassByPointer'     : {'JetFitterVariablesFactory' : 'NewJetFitterVariablesFactory',
                                               'JetFitterLikelihood'       : 'NewJetFitterLikelihood',
                                               'JetFitterNtupleWriter'     : 'NewJetFitterNtupleWriter'},
                        'PassByName'        : {'SecVxFinderName' : 'NewJetFitterVxFinder'},
                        'JetCollectionList' : 'jetCollectionList',
                        'ToolCollection'    : 'NewJetFitterTag' }

def toolNewJetFitterTag(name, useBTagFlagsDefaults = True, **options):
    """Sets up a NewJetFitterTag tool and returns it.

    The following options have BTaggingFlags defaults:

    OutputLevel                         default: BTaggingFlags.OutputLevel
    Runmodus                            default: BTaggingFlags.Runmodus
    jetCollectionList                   default: BTaggingFlags.Jets
    SecVxFinderName                     default: "NewJetFitterVxFinder"
    useForcedCalibration                default: False

    input:             name: The name of the tool (should be unique).
      useBTagFlagsDefaults : Whether to use BTaggingFlags defaults for options that are not specified.
                  **options: Python dictionary with options for the tool.
    output: The actual tool, which can then by added to ToolSvc via ToolSvc += output."""
    if useBTagFlagsDefaults:
        defaults = { 'OutputLevel'                      : BTaggingFlags.OutputLevel,
                     'Runmodus'                         : BTaggingFlags.Runmodus,
                     'jetCollectionList'                : BTaggingFlags.Jets,
                     'SecVxFinderName'                  : 'NewJetFitterVxFinder',
                     'useForcedCalibration'             : False }
        for option in defaults:
            options.setdefault(option, defaults[option])
    options['name'] = name
    from JetTagTools.JetTagToolsConf import Analysis__JetFitterTag
    return Analysis__JetFitterTag(**options)

#-----------------------------------------------------------------------

metaJetFitterExtrapolator = {'OneInTotal'     : True,
                             'ToolCollection' : 'NewJetFitterTag' }

def toolJetFitterExtrapolator(name, useBTagFlagsDefaults = True, **options):
    """Sets up a JetFitterExtrapolator tool and returns it.

    input:             name: The name of the tool (should be unique).
      useBTagFlagsDefaults : Whether to use BTaggingFlags defaults for options that are not specified.
                  **options: Python dictionary with options for the tool.
    output: The actual tool, which can then by added to ToolSvc via ToolSvc += output."""
    from TrkExTools.AtlasExtrapolator import AtlasExtrapolator
    return AtlasExtrapolator(**options)

#-----------------------------------------------------------------------

metaJetFitterFullLinearizedTrackFactory = {'DependsOn'     : ['JetFitterExtrapolator',],
                                           'PassByPointer' : {'Extrapolator' : 'JetFitterExtrapolator' },
                                           'ToolCollection': 'NewJetFitterTag' }

def toolJetFitterFullLinearizedTrackFactory(name, useBTagFlagsDefaults = True, **options):
    """Sets up a JetFitterFullLinearizedTrackFactory tool and returns it.

    input:             name: The name of the tool (should be unique).
      useBTagFlagsDefaults : Whether to use BTaggingFlags defaults for options that are not specified.
                             Note however that this tool has no BTaggingFlags defaults; the option is
                             here only for consistency.
                  **options: Python dictionary with options for the tool.
    output: The actual tool, which can then by added to ToolSvc via ToolSvc += output."""
    options['name'] = name
    from TrkVertexFitterUtils.TrkVertexFitterUtilsConf import Trk__FullLinearizedTrackFactory
    return Trk__FullLinearizedTrackFactory(**options)

#-----------------------------------------------------------------------

metaVxInternalEdmFactory = {'DependsOn'     : ['JetFitterFullLinearizedTrackFactory',],
                            'PassByPointer' : {'LinearizedTrackFactory' : 'JetFitterFullLinearizedTrackFactory' },
                            'ToolCollection': 'NewJetFitterTag' }

def toolVxInternalEdmFactory(name, useBTagFlagsDefaults = True, **options):
    """Sets up a VxInternalEdmFactory tool and returns it.
    
    The following options have BTaggingFlags defaults:

    OutputLevel                         default: BTaggingFlags.OutputLevel

    input:             name: The name of the tool (should be unique).
      useBTagFlagsDefaults : Whether to use BTaggingFlags defaults for options that are not specified.
                  **options: Python dictionary with options for the tool.
    output: The actual tool, which can then by added to ToolSvc via ToolSvc += output."""
    if useBTagFlagsDefaults:
        defaults = { 'OutputLevel'             : BTaggingFlags.OutputLevel, }
        for option in defaults:
            options.setdefault(option, defaults[option])
    options['name'] = name
    from TrkVxEdmCnv.TrkVxEdmCnvConf import Trk__VxCandidateXAODVertex
    return Trk__VxCandidateXAODVertex(**options)

#--------------------------------------------------------------------------

metaNewJetFitterLikelihood = { 'CalibrationFolders' : ['JetFitter',],
                               'DependsOn'          : ['BTagCalibrationBrokerTool',],
                               'PassByPointer'      : {'calibrationTool' : 'BTagCalibrationBrokerTool'},
                               'ToolCollection'     : 'NewJetFitterTag' }

def toolNewJetFitterLikelihood(name, useBTagFlagsDefaults = True, **options):
    """Sets up a NewJetFitterLikelihood tool and returns it.

    The following options have BTaggingFlags defaults:

    OutputLevel                         default: BTaggingFlags.OutputLevel
    CalibrationDirectory                default: "JetFitter"
    CalibrationSubDirectory             default: "RooFitLikelihood"

    input:             name: The name of the tool (should be unique).
      useBTagFlagsDefaults : Whether to use BTaggingFlags defaults for options that are not specified.
                  **options: Python dictionary with options for the tool.
    output: The actual tool, which can then by added to ToolSvc via ToolSvc += output."""
    if useBTagFlagsDefaults:
        defaults = { 'OutputLevel'             : BTaggingFlags.OutputLevel,
                     'CalibrationDirectory'    : 'JetFitter',
                     'CalibrationSubDirectory' : 'RooFitLikelihood' }
        for option in defaults:
            options.setdefault(option, defaults[option])
    options['name'] = name
    from JetTagTools.JetTagToolsConf import Analysis__JetFitterLikelihood
    return Analysis__JetFitterLikelihood(**options)

#--------------------------------------------------------------------------

metaNewJetFitterVariablesFactory = { 'PassByName'     : {'JetFitterInstance' : 'NewJetFitterTag',
                                                         'secVxFinderName'   : 'NewJetFitterVxFinder' },
                                     'ToolCollection' : 'NewJetFitterTag' }

def toolNewJetFitterVariablesFactory(name, useBTagFlagsDefaults = True, **options):
    """Sets up a NewJetFitterVariablesFactory tool and returns it.

    The following options have BTaggingFlags defaults:

    OutputLevel                         default: BTaggingFlags.OutputLevel
    JetFitterInstance                   default: "JetFitterTag"

    input:             name: The name of the tool (should be unique).
      useBTagFlagsDefaults : Whether to use BTaggingFlags defaults for options that are not specified.
                  **options: Python dictionary with options for the tool.
    output: The actual tool, which can then by added to ToolSvc via ToolSvc += output."""
    if useBTagFlagsDefaults:
        defaults = { 'OutputLevel'            : BTaggingFlags.OutputLevel,
                     'JetFitterInstance'      : 'JetFitterTag' }
#                     'usePtCorrectedMass'     : True }
        for option in defaults:
            options.setdefault(option, defaults[option])
    options['name'] = name
    from JetTagTools.JetTagToolsConf import Analysis__JetFitterVariablesFactory
    return Analysis__JetFitterVariablesFactory(**options)

#--------------------------------------------------------------------------

metaNewJetFitterNtupleWriter = { 'ToolCollection' : 'NewJetFitterTag' }

def toolNewJetFitterNtupleWriter(name, useBTagFlagsDefaults = True, **options):
    """Sets up a NewJetFitterNtupleWriter tool and returns it.

    The following options have BTaggingFlags defaults:

    OutputLevel                         default: BTaggingFlags.OutputLevel

    input:             name: The name of the tool (should be unique).
      useBTagFlagsDefaults : Whether to use BTaggingFlags defaults for options that are not specified.
                  **options: Python dictionary with options for the tool.
    output: The actual tool, which can then by added to ToolSvc via ToolSvc += output."""
    if useBTagFlagsDefaults:
        defaults = { 'OutputLevel'            : BTaggingFlags.OutputLevel }
        for option in defaults:
            options.setdefault(option, defaults[option])
    options['name'] = name
    from JetTagTools.JetTagToolsConf import Analysis__JetFitterNtupleWriter
    return Analysis__JetFitterNtupleWriter(**options)

#--------------------------------------------------------------------------

metaNewJetFitterVxFinder = { 'IsAVertexFinder' : True,
                             'VertexFinderxAODBaseName' : 'JetFitter',
                             'VertexFinderPassByPointer': {'JetFitterVariableFactory' : 'NewJetFitterVariablesFactory' },
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
                                                  'InDetImprovedJetFitterTrackSelectorTool',
                                                  'JetFitterExtrapolator',
                                                  'JetFitterFullLinearizedTrackFactory',
                                                  'NewJetFitterVariablesFactory',
                                                  'VxInternalEdmFactory'],
                             'PassByPointer'    : { 'Mode3dFinder'                  : 'JetFitterMode3dTo1dFinder',
                                                    'JetFitterInitializationHelper' : 'ImprovedJetFitterInitializationHelper',
                                                    'TrackSelector'                 : 'InDetImprovedJetFitterTrackSelectorTool',
                                                    'InDetJetFitterUtils'           : 'InDetJetFitterUtils',
                                                    'Extrapolator'                  : 'JetFitterExtrapolator',
                                                    'SequentialVertexFitter'        : 'JetFitterSequentialVertexFitter',
                                                    'JetFitterRoutines'             : 'ImprovedJetFitterRoutines',
                                                    'VertexEdmFactory'              : 'VxInternalEdmFactory' },
                             'ToolCollection'   : 'NewJetFitterTag' }

def toolNewJetFitterVxFinder(name, useBTagFlagsDefaults = True, **options):
    """Sets up a NewJetFitterVxFinder tool and returns it.

    The following options have BTaggingFlags defaults:

    OutputLevel                         default: BTaggingFlags.OutputLevel
    VxPrimaryContainer                  default: "PrimaryVertices"
    MaxNumDeleteIterations              default: 30
    VertexProbCut                       default: 0.001
    MaxClusteringIterations             default: 30
    VertexClusteringProbabilityCut      default: 0.005

    input:             name: The name of the tool (should be unique).
      useBTagFlagsDefaults : Whether to use BTaggingFlags defaults for options that are not specified.
                  **options: Python dictionary with options for the tool.
    output: The actual tool, which can then by added to ToolSvc via ToolSvc += output."""
    if useBTagFlagsDefaults:
        if not 'InDetKeys' in dir():
            from InDetRecExample.InDetKeys import InDetKeys
        defaults = { 'OutputLevel'                         : BTaggingFlags.OutputLevel,
                     'VxPrimaryContainer'                  : 'PrimaryVertices',
                     'MaxNumDeleteIterations'              : 30,
                     'VertexProbCut'                       : 0.001,
                     'MaxClusteringIterations'             : 30,
                     'VertexClusteringProbabilityCut'      : 0.005 }
        for option in defaults:
            options.setdefault(option, defaults[option])
    options['name'] = name
    from InDetSecVxFinderTool.InDetSecVxFinderToolConf import InDet__InDetImprovedJetFitterVxFinder
    return InDet__InDetImprovedJetFitterVxFinder(**options)

#--------------------------------------------------------------------------

metaInDetJetFitterUtils = { 'DependsOn'      : ['JetFitterFullLinearizedTrackFactory',
                                               'JetFitterExtrapolator'],
                            'PassByPointer'  : {'LinearizedTrackFactory' : 'JetFitterFullLinearizedTrackFactory',
                                               'Extrapolator'           : 'JetFitterExtrapolator' },
                            'ToolCollection' : 'NewJetFitterTag' }

def toolInDetJetFitterUtils(name, useBTagFlagsDefaults = True, **options):
    """Sets up a InDetJetFitterUtils tool and returns it.

    The following options have BTaggingFlags defaults:

    OutputLevel                         default: BTaggingFlags.OutputLevel

    input:             name: The name of the tool (should be unique).
      useBTagFlagsDefaults : Whether to use BTaggingFlags defaults for options that are not specified.
                  **options: Python dictionary with options for the tool.
    output: The actual tool, which can then by added to ToolSvc via ToolSvc += output."""
    if useBTagFlagsDefaults:
        defaults = { 'OutputLevel'          : BTaggingFlags.OutputLevel }
        for option in defaults:
            options.setdefault(option, defaults[option])
    options['name'] = name
    from InDetSecVxFinderTool.InDetSecVxFinderToolConf import InDet__InDetJetFitterUtils
    return InDet__InDetJetFitterUtils(**options)

#--------------------------------------------------------------------------

metaImprovedJetFitterRoutines = { 'DependsOn'      : ['ImprovedJetFitterInitializationHelper',
                                                      'TrkDistanceFinderNeutralNeutral',
                                                      'TrkDistanceFinderNeutralCharged'],
                                  'PassByPointer'  : {'JetFitterInitializationHelper'         : 'ImprovedJetFitterInitializationHelper',
                                                      'JetFitterMinimumDistanceFinderNeutral' : 'TrkDistanceFinderNeutralNeutral',
                                                      'JetFitterMinimumDistanceFinder'        : 'TrkDistanceFinderNeutralCharged' },
                                  'ToolCollection' : 'NewJetFitterTag' }

def toolImprovedJetFitterRoutines(name, useBTagFlagsDefaults = True, **options):
    """Sets up a ImprovedJetFitterRoutines tool and returns it.

    The following options have BTaggingFlags defaults:

    OutputLevel                         default: BTaggingFlags.OutputLevel
    BeFast                              default: False
    maxDRshift                          default: 0.0
    noPrimaryVertexRefit                default: False

    input:             name: The name of the tool (should be unique).
      useBTagFlagsDefaults : Whether to use BTaggingFlags defaults for options that are not specified.
                  **options: Python dictionary with options for the tool.
    output: The actual tool, which can then by added to ToolSvc via ToolSvc += output."""
    if useBTagFlagsDefaults:
        defaults = { 'OutputLevel'          : BTaggingFlags.OutputLevel,
                     'BeFast'               : False,
                     'maxDRshift'           : 0.0,
                     'noPrimaryVertexRefit' : False }
        for option in defaults:
            options.setdefault(option, defaults[option])
    options['name'] = name
    from TrkJetVxFitter.TrkJetVxFitterConf import Trk__JetFitterRoutines
    return Trk__JetFitterRoutines(**options)

#--------------------------------------------------------------------------

metaTrkDistanceFinderNeutralCharged = { 'ToolCollection' : 'NewJetFitterTag' }

def toolTrkDistanceFinderNeutralCharged(name, useBTagFlagsDefaults = True, **options):
    """Sets up a TrkDistanceFinderNeutralCharged tool and returns it.

    The following options have BTaggingFlags defaults:

    OutputLevel                         default: BTaggingFlags.OutputLevel

    input:             name: The name of the tool (should be unique).
      useBTagFlagsDefaults : Whether to use BTaggingFlags defaults for options that are not specified.
                  **options: Python dictionary with options for the tool.
    output: The actual tool, which can then by added to ToolSvc via ToolSvc += output."""
    if useBTagFlagsDefaults:
        defaults = { 'OutputLevel' : BTaggingFlags.OutputLevel }
        for option in defaults:
            options.setdefault(option, defaults[option])
    options['name'] = name
    from TrkJetVxFitter.TrkJetVxFitterConf import Trk__TrkDistanceFinderNeutralCharged
    return Trk__TrkDistanceFinderNeutralCharged(**options)

#--------------------------------------------------------------------------

metaTrkDistanceFinderNeutralNeutral = { 'ToolCollection' : 'NewJetFitterTag' }

def toolTrkDistanceFinderNeutralNeutral(name, useBTagFlagsDefaults = True, **options):
    """Sets up a TrkDistanceFinderNeutralNeutral tool and returns it.

    The following options have BTaggingFlags defaults:

    OutputLevel                         default: BTaggingFlags.OutputLevel

    input:             name: The name of the tool (should be unique).
      useBTagFlagsDefaults : Whether to use BTaggingFlags defaults for options that are not specified.
                  **options: Python dictionary with options for the tool.
    output: The actual tool, which can then by added to ToolSvc via ToolSvc += output."""
    if useBTagFlagsDefaults:
        defaults = { 'OutputLevel' : BTaggingFlags.OutputLevel }
        for option in defaults:
            options.setdefault(option, defaults[option])
    options['name'] = name
    from TrkJetVxFitter.TrkJetVxFitterConf import Trk__TrkDistanceFinderNeutralNeutral
    return Trk__TrkDistanceFinderNeutralNeutral(**options)

#--------------------------------------------------------------------------

metaImprovedJetFitterInitializationHelper = { 'DependsOn'      : ['JetFitterFullLinearizedTrackFactory',],
                                              'PassByPointer'  : {'LinearizedTrackFactory' : 'JetFitterFullLinearizedTrackFactory' },
                                              'ToolCollection' : 'NewJetFitterTag' }

def toolImprovedJetFitterInitializationHelper(name, useBTagFlagsDefaults = True, **options):
    """Sets up a ImprovedJetFitterInitializationHelper tool and returns it.

    The following options have BTaggingFlags defaults:

    OutputLevel                         default: BTaggingFlags.OutputLevel

    input:             name: The name of the tool (should be unique).
      useBTagFlagsDefaults : Whether to use BTaggingFlags defaults for options that are not specified.
                  **options: Python dictionary with options for the tool.
    output: The actual tool, which can then by added to ToolSvc via ToolSvc += output."""
    if useBTagFlagsDefaults:
        defaults = { 'OutputLevel' : BTaggingFlags.OutputLevel }
        for option in defaults:
            options.setdefault(option, defaults[option])
    options['name'] = name
    from TrkJetVxFitter.TrkJetVxFitterConf import Trk__JetFitterInitializationHelper
    return Trk__JetFitterInitializationHelper(**options)

#--------------------------------------------------------------------------

metaJetFitterSequentialVertexFitter = { 'DependsOn'      : ['JetFitterSequentialVertexSmoother',
                                                            'JetFitterFullLinearizedTrackFactory',
                                                            'VxInternalEdmFactory'],
                                        'PassByPointer'  : {'VertexSmoother'         : 'JetFitterSequentialVertexSmoother',
                                                            'LinearizedTrackFactory' : 'JetFitterFullLinearizedTrackFactory',
                                                            'XAODConverter'          : 'VxInternalEdmFactory' },
                                        'ToolCollection' : 'NewJetFitterTag' }

def toolJetFitterSequentialVertexFitter(name, useBTagFlagsDefaults = True, **options):
    """Sets up a JetFitterSequentialVertexFitter tool and returns it.

    input:             name: The name of the tool (should be unique).
      useBTagFlagsDefaults : Whether to use BTaggingFlags defaults for options that are not specified.
                             Note however that this tool has no BTaggingFlags defaults; the option is
                             here only for consistency.
                  **options: Python dictionary with options for the tool.
    output: The actual tool, which can then by added to ToolSvc via ToolSvc += output."""
    options['name'] = name
    from TrkVertexFitters.TrkVertexFittersConf import Trk__SequentialVertexFitter
    return Trk__SequentialVertexFitter(**options)

#--------------------------------------------------------------------------

metaJetFitterSequentialVertexSmoother = { 'ToolCollection' : 'NewJetFitterTag' }

def toolJetFitterSequentialVertexSmoother(name, useBTagFlagsDefaults = True, **options):
    """Sets up a JetFitterSequentialVertexSmoother tool and returns it.

    input:             name: The name of the tool (should be unique).
      useBTagFlagsDefaults : Whether to use BTaggingFlags defaults for options that are not specified.
                             Note however that this tool has no BTaggingFlags defaults; the option is
                             here only for consistency.
                  **options: Python dictionary with options for the tool.
    output: The actual tool, which can then by added to ToolSvc via ToolSvc += output."""
    options['name'] = name
    from TrkVertexFitters.TrkVertexFittersConf import Trk__SequentialVertexSmoother
    return Trk__SequentialVertexSmoother(**options)

#--------------------------------------------------------------------------

metaJetFitterMode3dTo1dFinder = { 'ToolCollection' : 'NewJetFitterTag' }

def toolJetFitterMode3dTo1dFinder(name, useBTagFlagsDefaults = True, **options):
    """Sets up a JetFitterMode3dTo1dFinder tool and returns it.

    input:             name: The name of the tool (should be unique).
      useBTagFlagsDefaults : Whether to use BTaggingFlags defaults for options that are not specified.
                             Note however that this tool has no BTaggingFlags defaults; the option is
                             here only for consistency.
                  **options: Python dictionary with options for the tool.
    output: The actual tool, which can then by added to ToolSvc via ToolSvc += output."""
    options['name'] = name
    from TrkVertexSeedFinderUtils.TrkVertexSeedFinderUtilsConf import Trk__Mode3dTo1dFinder
    return Trk__Mode3dTo1dFinder(**options)

#--------------------------------------------------------------------------

metaInDetImprovedJetFitterTrackSelectorTool = { 'DependsOn'      : ['JetFitterExtrapolator',],
                                                'PassByPointer'  : {'Extrapolator' : 'JetFitterExtrapolator'},
                                                'ToolCollection' : 'NewJetFitterTag' }

def toolInDetImprovedJetFitterTrackSelectorTool(name, useBTagFlagsDefaults = True, **options):
    """Sets up a InDetImprovedJetFitterTrackSelectorTool tool and returns it.

    The following options have BTaggingFlags defaults:

    pTMin                               default: 500.0
    IPd0Max                             default: 7.0
    IPz0Max                             default: 10.0
    sigIPd0Max                          default: 0.35
    sigIPz0Max                          default: 2.5
    etaMax                              default: 9999.0
    useTrackSummaryInfo                 default: True
    nHitBLayer                          default: 0
    nHitPix                             default: 1
    nHitSct                             default: 4
    nHitSi                              default: 7
    nHitTrt                             default: 0
    useSharedHitInfo                    default: False
    useTrackQualityInfo                 default: True
    fitChi2OnNdfMax                     default: 3.5
    TrackSummaryTool                    default: None

    input:             name: The name of the tool (should be unique).
      useBTagFlagsDefaults : Whether to use BTaggingFlags defaults for options that are not specified.
                  **options: Python dictionary with options for the tool.
    output: The actual tool, which can then by added to ToolSvc via ToolSvc += output."""
    if useBTagFlagsDefaults:
        defaults = { 'pTMin'                        : 500.0,
                     'IPd0Max'                      : 7.0,
                     'IPz0Max'                      : 10.0,
                     'sigIPd0Max'                   : 0.35,
                     'sigIPz0Max'                   : 2.5,
                     'etaMax'                       : 9999.0,
                     'useTrackSummaryInfo'          : True,
                     'nHitBLayer'                   : 0,
                     'nHitPix'                      : 1,
                     'nHitSct'                      : 4,
                     'nHitSi'                       : 7,
                     'nHitTrt'                      : 0,
                     'useSharedHitInfo'             : False,
                     'useTrackQualityInfo'          : True,
                     'fitChi2OnNdfMax'              : 3.5,
                     'TrackSummaryTool'             : None }
        for option in defaults:
            options.setdefault(option, defaults[option])
    options['name'] = name
    from InDetTrackSelectorTool.InDetTrackSelectorToolConf import InDet__InDetDetailedTrackSelectorTool
    return InDet__InDetDetailedTrackSelectorTool(**options)
