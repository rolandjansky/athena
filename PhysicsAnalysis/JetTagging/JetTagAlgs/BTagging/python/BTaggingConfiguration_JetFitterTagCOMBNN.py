# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# Configuration functions for JetFitterTagCOMBNN
# Author: Wouter van den Wollenberg (2013-2014)
from BTagging.BTaggingFlags import BTaggingFlags

metaJetFitterTagCOMBNN = { 'IsATagger'         : True,
                           'DependsOn'         : ['AtlasExtrapolator',
                                                  'BTagTrackToVertexTool',
                                                  'JetFitterTagNN',
                                                  'IP3DTag',
                                                  'InDetVKalVxInJetTool',
                                                  'BTagFullLinearizedTrackFactory',
                                                  'BTagTrackToVertexIPEstimator',
                                                  'JetFitterCOMBNNTool',
                                                  'NeuralNetworkToHistoToolCOMBNN',
#                                                  'JetFitterVariablesFactoryCOMBNN',
                                                  'JetFitterNtupleWriterCOMBNN'],
                           'PassByPointer'     : {
#        'jetfitterVariablesFactory' : 'JetFitterVariablesFactoryCOMBNN',
        'jetfitterClassifier'       : 'JetFitterCOMBNNTool',
        'jetfitterNtupleWriter'     : 'JetFitterNtupleWriterCOMBNN'},
#                           'PassByName'        : {'secVxFinderName' : 'NewJetFitterVxFinder' },
                           'JetCollectionList' : 'jetCollectionList',
                           'ToolCollection'    : 'JetFitterTagCOMBNN' }

def toolJetFitterTagCOMBNN(name, useBTagFlagsDefaults = True, **options):
    """Sets up a JetFitterTagCOMBNN tool and returns it.

    The following options have BTaggingFlags defaults:

    OutputLevel                         default: BTaggingFlags.OutputLevel
    Runmodus                            default: BTaggingFlags.Runmodus
    jetCollectionList                   default: BTaggingFlags.Jets
    SecVxFinderName                     default: "JetFitter"
    useForcedCalibration                default: False
    xAODBaseName                        default: "JetFitterCombNN"
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
#                     'supplementalTaggers'              : ['IP3D'],
#                     'storeOnlyBaseObject'              : True }
                     'xAODBaseName'                     : 'JetFitterCombNN',
                     'ipinfoTaggerName'                 : 'IP3D',
                     }
        for option in defaults:
            options.setdefault(option, defaults[option])
    options['name'] = name
    from JetTagTools.JetTagToolsConf import Analysis__JetFitterTag
    return Analysis__JetFitterTag(**options)

#-----------------------------------------------------------------------------

metaJetFitterCOMBNNTool = { 'CalibrationFolders' : ['JetFitter',],
                            'DependsOn'          : ['NeuralNetworkToHistoToolCOMBNN',
                                                    'BTagCalibrationBrokerTool'],
                            'PassByPointer'      : {'calibrationTool'          : 'BTagCalibrationBrokerTool',
                                                    'NeuralNetworkToHistoTool' : 'NeuralNetworkToHistoToolCOMBNN'},
                            'ToolCollection'     : 'JetFitterTagCOMBNN' }

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
    return Analysis__JetFitterNNTool(**options)

#-----------------------------------------------------------------------------

metaNeuralNetworkToHistoToolCOMBNN = { 'ToolCollection' : 'JetFitterTagCOMBNN' }

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

metaJetFitterVariablesFactoryCOMBNN = { 'PassByName'     : {'JetFitterInstance' : 'JetFitterTagCOMBNN'},
                                        'ToolCollection' : 'JetFitterTagCOMBNN' }

def toolJetFitterVariablesFactoryCOMBNN(name, useBTagFlagsDefaults = True, **options):
    """Sets up a JetFitterVariablesFactoryCOMBNN tool and returns it.

    The following options have BTaggingFlags defaults:

    OutputLevel                         default: BTaggingFlags.OutputLevel
    JetFitterInstance                   default: "JetFitterTagCOMBNN"
    usePtCorrectedMass                  default: True

    input:             name: The name of the tool (should be unique).
      useBTagFlagsDefaults : Whether to use BTaggingFlags defaults for options that are not specified.
                  **options: Python dictionary with options for the tool.
    output: The actual tool, which can then by added to ToolSvc via ToolSvc += output."""
    if useBTagFlagsDefaults:
        defaults = { 'OutputLevel'            : BTaggingFlags.OutputLevel,
                     'JetFitterInstance'      : 'JetFitterTagCOMBNN',
                     'usePtCorrectedMass'     : True }
        for option in defaults:
            options.setdefault(option, defaults[option])
    options['name'] = name
    from JetTagTools.JetTagToolsConf import Analysis__JetFitterVariablesFactory
    return Analysis__JetFitterVariablesFactory(**options)

#-----------------------------------------------------------------------------

metaJetFitterNtupleWriterCOMBNN = { 'ToolCollection' : 'JetFitterTagCOMBNN' }

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

