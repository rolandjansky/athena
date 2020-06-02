# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory

from BTagging.JetFitterFullLinearizedTrackFactoryConfig import JetFitterFullLinearizedTrackFactoryCfg
Trk__JetFitterInitializationHelper=CompFactory.Trk.JetFitterInitializationHelper

def ImprovedJetFitterInitializationHelperCfg(name, useBTagFlagsDefaults = True, **options):
    """Sets up a ImprovedJetFitterInitializationHelper tool and returns it.

    The following options have BTaggingFlags defaults:

    input:             name: The name of the tool (should be unique).
      useBTagFlagsDefaults : Whether to use BTaggingFlags defaults for options that are not specified.
                  **options: Python dictionary with options for the tool.
    output: The actual tool, which can then by added to ToolSvc via ToolSvc += output."""
    acc = ComponentAccumulator()
    if useBTagFlagsDefaults:
        jetFitterFullLinearizedTrackFactory = acc.popToolsAndMerge(JetFitterFullLinearizedTrackFactoryCfg('JFFullLinearizedTrackFactory'))
        defaults = {'LinearizedTrackFactory' : jetFitterFullLinearizedTrackFactory}
        for option in defaults:
            options.setdefault(option, defaults[option])
    options['name'] = name
    acc.setPrivateTools(Trk__JetFitterInitializationHelper(**options))

    return acc
