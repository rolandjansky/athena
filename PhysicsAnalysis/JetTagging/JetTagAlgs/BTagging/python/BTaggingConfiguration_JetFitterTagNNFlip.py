# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# Configuration functions for JetFitterTagNNFlip
# Author: Wouter van den Wollenberg (2013-2014)
from BTagging.BTaggingFlags import BTaggingFlags

metaJetFitterTagNNFlip = { 'IsATagger'         : True,
                           'DependsOn'         : ['AtlasExtrapolator',
                                                  'BTagTrackToVertexTool',
                                                  'NewJetFitterVxFinderFlip',
                                                  'JetFitterNNTool',
                                                  'NeuralNetworkToHistoToolNN',
                                                  'JetFitterVariablesFactoryNNFlip',
                                                  'JetFitterNtupleWriterNN'],
                           'PassByPointer'     : {'jetfitterVariablesFactory' : 'JetFitterVariablesFactoryNNFlip',
                                                  'jetfitterClassifier'       : 'JetFitterNNTool',
                                                  'jetfitterNtupleWriter'     : 'JetFitterNtupleWriterNN'},
                           'PassByName'        : {'secVxFinderName' : 'NewJetFitterVxFinderFlip'},
                           'JetCollectionList' : 'jetCollectionList',
                           'ToolCollection'    : 'JetFitterTagNNFlip' }

def toolJetFitterTagNNFlip(name, useBTagFlagsDefaults = True, **options):
    """Sets up a JetFitterTagNNFlip tool and returns it.

    The following options have BTaggingFlags defaults:

    OutputLevel                         default: BTaggingFlags.OutputLevel
    Runmodus                            default: BTaggingFlags.Runmodus
    jetCollectionList                   default: BTaggingFlags.Jets
    secVxFinderName                     default: "NewJetFitterVxFinderFlip"
    useForcedCalibration                default: False
    storeOnlyBaseObject                 default: True (False if not BTaggingFlags.JetFitterTag)
    useNN                               default: True
    useNNWithIP3D                       default: False

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
                     'useNNWithIP3D'                    : False }
        if not BTaggingFlags.JetFitterTag:
            defaults['storeOnlyBaseObject'] = False
        for option in defaults:
            options.setdefault(option, defaults[option])
    options['name'] = name
    from JetTagTools.JetTagToolsConf import Analysis__JetFitterTag
    return Analysis__JetFitterTag(**options)

#-----------------------------------------------------------------------------

metaJetFitterVariablesFactoryNNFlip = { 'PassByName'     : {'JetFitterInstance' : 'JetFitterTagNNFlip'},
                                        'ToolCollection' : 'JetFitterTagNNFlip' }

def toolJetFitterVariablesFactoryNNFlip(name, useBTagFlagsDefaults = True, **options):
    """Sets up a JetFitterVariablesFactoryNNFlip tool and returns it.

    The following options have BTaggingFlags defaults:

    OutputLevel                         default: BTaggingFlags.OutputLevel
    JetFitterInstance                   default: "JetFitterTagNNFlip"
    usePtCorrectedMass                  default: True
    revertFromPositiveToNegativeTags    default: True

    input:             name: The name of the tool (should be unique).
      useBTagFlagsDefaults : Whether to use BTaggingFlags defaults for options that are not specified.
                  **options: Python dictionary with options for the tool.
    output: The actual tool, which can then by added to ToolSvc via ToolSvc += output."""
    if useBTagFlagsDefaults:
        defaults = { 'OutputLevel'                      : BTaggingFlags.OutputLevel,
                     'JetFitterInstance'                : 'JetFitterTagNNFlip',
                     'usePtCorrectedMass'               : True,
                     'revertFromPositiveToNegativeTags' : True }
        for option in defaults:
            options.setdefault(option, defaults[option])
    options['name'] = name
    from JetTagTools.JetTagToolsConf import Analysis__JetFitterVariablesFactory
    return Analysis__JetFitterVariablesFactory(**options)
