# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from BTagging.BTaggingFlags import BTaggingFlags

# import the MultiSVTag configurable

from JetTagTools.JetTagToolsConf import Analysis__MultiSVTag

def MultiSVTagCfg( name = 'MultiSVbb1Tag', taggerNameBase = 'MultiSVbb1', useBTagFlagsDefaults = True, **options):
    """Sets up a MultiSVTag tool and returns it.

    The following options have BTaggingFlags defaults:

    Runmodus                            default: BTaggingFlags.Runmodus
    taggerNameBase                      default: "MultiSVbb1"
    SecVxFinderName                     default: "MSV"

    input:             name: The name of the tool (should be unique).
          useBTagFlagsDefaults : Whether to use BTaggingFlags defaults for options that are not specified.
                  **options: Python dictionary with options for the tool.
    output: The actual tool"""
    acc = ComponentAccumulator()
    options['name'] = name
    options['xAODBaseName'] = 'MSV'
    if useBTagFlagsDefaults:
        defaults = { 'Runmodus'                         : BTaggingFlags.Runmodus,
                     'taggerNameBase'                   : taggerNameBase,
                     'SecVxFinderName'                  : 'MSV' }
        for option in defaults:
            options.setdefault(option, defaults[option])
    acc.setPrivateTools(Analysis__MultiSVTag( **options))

    return acc
