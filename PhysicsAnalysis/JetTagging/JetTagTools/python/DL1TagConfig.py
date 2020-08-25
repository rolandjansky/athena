# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory

# import the DL1Tag configurable
Analysis__DL1Tag=CompFactory.Analysis.DL1Tag

# define the class
def DL1TagCfg(flags, name = 'DL1', scheme = '', useBTagFlagsDefaults = True, **options):
    """Sets up a DL1Tag tool and returns it.

    The following options have BTaggingFlags defaults:

    Runmodus                            default: BTagging.RunModus
    taggerName                          default: "DL1"
    taggerNameBase                      default: "DL1"
    forceDL1CalibrationAlias            default: BTaggingFlags.ForceDL1CalibrationAlias
    DL1CalibAlias                       default: BTaggingFlags.DL1CalibAlias

    input:             name: The name of the tool (should be unique).
          useBTagFlagsDefaults : Whether to use BTaggingFlags defaults for options that are not specified.
                  **options: Python dictionary with options for the tool.
    output: The actual tool"""
    acc = ComponentAccumulator()
    options['name'] = name + 'Tag'
    basename = name
    DL1LocalNNConfig = ''
    ForceDL1CalibrationAlias = True
    DL1CalibAlias = 'AntiKt4EMTopo'
    options['xAODBaseName'] = basename
    options['LocalNNConfigurationFile'] = DL1LocalNNConfig
    if scheme == 'Trig':
        options['HistosKey'] = 'JetTagTrigCalibHistosKey'
 
    if useBTagFlagsDefaults:
        defaults = { 'Runmodus'                         : flags.BTagging.RunModus,
                     'forceDL1CalibrationAlias'         : ForceDL1CalibrationAlias,
                     'DL1CalibAlias'                    : DL1CalibAlias,
                     'calibration_directory'            : basename,
                     }
        for option in defaults:
                options.setdefault(option, defaults[option])
    acc.setPrivateTools(Analysis__DL1Tag(**options))

    return acc
