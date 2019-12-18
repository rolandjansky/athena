# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from TrkExTools.AtlasExtrapolatorConfig import AtlasExtrapolatorCfg

# import the FullLinearizedTrackFactory configurable
Trk__FullLinearizedTrackFactory=CompFactory.Trk__FullLinearizedTrackFactory

def BTagFullLinearizedTrackFactoryCfg(flags, name = 'FullLinearizedTrackFactory', useBTagFlagsDefaults = True, **options ):
    """Sets up a BTagFullLinearizedTrackFactory tool and returns it.

    input:             name: The name of the tool (should be unique).
          useBTagFlagsDefaults : Whether to use BTaggingFlags defaults for options that are not specified.
                             Note however that this tool has no BTaggingFlags defaults; the option is
                             here only for consistency.
                  **options: Python dictionary with options for the tool.
    output: The actual tool, which can then be added to ToolSvc via ToolSvc += output."""
    acc = ComponentAccumulator()
    if useBTagFlagsDefaults:
        atlasExtrapolator= acc.popToolsAndMerge(AtlasExtrapolatorCfg(flags, 'AtlasExtrapolator'))
        defaults = { 'Extrapolator'            : atlasExtrapolator}
        for option in defaults:
            options.setdefault(option, defaults[option])

    options['name'] = name
    acc.setPrivateTools(Trk__FullLinearizedTrackFactory( **options))

    return acc
