# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from BTagging.BTaggingFlags import BTaggingFlags

# import the DL1Tag configurable
from JetTagTools.JetTagToolsConf import Analysis__DL1Tag

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
    options['xAODBaseName'] = basename
    options['LocalNNConfigurationFile'] = BTaggingFlags.DL1LocalNNConfig
 
    if useBTagFlagsDefaults:
        defaults = { 'Runmodus'                         : flags.BTagging.RunModus,
                     'forceDL1CalibrationAlias'         : BTaggingFlags.ForceDL1CalibrationAlias,
                     'DL1CalibAlias'                    : BTaggingFlags.DL1CalibAlias,
                     'calibration_directory'            : basename,
                     }
        for option in defaults:
                options.setdefault(option, defaults[option])
    acc.setPrivateTools(Analysis__DL1Tag(**options))

    return acc
