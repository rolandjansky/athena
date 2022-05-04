# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory

def TrkDistanceFinderNeutralChargedCfg(name, **options):
    """Sets up a TrkDistanceFinderNeutralCharged tool and returns it.

    The following options have BTaggingFlags defaults:

    input:             name: The name of the tool (should be unique).
                  **options: Python dictionary with options for the tool.
    output: The actual tool, which can then by added to ToolSvc via ToolSvc += output."""
    acc = ComponentAccumulator()
    options['name'] = name
    acc.setPrivateTools(CompFactory.Trk.TrkDistanceFinderNeutralCharged(**options))

    return acc
