# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# Configuration functions for JetFitterTagNN
# Author: Wouter van den Wollenberg (2013-2014)
from BTagging.BTaggingFlags import BTaggingFlags

metaJetFitterTagNN = { 'IsATagger'         : True,
                       'DependsOn'         : ['AtlasExtrapolator',
                                              'BTagTrackToVertexTool',
                                              'NewJetFitterVxFinder',
                                              'JetFitterNNTool',
                                              'NeuralNetworkToHistoToolNN',
#                                              'JetFitterVariablesFactoryNN',
                                              'JetFitterNtupleWriterNN'],
                       'PassByPointer'     : {
#        'jetfitterVariablesFactory' : 'JetFitterVariablesFactoryNN',
        'jetfitterClassifier'       : 'JetFitterNNTool',
        'jetfitterNtupleWriter'     : 'JetFitterNtupleWriterNN' },
#                       'PassByName'        : {'secVxFinderName' : 'NewJetFitterVxFinder'},
                       'JetCollectionList' : 'jetCollectionList',
                       'ToolCollection'    : 'JetFitterTagNN' }

def toolJetFitterTagNN(name, useBTagFlagsDefaults = True, **options):
    """Sets up a JetFitterTagNN tool and returns it.

    The following options have BTaggingFlags defaults:

    OutputLevel                         default: BTaggingFlags.OutputLevel
    Runmodus                            default: BTaggingFlags.Runmodus
    jetCollectionList                   default: BTaggingFlags.Jets
    SecVxFinderName                     default: "JetFitter"
    useForcedCalibration                default: False
    ipinfoTaggerName                    default: ""
    xAODBaseName                        default: "JetFitter"

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
#                     'supplementalTaggers'              : [],
#                     'storeOnlyBaseObject'              : True,
                     'ipinfoTaggerName'                 : "",
                     'xAODBaseName'                     : 'JetFitter', }
#        if not BTaggingFlags.JetFitterTag:
#            defaults['storeOnlyBaseObject'] = False
        for option in defaults:
            options.setdefault(option, defaults[option])
    options['name'] = name
    from JetTagTools.JetTagToolsConf import Analysis__JetFitterTag
    return Analysis__JetFitterTag(**options)

#-------------------------------------------------------------------------

metaJetFitterNNTool = { 'CalibrationFolders' : ['JetFitter',],
                        'DependsOn'          : ['NeuralNetworkToHistoToolNN',
                                                'BTagCalibrationBrokerTool'],
                        'PassByPointer'      : {'calibrationTool'          : 'BTagCalibrationBrokerTool',
                                                'NeuralNetworkToHistoTool' : 'NeuralNetworkToHistoToolNN'},
                        'ToolCollection'     : 'JetFitterTagNN' }

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
        defaults = { 'OutputLevel'                      : BTaggingFlags.OutputLevel,
                     'CalibrationDirectory'             : 'JetFitter',
                     'CalibrationSubDirectory'          : 'NeuralNetwork',
                     'useCombinedIPNN'                  : False,
                     'usePtCorrectedMass'               : True, }
        for option in defaults:
            options.setdefault(option, defaults[option])
    options['name'] = name
    from JetTagTools.JetTagToolsConf import Analysis__JetFitterNNTool
    return Analysis__JetFitterNNTool(**options)

#-------------------------------------------------------------------------

metaNeuralNetworkToHistoToolNN = { 'ToolCollection' : 'JetFitterTagNN' }

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

metaJetFitterVariablesFactoryNN = { 'PassByName'     : {'JetFitterInstance' : 'JetFitterTagNN'},
                                    'ToolCollection' : 'JetFitterTagNN' }

def toolJetFitterVariablesFactoryNN(name, useBTagFlagsDefaults = True, **options):
    """Sets up a JetFitterVariablesFactoryNN tool and returns it.

    The following options have BTaggingFlags defaults:

    OutputLevel                         default: BTaggingFlags.OutputLevel
    JetFitterInstance                   default: "JetFitterTagNN"
    usePtCorrectedMass                  default: True

    input:             name: The name of the tool (should be unique).
      useBTagFlagsDefaults : Whether to use BTaggingFlags defaults for options that are not specified.
                  **options: Python dictionary with options for the tool.
    output: The actual tool, which can then by added to ToolSvc via ToolSvc += output."""
    if useBTagFlagsDefaults:
        defaults = { 'OutputLevel'            : BTaggingFlags.OutputLevel,
                     'JetFitterInstance'      : 'JetFitterTagNN',
                     'usePtCorrectedMass'     : True }
        for option in defaults:
            options.setdefault(option, defaults[option])
    options['name'] = name
    from JetTagTools.JetTagToolsConf import Analysis__JetFitterVariablesFactory
    return Analysis__JetFitterVariablesFactory(**options)

#-------------------------------------------------------------------------

metaJetFitterNtupleWriterNN = { 'ToolCollection' : 'JetFitterTagNN' }

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
