# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# Configuration functions for NewJetFitterTag
# Author: Wouter van den Wollenberg (2013-2014)
# Because there are some many different JetFitter tools floating around
# now, this file was created to contain them all.
from BTagging.BTaggingFlags import BTaggingFlags

metaJetFitterExtrapolator = {'OneInTotal'     : True,
                             'ToolCollection' : 'JetFitterCollection' }

def toolJetFitterExtrapolator(name, useBTagFlagsDefaults = True, **options):
    """Sets up a JetFitterExtrapolator tool and returns it.

    input:             name: The name of the tool (should be unique).
      useBTagFlagsDefaults : Whether to use BTaggingFlags defaults for options that are not specified.
                  **options: Python dictionary with options for the tool.
    output: The actual tool, which can then by added to ToolSvc via ToolSvc += output."""
    options['name'] = name
    from TrkExTools.AtlasExtrapolator import AtlasExtrapolator
    return AtlasExtrapolator(**options)

#--------------------------------------------------------------------------

metaInDetImprovedJetFitterTrackSelectorTool = { 'DependsOn'      : ['JetFitterExtrapolator',],
                                                'PassByPointer'  : {'Extrapolator' : 'JetFitterExtrapolator'},
                                                'ToolCollection' : 'JetFitterCollection' }

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

#--------------------------------------------------------------------------

metaJetFitterFullLinearizedTrackFactory = {'DependsOn'     : ['JetFitterExtrapolator',],
                                           'PassByPointer' : {'Extrapolator' : 'JetFitterExtrapolator' },
                                           'ToolCollection': 'JetFitterCollection' }

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

#--------------------------------------------------------------------------

metaJetFitterMode3dTo1dFinder = { 'ToolCollection' : 'JetFitterCollection' }

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

metaJetFitterSequentialVertexSmoother = { 'ToolCollection' : 'JetFitterCollection' }

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

metaVxInternalEdmFactory = {'DependsOn'     : ['JetFitterFullLinearizedTrackFactory',],
                            'PassByPointer' : {'LinearizedTrackFactory' : 'JetFitterFullLinearizedTrackFactory' },
                            'ToolCollection': 'JetFitterCollection' }

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

def toolVxInternalEdmFactory_SV(name, useBTagFlagsDefaults = True, **options):
    """Sets up a VxInternalEdmFactory tool and returns it.

    The following options have BTaggingFlags defaults:

    OutputLevel                         default: BTaggingFlags.OutputLevel

    input:             name: The name of the tool (should be unique).
      useBTagFlagsDefaults : Whether to use BTaggingFlags defaults for options that are not specified.
                  **options: Python dictionary with options for the tool.
    output: The actual tool, which can then by added to ToolSvc via ToolSvc += output."""
    if useBTagFlagsDefaults:
        jetFitterFullLinearizedTrackFactory = toolJetFitterFullLinearizedTrackFactory('JetFitterFullLinearizedTrackFactory')
        defaults = { 'OutputLevel'             : BTaggingFlags.OutputLevel,
                     'LinearizedTrackFactory'  : jetFitterFullLinearizedTrackFactory }
        for option in defaults:
            options.setdefault(option, defaults[option])
    options['name'] = name
    from TrkVxEdmCnv.TrkVxEdmCnvConf import Trk__VxCandidateXAODVertex
    return Trk__VxCandidateXAODVertex(**options)

#--------------------------------------------------------------------------

metaJetFitterSequentialVertexFitter = { 'DependsOn'      : ['JetFitterSequentialVertexSmoother',
                                                            'JetFitterFullLinearizedTrackFactory'] ,
                                        'PassByPointer'  : {'VertexSmoother'         : 'JetFitterSequentialVertexSmoother',
                                                            'LinearizedTrackFactory' : 'JetFitterFullLinearizedTrackFactory'},

                                        'ToolCollection' : 'JetFitterCollection' }

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

def toolJetFitterSequentialVertexFitter_SV(name, useBTagFlagsDefaults = True, **options):
    """Sets up a JetFitterSequentialVertexFitter tool and returns it.

    input:             name: The name of the tool (should be unique).
      useBTagFlagsDefaults : Whether to use BTaggingFlags defaults for options that are not specified.
                             Note however that this tool has no BTaggingFlags defaults; the option is
                             here only for consistency.
                  **options: Python dictionary with options for the tool.
    output: The actual tool, which can then by added to ToolSvc via ToolSvc += output."""
    if useBTagFlagsDefaults:
        jetFitterFullLinearizedTrackFactory = toolJetFitterFullLinearizedTrackFactory('JFFullLinearizedTrackFactory')
        jetFitterSequentialVertexSmoother = toolJetFitterSequentialVertexSmoother('JFSequentialVertexSmoother')
        defaults = { 'LinearizedTrackFactory' : jetFitterFullLinearizedTrackFactory,
                     'VertexSmoother'         : jetFitterSequentialVertexSmoother, }
        for option in defaults:
            options.setdefault(option, defaults[option])
    options['name'] = name
    from TrkVertexFitters.TrkVertexFittersConf import Trk__SequentialVertexFitter
    return Trk__SequentialVertexFitter(**options)

#--------------------------------------------------------------------------

metaImprovedJetFitterInitializationHelper = { 'DependsOn'      : ['JetFitterFullLinearizedTrackFactory',],
                                              'PassByPointer'  : {'LinearizedTrackFactory' : 'JetFitterFullLinearizedTrackFactory' },
                                              'ToolCollection' : 'JetFitterCollection' }

def toolImprovedJetFitterInitializationHelper(name, useBTagFlagsDefaults = True, **options):
    """Sets up a ImprovedJetFitterInitializationHelper tool and returns it.

    The following options have BTaggingFlags defaults:

    OutputLevel                         default: BTaggingFlags.OutputLevel

    input:             name: The name of the tool (should be unique).
      useBTagFlagsDefaults : Whether to use BTaggingFlags defaults for options that are not specified.
                  **options: Python dictionary with options for the tool.
    output: The actual tool, which can then by added to ToolSvc via ToolSvc += output."""
    if useBTagFlagsDefaults:
        jetFitterFullLinearizedTrackFactory = toolJetFitterFullLinearizedTrackFactory('JFFullLinearizedTrackFactory')
        defaults = { 'OutputLevel' : BTaggingFlags.OutputLevel,
		     'LinearizedTrackFactory' : jetFitterFullLinearizedTrackFactory}
        for option in defaults:
            options.setdefault(option, defaults[option])
    options['name'] = name
    from TrkJetVxFitter.TrkJetVxFitterConf import Trk__JetFitterInitializationHelper
    return Trk__JetFitterInitializationHelper(**options)

#--------------------------------------------------------------------------

metaTrkDistanceFinderNeutralNeutral = { 'ToolCollection' : 'JetFitterCollection' }

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

metaTrkDistanceFinderNeutralCharged = { 'ToolCollection' : 'JetFitterCollection' }

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

metaImprovedJetFitterRoutines = { 'DependsOn'      : ['ImprovedJetFitterInitializationHelper',
                                                      'TrkDistanceFinderNeutralNeutral',
                                                      'TrkDistanceFinderNeutralCharged'],
                                  'PassByPointer'  : {'JetFitterInitializationHelper'         : 'ImprovedJetFitterInitializationHelper',
                                                      'JetFitterMinimumDistanceFinderNeutral' : 'TrkDistanceFinderNeutralNeutral',
                                                      'JetFitterMinimumDistanceFinder'        : 'TrkDistanceFinderNeutralCharged' },
                                  'ToolCollection' : 'JetFitterCollection' }

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
        improvedJetFitterInitializationHelper = toolImprovedJetFitterInitializationHelper('ImprovedJFInitHelper')
        trkDistanceFinderNeutralNeutral = toolTrkDistanceFinderNeutralNeutral('TrkDistFinderNeutralNeutral')
        trkDistanceFinderNeutralCharged = toolTrkDistanceFinderNeutralCharged('TrkDistFinderNeutralCharged')
        defaults = { 'OutputLevel'          : BTaggingFlags.OutputLevel,
                     'BeFast'               : False,
                     'maxDRshift'           : 0.0,
                     'noPrimaryVertexRefit' : False,
                     'JetFitterInitializationHelper' : improvedJetFitterInitializationHelper,
                     'JetFitterMinimumDistanceFinder' : trkDistanceFinderNeutralCharged,
                     'JetFitterMinimumDistanceFinderNeutral' : trkDistanceFinderNeutralNeutral, }
        for option in defaults:
            options.setdefault(option, defaults[option])
    options['name'] = name
    from TrkJetVxFitter.TrkJetVxFitterConf import Trk__JetFitterRoutines
    return Trk__JetFitterRoutines(**options)

#--------------------------------------------------------------------------

metaInDetJetFitterUtils = { 'DependsOn'      : ['JetFitterFullLinearizedTrackFactory',
                                                'JetFitterExtrapolator'],
                            'PassByPointer'  : {'LinearizedTrackFactory' : 'JetFitterFullLinearizedTrackFactory',
                                               'Extrapolator'           : 'JetFitterExtrapolator' },
                            'ToolCollection' : 'JetFitterCollection' }

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

def toolInDetJetFitterUtils_SV(name, useBTagFlagsDefaults = True, **options):
    """Sets up a InDetJetFitterUtils tool and returns it.

    The following options have BTaggingFlags defaults:

    OutputLevel                         default: BTaggingFlags.OutputLevel

    input:             name: The name of the tool (should be unique).
      useBTagFlagsDefaults : Whether to use BTaggingFlags defaults for options that are not specified.
                  **options: Python dictionary with options for the tool.
    output: The actual tool, which can then by added to ToolSvc via ToolSvc += output."""
    if useBTagFlagsDefaults:
        jetFitterFullLinearizedTrackFactory = toolJetFitterFullLinearizedTrackFactory('JFFullLinearizedTrackFactory')
        jetFitterExtrapolator = toolJetFitterExtrapolator('JFExtrapolator')
        defaults = { 'OutputLevel'          : BTaggingFlags.OutputLevel,
                     'LinearizedTrackFactory': jetFitterFullLinearizedTrackFactory,
                     'Extrapolator' : jetFitterExtrapolator }
        for option in defaults:
            options.setdefault(option, defaults[option])
    options['name'] = name
    from InDetSecVxFinderTool.InDetSecVxFinderToolConf import InDet__InDetJetFitterUtils
    return InDet__InDetJetFitterUtils(**options)

#--------------------------------------------------------------------------



#--------------------------------------------------------------------------

metaJetFitterVariablesFactory = { #'PassByName'     : {'JetFitterInstance' : 'JetFitterTag',
                                  #                    'secVxFinderName'   : 'JetFitterVxFinder' },
                                  'ToolCollection' : 'JetFitterCollection' }

def toolJetFitterVariablesFactory(name, useBTagFlagsDefaults = True, **options):
    """Sets up a NewJetFitterVariablesFactory tool and returns it.

    The following options have BTaggingFlags defaults:

    OutputLevel                         default: BTaggingFlags.OutputLevel
    JetFitterInstance                   default: "JetFitterTag"
    secVxFinderName                     default: "JetFitterVxFinder"

    input:             name: The name of the tool (should be unique).
      useBTagFlagsDefaults : Whether to use BTaggingFlags defaults for options that are not specified.
                  **options: Python dictionary with options for the tool.
    output: The actual tool, which can then by added to ToolSvc via ToolSvc += output."""
    if useBTagFlagsDefaults:
        defaults = { 'OutputLevel'            : BTaggingFlags.OutputLevel,
                     'JetFitterInstance'      : 'JetFitterTag',
                     'secVxFinderName'        : 'JetFitterVxFinder' }
        for option in defaults:
            options.setdefault(option, defaults[option])
    options['name'] = name
    from JetTagTools.JetTagToolsConf import Analysis__JetFitterVariablesFactory
    return Analysis__JetFitterVariablesFactory(**options)

#--------------------------------------------------------------------------

metaJetFitterNtupleWriterNN = { 'ToolCollection' : 'JetFitterCollection' }

def toolJetFitterNtupleWriterNN(name, useBTagFlagsDefaults = True, **options):
    """Sets up a JetFitterNtupleWriterNN tool and returns it.

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

#-------------------------------------------------------------------------

metaNeuralNetworkToHistoToolNN = { 'ToolCollection' : 'JetFitterCollection' }

def toolNeuralNetworkToHistoToolNN(name, useBTagFlagsDefaults = True, **options):
    """Sets up a NeuralNetworkToHistoToolNN tool and returns it.

    The following options have BTaggingFlags defaults:

    OutputLevel                         default: BTaggingFlags.OutputLevel

    input:             name: The name of the tool (should be unique).
      useBTagFlagsDefaults : Whether to use BTaggingFlags defaults for options that are not specified.
                  **options: Python dictionary with options for the tool.
    output: The actual tool, which can then by added to ToolSvc via ToolSvc += output."""
    if useBTagFlagsDefaults:
        defaults = { 'OutputLevel'                      : BTaggingFlags.OutputLevel }
        for option in defaults:
            options.setdefault(option, defaults[option])
    options['name'] = name
    from TrkNeuralNetworkUtils.TrkNeuralNetworkUtilsConf import Trk__NeuralNetworkToHistoTool
    return Trk__NeuralNetworkToHistoTool(**options)

#-------------------------------------------------------------------------

metaJetFitterNNTool = { 'CalibrationTaggers' : ['JetFitter',],
                        #'DependsOn'          : ['NeuralNetworkToHistoToolNN'],
                        #'PassByPointer'      : {'NeuralNetworkToHistoTool' : 'NeuralNetworkToHistoToolNN'},
                        'ToolCollection'     : 'JetFitterCollection' }

def _slim_jf_options(options):
    new_opts = {}
    removed = set(
        ['CalibrationDirectory', 'CalibrationSubDirectory',
         'NeuralNetworkToHistoTool'])
    kept = set(options) - removed
    for optname in kept:
        new_opts[optname] = options[optname]
    return new_opts

def toolJetFitterNNTool(name, useBTagFlagsDefaults = True, **options):
    """Sets up a JetFitterNNTool tool and returns it.

    The following options have BTaggingFlags defaults:

    OutputLevel                         default: BTaggingFlags.OutputLevel
    CalibrationDirectory                default: "JetFitter"
    CalibrationSubDirectory             default: "NeuralNetwork"
    useCombinedIPNN                     default: False
    usePtCorrectedMass                  default: True

    input:             name: The name of the tool (should be unique).
      useBTagFlagsDefaults : Whether to use BTaggingFlags defaults for options that are not specified.
                  **options: Python dictionary with options for the tool.
    output: The actual tool, which can then by added to ToolSvc via ToolSvc += output."""
    if useBTagFlagsDefaults:
        nnToHistoTool = toolNeuralNetworkToHistoToolNN('NeuralNetworkToHistoToolNN')
        defaults = { 'OutputLevel'                      : BTaggingFlags.OutputLevel,
                     'useCombinedIPNN'                  : False,
                     'CalibrationDirectory'             : 'JetFitter',
                     'CalibrationSubDirectory'          : 'NeuralNetwork',
                     'usePtCorrectedMass'               : True,
                     'NeuralNetworkToHistoTool'         : nnToHistoTool, }
        for option in defaults:
            options.setdefault(option, defaults[option])
    options['name'] = name
    from JetTagTools.JetTagToolsConf import Analysis__JetFitterNNTool
    from JetTagTools.JetTagToolsConf import Analysis__JetFitterInputWriter
    if BTaggingFlags.RunJetFitterNNTool:
        return Analysis__JetFitterNNTool(**options)
    else:
        return Analysis__JetFitterInputWriter(**_slim_jf_options(options))

#-------------------------------------------------------------------------

metaJetFitterTagNN = { 'IsATagger'         : True,
                       'xAODBaseName'      : 'JetFitter',
                       'DependsOn'         : [#'AtlasExtrapolator',
                                              #'BTagTrackToVertexTool',
                                              #'NewJetFitterVxFinder',
                                              #'JetFitterNNTool',
                                              #'NeuralNetworkToHistoToolNN',
                                              #'JetFitterNtupleWriterNN'
                                             ],
                       'PassByPointer'     : { #'jetfitterClassifier'       : 'JetFitterNNTool',
                                               #'jetfitterNtupleWriter'     : 'JetFitterNtupleWriterNN'
                                             },
                       'JetCollectionList' : 'jetCollectionList',
                       'ToolCollection'    : 'JetFitterCollection' }

def toolJetFitterTagNN(name, useBTagFlagsDefaults = True, **options):
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
        jetFitterNtupleWriterNN = toolJetFitterNtupleWriterNN('JetFitterNtupleWriterNN')
        jetfitterClassifier = toolJetFitterNNTool('JetFitterNNTool')
        defaults = { 'OutputLevel'                      : BTaggingFlags.OutputLevel,
                     'Runmodus'                         : BTaggingFlags.Runmodus,
                     'jetCollectionList'                : BTaggingFlags.Jets,
                     'SecVxFinderName'                  : 'JetFitter',
                     'useForcedCalibration'             : False,
                     'ipinfoTaggerName'                 : "",
                     'jetfitterNtupleWriter'            : jetFitterNtupleWriterNN,
                     'jetfitterClassifier'              : jetfitterClassifier,
                     }
        for option in defaults:
            options.setdefault(option, defaults[option])
    options['name'] = name
    from JetTagTools.JetTagToolsConf import Analysis__JetFitterTag
    return Analysis__JetFitterTag(**options)

#-------------------------------------------------------------------------

metaJetFitterNtupleWriterCOMBNN = { 'ToolCollection' : 'JetFitterCollection' }

def toolJetFitterNtupleWriterCOMBNN(name, useBTagFlagsDefaults = True, **options):
    """Sets up a JetFitterNtupleWriterCOMBNN tool and returns it.

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

#-------------------------------------------------------------------------

metaNeuralNetworkToHistoToolCOMBNN = { 'ToolCollection' : 'JetFitterCollection' }

def toolNeuralNetworkToHistoToolCOMBNN(name, useBTagFlagsDefaults = True, **options):
    """Sets up a NeuralNetworkToHistoToolCOMBNN tool and returns it.

    The following options have BTaggingFlags defaults:

    OutputLevel                         default: BTaggingFlags.OutputLevel

    input:             name: The name of the tool (should be unique).
      useBTagFlagsDefaults : Whether to use BTaggingFlags defaults for options that are not specified.
                  **options: Python dictionary with options for the tool.
    output: The actual tool, which can then by added to ToolSvc via ToolSvc += output."""
    if useBTagFlagsDefaults:
        defaults = { 'OutputLevel'                      : BTaggingFlags.OutputLevel }
        for option in defaults:
            options.setdefault(option, defaults[option])
    options['name'] = name
    from TrkNeuralNetworkUtils.TrkNeuralNetworkUtilsConf import Trk__NeuralNetworkToHistoTool
    return Trk__NeuralNetworkToHistoTool(**options)

#-----------------------------------------------------------------------------

metaJetFitterCOMBNNTool = { 'CalibrationTaggers' : ['JetFitter',],
                            'DependsOn'          : ['NeuralNetworkToHistoToolCOMBNN'],
                            'PassByPointer'      : {'NeuralNetworkToHistoTool' : 'NeuralNetworkToHistoToolCOMBNN'},
                            'ToolCollection'     : 'JetFitterCollection' }

def toolJetFitterCOMBNNTool(name, useBTagFlagsDefaults = True, **options):
    """Sets up a JetFitterCOMBNNTool tool and returns it.

    The following options have BTaggingFlags defaults:

    OutputLevel                         default: BTaggingFlags.OutputLevel
    CalibrationDirectory                default: "JetFitter"
    CalibrationSubDirectory             default: "NeuralNetwork"
    useCombinedIPNN                     default: True
    usePtCorrectedMass                  default: True

    input:             name: The name of the tool (should be unique).
      useBTagFlagsDefaults : Whether to use BTaggingFlags defaults for options that are not specified.
                  **options: Python dictionary with options for the tool.
    output: The actual tool, which can then by added to ToolSvc via ToolSvc += output."""
    if useBTagFlagsDefaults:
        defaults = { 'OutputLevel'                      : BTaggingFlags.OutputLevel,
                     'CalibrationDirectory'             : 'JetFitter',
                     'CalibrationSubDirectory'          : 'NeuralNetwork',
                     'useCombinedIPNN'                  : True,
                     'usePtCorrectedMass'               : True }
        for option in defaults:
            options.setdefault(option, defaults[option])
    options['name'] = name
    from JetTagTools.JetTagToolsConf import Analysis__JetFitterNNTool
    from JetTagTools.JetTagToolsConf import Analysis__JetFitterInputWriter
    if BTaggingFlags.RunJetFitterNNTool:
        return Analysis__JetFitterNNTool(**options)
    else:
        return Analysis__JetFitterInputWriter(**_slim_jf_options(options))


#-----------------------------------------------------------------------------

metaJetFitterTagCOMBNN = { 'IsATagger'         : True,
                           'xAODBaseName'      : 'JetFitterCombNN',
                           'DependsOn'         : ['AtlasExtrapolator',
                                                  'BTagTrackToVertexTool',
                                                  'JetFitterTagNN',
                                                  'IP3DTag',
                                                  'InDetVKalVxInJetTool',
                                                  'BTagFullLinearizedTrackFactory',
                                                  'BTagTrackToVertexIPEstimator',
                                                  'JetFitterCOMBNNTool',
                                                  'NeuralNetworkToHistoToolCOMBNN',
                                                  'JetFitterNtupleWriterCOMBNN'],
                           'PassByPointer'     : { 'jetfitterClassifier'       : 'JetFitterCOMBNNTool',
                                                   'jetfitterNtupleWriter'     : 'JetFitterNtupleWriterCOMBNN'},
                           'JetCollectionList' : 'jetCollectionList',
                           'ToolCollection'    : 'JetFitterCollection' }

def toolJetFitterTagCOMBNN(name, useBTagFlagsDefaults = True, **options):
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
                     'SecVxFinderName'                  : 'JetFitter',
                     'useForcedCalibration'             : False,
                     'ipinfoTaggerName'                 : 'IP3D',
                     }
        for option in defaults:
            options.setdefault(option, defaults[option])
    options['name'] = name
    from JetTagTools.JetTagToolsConf import Analysis__JetFitterTag
    return Analysis__JetFitterTag(**options)

#--------------------------------------------------------------------------
# new jf track selector tool for hackathon 2019
metaJetFitterTrackSelectorTool = {'VertexFinderxAODBaseName' : 'JetFitter',
                              'VertexFinderPassByPointer': {'JetFitterVariableFactory' : 'JetFitterVariablesFactory' },
                              'DependsOn'      : ['InDetJetFitterUtils',
                                                  'InDetImprovedJetFitterTrackSelectorTool',
                                                  'JetFitterExtrapolator'],
                              'PassByPointer'  : {'TrackSelector'                 : 'InDetImprovedJetFitterTrackSelectorTool',
                                                  'InDetJetFitterUtils'           : 'InDetJetFitterUtils',
                                                  'Extrapolator'                  : 'JetFitterExtrapolator' },
                              'ToolCollection' : 'JetFitterCollection' }

def toolJetFitterTrackSelectorTool(name, useBTagFlagsDefaults = True, **options):
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
    from InDetSecVxFinderTool.InDetSecVxFinderToolConf import InDet__JetFitterTrackSelectorTool
    return InDet__JetFitterTrackSelectorTool(**options)

def toolJetFitterTrackSelectorTool_SV(name, suffix="", useBTagFlagsDefaults = True, **options):
    """Sets up a InDetJetFitterUtils tool and returns it.

    The following options have BTaggingFlags defaults:

    OutputLevel                         default: BTaggingFlags.OutputLevel

    input:             name: The name of the tool (should be unique).
      useBTagFlagsDefaults : Whether to use BTaggingFlags defaults for options that are not specified.
                  **options: Python dictionary with options for the tool.
    output: The actual tool, which can then by added to ToolSvc via ToolSvc += output."""
    if useBTagFlagsDefaults:
        inDetJetFitterUtils = toolInDetJetFitterUtils_SV('InDetJFUtils'+suffix)
        ImprovedJetFitterTrackSelectorTool = toolInDetImprovedJetFitterTrackSelectorTool('ImprovedJFTrackSelTool'+suffix)
        jetFitterExtrapolator = toolJetFitterExtrapolator('JFExtrapolator'+suffix)
        defaults = { 'OutputLevel'          : BTaggingFlags.OutputLevel,
                     'InDetJetFitterUtils'  : inDetJetFitterUtils,
                     'TrackSelector'        : ImprovedJetFitterTrackSelectorTool,
                     'Extrapolator'        : jetFitterExtrapolator }
        for option in defaults:
            options.setdefault(option, defaults[option])
    options['name'] = name + suffix
    from InDetSecVxFinderTool.InDetSecVxFinderToolConf import InDet__JetFitterTrackSelectorTool
    return InDet__JetFitterTrackSelectorTool(**options)

#--------------------------------------------------------------------------


metaJetFitterMultiStageFit = { 'DependsOn'      : ['InDetJetFitterUtils',
                                                   'ImprovedJetFitterRoutines',
                                                   'ImprovedJetFitterInitializationHelper'],
                               'PassByPointer'  : {'JetFitterInitializationHelper' : 'ImprovedJetFitterInitializationHelper',
                                                   'InDetJetFitterUtils'           : 'InDetJetFitterUtils',
                                                   'JetFitterRoutines'             : 'ImprovedJetFitterRoutines'},
                               'ToolCollection' : 'JetFitterCollection'}

def toolJetFitterMultiStageFit(name, useBTagFlagsDefaults = True, **options):
    """Sets up the new JetFitterMultiStageFit tool and returns it.



    input:             name: The name of the tool (should be unique).
      useBTagFlagsDefaults : Whether to use BTaggingFlags defaults for options that are not specified.
                  **options: Python dictionary with options for the tool.
    output: The actual tool, which can then by added to ToolSvc via ToolSvc += output."""
    if useBTagFlagsDefaults:
        defaults = { 'OutputLevel'                         : BTaggingFlags.OutputLevel,
                     'MaxNumDeleteIterations'              : 30,
                     'VertexProbCut'                       : 0.001,
                     'MaxClusteringIterations'             : 30,
                     'VertexClusteringProbabilityCut'      : 0.005 }
    for option in defaults:
        options.setdefault(option, defaults[option])
    options['name'] = name
    from InDetSecVxFinderTool.InDetSecVxFinderToolConf import InDet__JetFitterMultiStageFit
    return InDet__JetFitterMultiStageFit(**options)

def toolJetFitterMultiStageFit_SV(name, suffix="", useBTagFlagsDefaults = True, **options):
    """Sets up the new JetFitterMultiStageFit tool and returns it.



    input:             name: The name of the tool (should be unique).
      useBTagFlagsDefaults : Whether to use BTaggingFlags defaults for options that are not specified.
                  **options: Python dictionary with options for the tool.
    output: The actual tool, which can then by added to ToolSvc via ToolSvc += output."""
    if useBTagFlagsDefaults:
        improvedJetFitterInitializationHelper = toolImprovedJetFitterInitializationHelper('ImprovedJFInitHelper'+suffix)
        inDetJetFitterUtils = toolInDetJetFitterUtils_SV('InDetJFUtils'+suffix)
        inDetJetFitterRoutines = toolImprovedJetFitterRoutines('ImprovedJetFitterRoutines'+suffix)
        defaults = { 'OutputLevel'                         : BTaggingFlags.OutputLevel,
                     'MaxNumDeleteIterations'              : 30,
                     'VertexProbCut'                       : 0.001,
                     'MaxClusteringIterations'             : 30,
                     'VertexClusteringProbabilityCut'      : 0.005,
                     'InDetJetFitterUtils'  : inDetJetFitterUtils,
                     'JetFitterInitializationHelper': improvedJetFitterInitializationHelper,
                     'JetFitterRoutines': inDetJetFitterRoutines }
    for option in defaults:
        options.setdefault(option, defaults[option])
    options['name'] = name + suffix
    from InDetSecVxFinderTool.InDetSecVxFinderToolConf import InDet__JetFitterMultiStageFit
    return InDet__JetFitterMultiStageFit(**options)

#--------------------------------------------------------------------------
metaNewJetFitterVxFinder = { 'IsAVertexFinder' : True,
                             'VertexFinderxAODBaseName' : 'JetFitter',
                             'VertexFinderPassByPointer': {'JetFitterVariableFactory' : 'JetFitterVariablesFactory' },
                             'DependsOn'       : ['BTagTrackToVertexTool',
                                                  'TrkDistanceFinderNeutralCharged',
                                                  'TrkDistanceFinderNeutralNeutral',
                                                  'JetFitterSequentialVertexSmoother',
                                                  'JetFitterFullLinearizedTrackFactory',
                                                  'JetFitterVariablesFactory',
                                                  'JetFitterTrackSelectorTool',
                                                  'JetFitterTwoTrackVtxFinderTool',
                                                  'JetFitterV0FinderTool',
                                                  'JetFitterMultiStageFit'],
                             'PassByPointer'    : { 'JetFitterTrackSelectorTool'    : 'JetFitterTrackSelectorTool',
                                                    'JetFitterTwoTrackVtxFinderTool': 'JetFitterTwoTrackVtxFinderTool',
                                                    'JetFitterV0FinderTool'         : 'JetFitterV0FinderTool',
                                                    'JetFitterMultiStageFit'        : 'JetFitterMultiStageFit'},
                             'ToolCollection'   : 'JetFitterCollection' }

def toolNewJetFitterVxFinder(name, useBTagFlagsDefaults = True, **options):
    """Sets up a NewJetFitterVxFinder tool and returns it.

    The following options have BTaggingFlags defaults:

    OutputLevel                         default: BTaggingFlags.OutputLevel

    input:             name: The name of the tool (should be unique).
      useBTagFlagsDefaults : Whether to use BTaggingFlags defaults for options that are not specified.
                  **options: Python dictionary with options for the tool.
    output: The actual tool, which can then by added to ToolSvc via ToolSvc += output."""
    if useBTagFlagsDefaults:
        if not 'InDetKeys' in dir():
            from InDetRecExample.InDetKeys import InDetKeys
        defaults = { 'OutputLevel'                         : BTaggingFlags.OutputLevel }
        for option in defaults:
            options.setdefault(option, defaults[option])
    options['name'] = name
    from InDetSecVxFinderTool.InDetSecVxFinderToolConf import InDet__InDetImprovedJetFitterVxFinder
    return InDet__InDetImprovedJetFitterVxFinder(**options)

#--------------------------------------------------------------------------

metaJetFitterTwoTrackVtxFinderTool = { 'DependsOn': ['JetFitterSequentialVertexFitter'],
                                       'PassByPointer': {'SequentialVertexFitter' : 'JetFitterSequentialVertexFitter'},
                                       'ToolCollection' : 'JetFitterCollection'}

def toolJetFitterTwoTrackVtxFinderTool(name , useBTagFlagsDefaults = True, **options):
    """Sets up the new JetFitter TwoTrack Vtx Finder Tool and returns it.

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
    from InDetSecVxFinderTool.InDetSecVxFinderToolConf import InDet__JetFitterTwoTrackVtxFinderTool
    return InDet__JetFitterTwoTrackVtxFinderTool(**options)

def toolJetFitterTwoTrackVtxFinderTool_SV(name, suffix="" , useBTagFlagsDefaults = True, **options):
    """Sets up the new JetFitter TwoTrack Vtx Finder Tool and returns it.

    OutputLevel                         default: BTaggingFlags.OutputLevel

    input:             name: The name of the tool (should be unique).
      useBTagFlagsDefaults : Whether to use BTaggingFlags defaults for options that are not specified.
                  **options: Python dictionary with options for the tool.
    output: The actual tool, which can then by added to ToolSvc via ToolSvc += output."""
    if useBTagFlagsDefaults:
        inDetJetFitterSequentialVertexFitter = toolJetFitterSequentialVertexFitter_SV("toolJetFitterSequentialVertexFitter"+suffix)
        defaults = { 'OutputLevel'            : BTaggingFlags.OutputLevel,
                     'SequentialVertexFitter' : inDetJetFitterSequentialVertexFitter }
        for option in defaults:
            options.setdefault(option, defaults[option])
    options['name'] = name + suffix
    from InDetSecVxFinderTool.InDetSecVxFinderToolConf import InDet__JetFitterTwoTrackVtxFinderTool
    return InDet__JetFitterTwoTrackVtxFinderTool(**options)


#--------------------------------------------------------------------------

metaJetFitterV0FinderTool = { 'DependsOn': ['JetFitterMode3dTo1dFinder','InDetJetFitterUtils'],
                              'PassByPointer': { 'Mode3dFinder' : 'JetFitterMode3dTo1dFinder',
                                                 'InDetJetFitterUtils' : 'InDetJetFitterUtils' },
                              'ToolCollection' : 'JetFitterCollection'}

def toolJetFitterV0FinderTool(name , useBTagFlagsDefaults = True, **options):
    """Sets up the new JetFitter V0 Finder Tool and returns it.

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
    from InDetSecVxFinderTool.InDetSecVxFinderToolConf import InDet__JetFitterV0FinderTool
    return InDet__JetFitterV0FinderTool(**options)

def toolJetFitterV0FinderTool_SV(name , suffix="", useBTagFlagsDefaults = True, **options):
    """Sets up the new JetFitter V0 Finder Tool and returns it.

    OutputLevel                         default: BTaggingFlags.OutputLevel

    input:             name: The name of the tool (should be unique).
      useBTagFlagsDefaults : Whether to use BTaggingFlags defaults for options that are not specified.
                  **options: Python dictionary with options for the tool.
    output: The actual tool, which can then by added to ToolSvc via ToolSvc += output."""
    if useBTagFlagsDefaults:
        inDetJetFitterUtils = toolInDetJetFitterUtils_SV('InDetJFUtils'+suffix)
        inDetMode3dTo1dFinder = toolJetFitterMode3dTo1dFinder('Mode3dTo1dFinder'+suffix)
        defaults = { 'OutputLevel'          : BTaggingFlags.OutputLevel,
                     'InDetJetFitterUtils'  : inDetJetFitterUtils,
                     'Mode3dFinder': inDetMode3dTo1dFinder }
        for option in defaults:
            options.setdefault(option, defaults[option])
    options['name'] = name + suffix
    from InDetSecVxFinderTool.InDetSecVxFinderToolConf import InDet__JetFitterV0FinderTool
    return InDet__JetFitterV0FinderTool(**options)


#--------------------------------------------------------------------------     

def toolNewJetFitterVxFinder(name, useBTagFlagsDefaults = True, **options):
    """Sets up a NewJetFitterVxFinder tool and returns it.

    The following options have BTaggingFlags defaults:

    OutputLevel                         default: BTaggingFlags.OutputLevel

    input:             name: The name of the tool (should be unique).
      useBTagFlagsDefaults : Whether to use BTaggingFlags defaults for options that are not specified.
                  **options: Python dictionary with options for the tool.
    output: The actual tool, which can then by added to ToolSvc via ToolSvc += output."""
    if useBTagFlagsDefaults:
        if not 'InDetKeys' in dir():
            from InDetRecExample.InDetKeys import InDetKeys
        defaults = { 'OutputLevel'                         : BTaggingFlags.OutputLevel }
        for option in defaults:
            options.setdefault(option, defaults[option])
    options['name'] = name
    from InDetSecVxFinderTool.InDetSecVxFinderToolConf import InDet__InDetImprovedJetFitterVxFinder
    return InDet__InDetImprovedJetFitterVxFinder(**options)

#--------------------------------------------------------------------------     

def toolNewJetFitterVxFinder_SV(name, suffix = "", useBTagFlagsDefaults = True, **options):
    """Sets up a NewJetFitterVxFinder tool and returns it.

    The following options have BTaggingFlags defaults:

    OutputLevel                         default: BTaggingFlags.OutputLevel
    VxPrimaryContainer                  default: BTaggingFlags.PrimaryVertexCollectionName
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
        inDetImprovedJetFitterTrackSelectorTool = toolJetFitterTrackSelectorTool_SV('InDetImprovedJFTrackSelTool'+suffix,suffix)
        inDetJetFitterTwoTrackVtxFinderTool = toolJetFitterTwoTrackVtxFinderTool_SV('inDetJetFitterTwoTrackVtxFinderTool'+suffix,suffix)
        inDetImprovedJFV0FinderTool = toolJetFitterV0FinderTool_SV('inDetImprovedJFV0FinderTool'+suffix,suffix)
        inDetJetFitterMultiStageFit = toolJetFitterMultiStageFit_SV('inDetJetFitterMultiStageFit'+suffix,suffix)
        defaults = { 'OutputLevel'                         : BTaggingFlags.OutputLevel,
                     'JetFitterTwoTrackVtxFinderTool'      : inDetJetFitterTwoTrackVtxFinderTool,
                     'JetFitterTrackSelectorTool'          : inDetImprovedJetFitterTrackSelectorTool, 
                     'JetFitterV0FinderTool'               : inDetImprovedJFV0FinderTool,
                     'JetFitterMultiStageFit'              : inDetJetFitterMultiStageFit
                     }
        for option in defaults:
            options.setdefault(option, defaults[option])
    options['name'] = name + suffix
    from InDetSecVxFinderTool.InDetSecVxFinderToolConf import InDet__InDetImprovedJetFitterVxFinder
    return InDet__InDetImprovedJetFitterVxFinder(**options)

#--------------------------------------------------------------------------  
