# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory

def MuonSelectorToolCfg( name = 'MuonSelectorTool', useBTagFlagsDefaults = True, **options ):
    """Sets up a MuonSelectorTool tool and returns it.
    The following options have BTaggingFlags defaults:

    MaxEta                              default: 2.7

    input:    name: The name of the tool (should be unique).
                  **options: Python dictionary with options for the tool.
    output: The actual tool."""
    acc = ComponentAccumulator()
    if useBTagFlagsDefaults:
        defaults = { 'MaxEta'                 : 2.7 ,
                   }
        for option in defaults:
            options.setdefault(option, defaults[option])
    options['name'] = name
    acc.setPrivateTools(CompFactory.CP.MuonSelectionTool( **options))

    return acc
