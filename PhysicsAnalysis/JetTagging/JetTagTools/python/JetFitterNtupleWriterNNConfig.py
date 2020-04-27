# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory

# import the JetFitterNtupleWriter configurable
Analysis__JetFitterNtupleWriter=CompFactory.Analysis.JetFitterNtupleWriter

def JetFitterNtupleWriterNNCfg( name = 'JetFitterNtupleWriterNN', useBTagFlagsDefaults = True, **options ):
    """Sets up a JetFitterTagNN tool and returns it.
    input:             name: The name of the tool (should be unique).
                  **options: Python dictionary with options for the tool.
    output: The actual tool."""
    acc = ComponentAccumulator()
    options['name'] = name
    acc.setPrivateTools(Analysis__JetFitterNtupleWriter( **options))

    return acc
