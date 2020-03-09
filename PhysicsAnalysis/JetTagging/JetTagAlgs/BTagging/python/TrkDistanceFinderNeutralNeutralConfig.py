# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory

Trk__TrkDistanceFinderNeutralNeutral=CompFactory.Trk__TrkDistanceFinderNeutralNeutral

def TrkDistanceFinderNeutralNeutralCfg(name, **options):
    """Sets up a TrkDistanceFinderNeutralNeutral tool and returns it.

    The following options have BTaggingFlags defaults:

    input:             name: The name of the tool (should be unique).
                  **options: Python dictionary with options for the tool.
    output: The actual tool, which can then by added to ToolSvc via ToolSvc += output."""
    acc = ComponentAccumulator()
    options['name'] = name
    acc.setPrivateTools(Trk__TrkDistanceFinderNeutralNeutral(**options))

    return acc
