# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from TrkConfig.AtlasExtrapolatorConfig import AtlasExtrapolatorCfg

Trk__FullLinearizedTrackFactory=CompFactory.Trk.FullLinearizedTrackFactory


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

    # @TODO migrate to run3 style job options
    if 'Extrapolator' not in options :
        # @TODO the ConfigFlags should be passed from the outside as argument
        from AthenaConfiguration.AllConfigFlags import ConfigFlags
        flags=ConfigFlags
        Extrapolator_acc = AtlasExtrapolatorCfg(flags)
        extrapolator=acc.popToolsAndMerge(Extrapolator_acc)
        acc.addPublicTool(extrapolator)
        options.setdefault('Extrapolator', extrapolator)

    acc.setPrivateTools(Trk__FullLinearizedTrackFactory(**options))

    return acc
