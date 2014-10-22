# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# Configuration functions for JetFitterTag
# Author: Wouter van den Wollenberg (2013-2014)
from BTagging.BTaggingFlags import BTaggingFlags

metaJetFitterTag = { 'IsATagger'         : True,
                     'DependsOn'         : ['AtlasExtrapolator',
                                            'BTagTrackToVertexTool',
                                            'JetFitterVxFinder',
                                            'JetFitterLikelihood',
                                            'JetFitterVariablesFactory',
                                            'JetFitterNtupleWriter'],
                     'PassByPointer'     : {'JetFitterVariablesFactory' : 'JetFitterVariablesFactory',
                                            'JetFitterLikelihood'       : 'JetFitterLikelihood',
                                            'JetFitterNtupleWriter'     : 'JetFitterNtupleWriter'},
                     'PassByName'        : {'SecVxFinderName' : 'JetFitterVxFinder'},
                     'JetCollectionList' : 'jetCollectionList',
                     'ToolCollection'    : 'JetFitterTag' }

def toolJetFitterTag(name, useBTagFlagsDefaults = True, **options):
    """Sets up a JetFitterTag tool and returns it.

    The following options have BTaggingFlags defaults:

    OutputLevel                         default: BTaggingFlags.OutputLevel
    Runmodus                            default: BTaggingFlags.Runmodus
    jetCollectionList                   default: BTaggingFlags.Jets
    SecVxFinderName                     default: "JetFitterVxFinder"
    useForcedCalibration                default: False
    storeOnlyBaseObject                 default: False

    input:             name: The name of the tool (should be unique).
      useBTagFlagsDefaults : Whether to use BTaggingFlags defaults for options that are not specified.
                  **options: Python dictionary with options for the tool.
    output: The actual tool, which can then by added to ToolSvc via ToolSvc += output."""
    if useBTagFlagsDefaults:
        defaults = { 'OutputLevel'                      : BTaggingFlags.OutputLevel,
                     'Runmodus'                         : BTaggingFlags.Runmodus,
                     'jetCollectionList'                : BTaggingFlags.Jets,
                     'SecVxFinderName'                  : 'JetFitterVxFinder',
                     'useForcedCalibration'             : False,
                     'storeOnlyBaseObject'              : False }
        for option in defaults:
            options.setdefault(option, defaults[option])
    options['name'] = name
    from JetTagTools.JetTagToolsConf import Analysis__JetFitterTag
    return Analysis__JetFitterTag(**options)

#-----------------------------------------------------------------------

metaJetFitterLikelihood = { 'CalibrationFolders' : ['JetFitter',],
                            'DependsOn'          : ['BTagCalibrationBrokerTool',],
                            'PassByPointer'      : {'calibrationTool' : 'BTagCalibrationBrokerTool'},
                            'ToolCollection'     : 'JetFitterTag' }

def toolJetFitterLikelihood(name, useBTagFlagsDefaults = True, **options):
    """Sets up a JetFitterLikelihood tool and returns it.

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

#-----------------------------------------------------------------------

metaJetFitterVariablesFactory = { 'PassByName'     : {'JetFitterInstance' : 'JetFitterTag' },
                                  'ToolCollection' : 'JetFitterTag' }

def toolJetFitterVariablesFactory(name, useBTagFlagsDefaults = True, **options):
    """Sets up a JetFitterVariablesFactory tool and returns it.

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
        for option in defaults:
            options.setdefault(option, defaults[option])
    options['name'] = name
    from JetTagTools.JetTagToolsConf import Analysis__JetFitterVariablesFactory
    return Analysis__JetFitterVariablesFactory(**options)

#-----------------------------------------------------------------------

metaJetFitterNtupleWriter = { 'ToolCollection' : 'JetFitterTag' }

def toolJetFitterNtupleWriter(name, useBTagFlagsDefaults = True, **options):
    """Sets up a JetFitterNtupleWriter tool and returns it.

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

#-----------------------------------------------------------------------

metaJetFitterRoutines = { 'DependsOn'      : ['JetFitterInitializationHelper',],
                          'PassByPointer'  : {'JetFitterInitializationHelper' : 'JetFitterInitializationHelper'},
                          'ToolCollection' : 'JetFitterTag' }

def toolJetFitterRoutines(name, useBTagFlagsDefaults = True, **options):
    """Sets up a JetFitterRoutines tool and returns it.

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

#-----------------------------------------------------------------------

metaJetFitterInitializationHelper = { 'DependsOn'      : ['JetFitterFullLinearizedTrackFactory',],
                                      'PassByPointer'  : { 'LinearizedTrackFactory' : 'JetFitterFullLinearizedTrackFactory' },
                                      'ToolCollection' : 'JetFitterTag' }

def toolJetFitterInitializationHelper(name, useBTagFlagsDefaults = True, **options):
    """Sets up a JetFitterInitializationHelper tool and returns it.

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

#-----------------------------------------------------------------------

metaInDetJetFitterTrackSelectorTool = { 'DependsOn'      : ['JetFitterExtrapolator',],
                                        'PassByPointer'  : {'Extrapolator' : 'JetFitterExtrapolator' },
                                        'ToolCollection' : 'JetFitterTag' }

def toolInDetJetFitterTrackSelectorTool(name, useBTagFlagsDefaults = True, **options):
    """Sets up a InDetJetFitterTrackSelectorTool tool and returns it.

    The following options have BTaggingFlags defaults:

    pTMin                               default: 769.2
    IPd0Max                             default: 3.5
    IPz0Max                             default: 5
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
        defaults = { 'pTMin'                      : 769.2,
                     'IPd0Max'                    : 3.5,
                     'IPz0Max'                    : 5,
                     'sigIPd0Max'                 : 0.35,
                     'sigIPz0Max'                 : 2.5,
                     'etaMax'                     : 9999.0,
                     'useTrackSummaryInfo'        : True,
                     'nHitBLayer'                 : 0,
                     'nHitPix'                    : 1,
                     'nHitSct'                    : 4,
                     'nHitSi'                     : 7,
                     'nHitTrt'                    : 0,
                     'useSharedHitInfo'           : False,
                     'useTrackQualityInfo'        : True,
                     'fitChi2OnNdfMax'            : 3.5,
                     'TrackSummaryTool'           : None }
        for option in defaults:
            options.setdefault(option, defaults[option])
    options['name'] = name
    from InDetTrackSelectorTool.InDetTrackSelectorToolConf import InDet__InDetDetailedTrackSelectorTool
    return InDet__InDetDetailedTrackSelectorTool(**options)

#-----------------------------------------------------------------------

metaJetFitterVxFinder = { 'IsAVertexFinder' : True,
                          'DependsOn'       : ['AtlasExtrapolator',
                                               'BTagTrackToVertexTool',
                                               'JetFitterExtrapolator',
                                               'InDetJetFitterTrackSelectorTool',
                                               'JetFitterInitializationHelper',
                                               'JetFitterRoutines',
                                               'JetFitterFullLinearizedTrackFactory'],
                          'PassByPointer'   : {'JetFitterInitializationHelper' : 'JetFitterInitializationHelper',
                                               'JetFitterRoutines'             : 'JetFitterRoutines',
                                               'TrackSelector'                 : 'InDetJetFitterTrackSelectorTool' },
                          'ToolCollection'  : 'JetFitterTag' }

def toolJetFitterVxFinder(name, useBTagFlagsDefaults = True, **options):
    """Sets up a JetFitterVxFinder tool and returns it.

    The following options have BTaggingFlags defaults:

    OutputLevel                         default: BTaggingFlags.OutputLevel
    MaxNumDeleteIterations              default: 30
    VertexProbCut                       default: 0.01
    MaxClusteringIterations             default: 30
    VertexClusteringProbabilityCut      default: 0.01

    input:             name: The name of the tool (should be unique).
      useBTagFlagsDefaults : Whether to use BTaggingFlags defaults for options that are not specified.
                  **options: Python dictionary with options for the tool.
    output: The actual tool, which can then by added to ToolSvc via ToolSvc += output."""
    if useBTagFlagsDefaults:
        defaults = { 'OutputLevel'                    : BTaggingFlags.OutputLevel,
                     'MaxNumDeleteIterations'         : 30,
                     'VertexProbCut'                  : 0.01,
                     'MaxClusteringIterations'        : 30,
                     'VertexClusteringProbabilityCut' : 0.01 }
        for option in defaults:
            options.setdefault(option, defaults[option])
    options['name'] = name
    from InDetSecVxFinderTool.InDetSecVxFinderToolConf import InDet__InDetJetFitterVxFinder
    return InDet__InDetJetFitterVxFinder(**options)
