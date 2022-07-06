# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from TrkConfig.TrkNeuralNetworkUtilsConfig import NeuralNetworkToHistoToolCfg

# import the JetFitterNNTool configurable

def JetFitterNNToolCfg( flags, name = 'JetFitterNNTool', scheme='', CombinedIPNN = False, useBTagFlagsDefaults = True, **options ):
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

    if useBTagFlagsDefaults:
        if not CombinedIPNN:
            nnToHistoTool = acc.popToolsAndMerge(NeuralNetworkToHistoToolCfg(flags))
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

    acc.setPrivateTools(CompFactory.Analysis.JetFitterNNTool( **options))
 
    return acc

