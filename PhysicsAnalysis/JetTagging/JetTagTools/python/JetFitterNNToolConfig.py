# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from JetTagTools.NeuralNetworkToHistoToolConfig import NeuralNetworkToHistoToolCfg

# import the JetFitterNNTool configurable
Analysis__JetFitterNNTool=CompFactory.Analysis.JetFitterNNTool

def JetFitterNNToolCfg( name = 'JetFitterNNTool', scheme='', CombinedIPNN = False, useBTagFlagsDefaults = True, **options ):
    """Sets up a JetFitterNNTool tool and returns it.

    The following options have BTaggingFlags defaults:

    CalibrationDirectory                default: "JetFitter"
    CalibrationSubDirectory             default: "NeuralNetwork"
    useCombinedIPNN                     default: False
    usePtCorrectedMass                  default: True

    input:             name: The name of the tool (should be unique).
          useBTagFlagsDefaults : Whether to use BTaggingFlags defaults for options that are not specified.
                  **options: Python dictionary with options for the tool.
    output: The actual tool."""
    acc = ComponentAccumulator()
    options['name'] = name
    if scheme == 'Trig':
        options['HistosKey'] = 'JetTagTrigCalibHistosKey'
    # you can force the NN tool off with this flag (avoids loading
    # old jetfitter nns which sometimes crash
    RunJetFitterNNTool = True

    if not RunJetFitterNNTool:
        raise ValueError("This case is not implemented because always True during Run2. Contact BTagging software team.")
    if useBTagFlagsDefaults:
        if not CombinedIPNN:
            nnToHistoTool = acc.popToolsAndMerge(NeuralNetworkToHistoToolCfg('NeuralNetworkToHistoToolNN'))
            defaults = { 'useCombinedIPNN'                  : False,
                     'CalibrationDirectory'             : 'JetFitter',
                     'CalibrationSubDirectory'          : 'NeuralNetwork',
                     'usePtCorrectedMass'               : True,
                     'NeuralNetworkToHistoTool'         : nnToHistoTool, }
        else:
            defaults = { 'CalibrationDirectory'             : 'JetFitter',
                     'CalibrationSubDirectory'          : 'NeuralNetwork',
                     'useCombinedIPNN'                  : True,
                     'usePtCorrectedMass'               : True }

        for option in defaults:
            options.setdefault(option, defaults[option])
    acc.setPrivateTools(Analysis__JetFitterNNTool( **options))
 
    return acc

