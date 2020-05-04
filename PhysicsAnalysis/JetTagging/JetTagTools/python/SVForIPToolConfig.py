# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory

# import the SVForIPTool configurable
Analysis__SVForIPTool=CompFactory.Analysis.SVForIPTool

def SVForIPToolCfg( name = 'SVForIPTool', **options ):
    """Sets up a SVForIPTool tool and returns it.

    input:    name: The name of the tool (should be unique).
                  **options: Python dictionary with options for the tool.
    output: The actual tool."""
    acc = ComponentAccumulator()
    options['name'] = name
    acc.setPrivateTools(Analysis__SVForIPTool( **options))

    return acc
