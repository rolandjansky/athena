# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from JetTagTools.BTagFullLinearizedTrackFactoryConfig import BTagFullLinearizedTrackFactoryCfg
from AthenaConfiguration.ComponentFactory import CompFactory
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from TrkConfig.AtlasExtrapolatorConfig import AtlasExtrapolatorCfg

# importi the FullLinearizedTrackFactory configurable
InDet__InDetJetFitterUtils=CompFactory.InDet.InDetJetFitterUtils


def InDetJetFitterUtilsCfg(flags, name = 'InDetJFUtils', useBTagFlagsDefaults = True, options = {}):
    """Sets up a InDetJetFitterUtils tool and returns it.

    The following options have BTaggingFlags defaults:

    input:             name: The name of the tool (should be unique).
          useBTagFlagsDefaults : Whether to use BTaggingFlags defaults for options that are not specified.
                  **options: Python dictionary with options for the tool.
    output: The actual tool."""
    acc = ComponentAccumulator()
    if useBTagFlagsDefaults:
        jetFitterFullLinearizedTrackFactory = acc.popToolsAndMerge(BTagFullLinearizedTrackFactoryCfg(flags, 'JFFullLinearizedTrackFactory')) 
        jetFitterExtrapolator= acc.popToolsAndMerge(AtlasExtrapolatorCfg(flags, 'JFExtrapolator'))
        defaults = { 'LinearizedTrackFactory': jetFitterFullLinearizedTrackFactory,
                     'Extrapolator' : jetFitterExtrapolator }
        for option in defaults:
            options.setdefault(option, defaults[option])
    options['name'] = name
    acc.setPrivateTools(InDet__InDetJetFitterUtils(**options))

    return acc
