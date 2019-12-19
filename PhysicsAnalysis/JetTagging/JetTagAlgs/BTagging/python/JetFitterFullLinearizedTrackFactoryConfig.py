# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory

Trk__FullLinearizedTrackFactory=CompFactory.Trk__FullLinearizedTrackFactory

def JetFitterFullLinearizedTrackFactoryCfg(name, useBTagFlagsDefaults = True, **options):
    """Sets up a JetFitterFullLinearizedTrackFactory tool and returns it.

    input:             name: The name of the tool (should be unique).
      useBTagFlagsDefaults : Whether to use BTaggingFlags defaults for options that are not specified.
                             Note however that this tool has no BTaggingFlags defaults; the option is
                             here only for consistency.
                  **options: Python dictionary with options for the tool.
    output: The actual tool, which can then by added to ToolSvc via ToolSvc += output."""
    acc = ComponentAccumulator()
    options['name'] = name
    acc.setPrivateTools(Trk__FullLinearizedTrackFactory(**options))

    return acc
