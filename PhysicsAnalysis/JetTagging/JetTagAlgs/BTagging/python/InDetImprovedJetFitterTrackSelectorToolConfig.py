# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory

# import the InDetDetailedTrackSelectorTool configurable
InDet__InDetDetailedTrackSelectorTool=CompFactory.InDet.InDetDetailedTrackSelectorTool


def InDetImprovedJetFitterTrackSelectorToolCfg(name, useBTagFlagsDefaults = True, **options):
    """Sets up a InDetImprovedJetFitterTrackSelectorTool tool and returns it.

    The following options have BTaggingFlags defaults:

    pTMin                               default: 500.0
    IPd0Max                             default: 7.0
    IPz0Max                             default: 10.0
    sigIPd0Max                          default: 0.35
    sigIPz0Max                          default: 2.5
    etaMax                              default: 9999.0
    useTrackSummaryInfo                 default: True
    nHitBLayer                          default: 0
    nHitPix                             default: 1
    nHitSct                             default: 4
    nHitSi                              default: 7
    nHitTrt                             default: 0
    useSharedHitInfo                    default: False
    useTrackQualityInfo                 default: True
    fitChi2OnNdfMax                     default: 3.5
    TrackSummaryTool                    default: None

    input:             name: The name of the tool (should be unique).
      useBTagFlagsDefaults : Whether to use BTaggingFlags defaults for options that are not specified.
                  **options: Python dictionary with options for the tool.
    output: The actual tool, which can then by added to ToolSvc via ToolSvc += output."""
    acc = ComponentAccumulator()
    if useBTagFlagsDefaults:
        defaults = { 'pTMin'                        : 500.0,
                     'IPd0Max'                      : 7.0,
                     'IPz0Max'                      : 10.0,
                     'sigIPd0Max'                   : 0.35,
                     'sigIPz0Max'                   : 2.5,
                     'etaMax'                       : 9999.0,
                     'useTrackSummaryInfo'          : True,
                     'nHitBLayer'                   : 0,
                     'nHitPix'                      : 1,
                     'nHitSct'                      : 4,
                     'nHitSi'                       : 7,
                     'nHitTrt'                      : 0,
                     'useSharedHitInfo'             : False,
                     'useTrackQualityInfo'          : True,
                     'fitChi2OnNdfMax'              : 3.5,
                     'TrackSummaryTool'             : '' }
        for option in defaults:
            options.setdefault(option, defaults[option])
    options['name'] = name
    acc.setPrivateTools(InDet__InDetDetailedTrackSelectorTool(**options))

    return acc
