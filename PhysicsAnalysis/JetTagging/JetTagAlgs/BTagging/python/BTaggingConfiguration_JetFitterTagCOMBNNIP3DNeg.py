# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# Configuration functions for JetFitterTagCOMBNNIP3DNeg
# Author: Wouter van den Wollenberg (2013-2014)
from BTagging.BTaggingFlags import BTaggingFlags

metaJetFitterTagCOMBNNIP3DNeg = { 'IsATagger'         : True,
                                  'DependsOn'         : ['AtlasExtrapolator',
                                                         'BTagTrackToVertexTool',
                                                         'JetFitterTagNNFlip',
                                                         'IP3DNegTag',
                                                         'JetFitterCOMBNNTool',
                                                         'NeuralNetworkToHistoToolCOMBNN',
                                                         'JetFitterVariablesFactoryCOMBNNIP3DNeg',
                                                         'JetFitterNtupleWriterCOMBNN'],
                                  'PassByPointer'     : {'jetfitterVariablesFactory' : 'JetFitterVariablesFactoryCOMBNNIP3DNeg',
                                                         'jetfitterClassifier'       : 'JetFitterCOMBNNTool',
                                                         'jetfitterNtupleWriter'     : 'JetFitterNtupleWriterCOMBNN'},
                                  'PassByName'        : {'secVxFinderName' : 'NewJetFitterVxFinderFlip'},
                                  'JetCollectionList' : 'jetCollectionList',
                                  'ToolCollection'    : 'JetFitterTagCOMBNNIP3DNeg' }

def toolJetFitterTagCOMBNNIP3DNeg(name, useBTagFlagsDefaults = True, **options):
    """Sets up a JetFitterTagCOMBNNIP3DNeg tool and returns it.

    The following options have BTaggingFlags defaults:

    OutputLevel                         default: BTaggingFlags.OutputLevel
    Runmodus                            default: BTaggingFlags.Runmodus
    jetCollectionList                   default: BTaggingFlags.Jets
    secVxFinderName                     default: "NewJetFitterVxFinderFlip"
    useForcedCalibration                default: False
    storeOnlyBaseObject                 default: True
    useNN                               default: True
    useNNWithIP3D                       default: True
    IP3DinfoName                        default: "IP3DNeg"

    input:             name: The name of the tool (should be unique).
      useBTagFlagsDefaults : Whether to use BTaggingFlags defaults for options that are not specified.
                  **options: Python dictionary with options for the tool.
    output: The actual tool, which can then by added to ToolSvc via ToolSvc += output."""
    if useBTagFlagsDefaults:
        defaults = { 'OutputLevel'                      : BTaggingFlags.OutputLevel,
                     'Runmodus'                         : BTaggingFlags.Runmodus,
                     'jetCollectionList'                : BTaggingFlags.Jets,
                     'secVxFinderName'                  : 'NewJetFitterVxFinderFlip',
                     'useForcedCalibration'             : False,
                     'storeOnlyBaseObject'              : True,
                     'useNN'                            : True,
                     'useNNWithIP3D'                    : True,
                     'IP3DinfoName'                     : 'IP3DNeg' }
        for option in defaults:
            options.setdefault(option, defaults[option])
    options['name'] = name
    from JetTagTools.JetTagToolsConf import Analysis__JetFitterTag
    return Analysis__JetFitterTag(**options)

#------------------------------------------------------------------------------------

metaJetFitterVariablesFactoryCOMBNNIP3DNeg = { 'PassByName'     : {'JetFitterInstance' : 'JetFitterTagCOMBNNIP3DNeg'},
                                               'ToolCollection' : 'JetFitterTagCOMBNNIP3DNeg' }

def toolJetFitterVariablesFactoryCOMBNNIP3DNeg(name, useBTagFlagsDefaults = True, **options):
    """Sets up a JetFitterVariablesFactoryCOMBNNIP3DNeg tool and returns it.

    The following options have BTaggingFlags defaults:

    OutputLevel                         default: BTaggingFlags.OutputLevel
    JetFitterInstance                   default: "JetFitterTagCOMBNNIP3DNeg"
    usePtCorrectedMass                  default: True
    revertFromPositiveToNegativeTags    default: True

    input:             name: The name of the tool (should be unique).
      useBTagFlagsDefaults : Whether to use BTaggingFlags defaults for options that are not specified.
                  **options: Python dictionary with options for the tool.
    output: The actual tool, which can then by added to ToolSvc via ToolSvc += output."""
    if useBTagFlagsDefaults:
        defaults = { 'OutputLevel'                      : BTaggingFlags.OutputLevel,
                     'JetFitterInstance'                : 'JetFitterTagCOMBNNIP3DNeg',
                     'usePtCorrectedMass'               : True,
                     'revertFromPositiveToNegativeTags' : True }
        for option in defaults:
            options.setdefault(option, defaults[option])
    options['name'] = name
    from JetTagTools.JetTagToolsConf import Analysis__JetFitterVariablesFactory
    return Analysis__JetFitterVariablesFactory(**options)
