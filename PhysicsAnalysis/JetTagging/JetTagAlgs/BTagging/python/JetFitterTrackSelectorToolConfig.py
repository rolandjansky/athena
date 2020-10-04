# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory

from BTagging.InDetJetFitterUtilsConfig import InDetJetFitterUtilsCfg
from BTagging.InDetImprovedJetFitterTrackSelectorToolConfig import InDetImprovedJetFitterTrackSelectorToolCfg
from TrkConfig.AtlasExtrapolatorConfig import AtlasExtrapolatorCfg

# import the InDetDetailedTrackSelectorTool configurable
InDet__JetFitterTrackSelectorTool=CompFactory.InDet.JetFitterTrackSelectorTool

def InDetJetFitterTrackSelectorToolCfg(flags, name, suffix="", useBTagFlagsDefaults = True, **options):
    """Sets up a InDetJetFitterTrackSelectorTool  tool and returns it.

    The following options have BTaggingFlags defaults:
    
    revertFromPositiveToNegativeTags          default: False
    cutCompPrimaryVertexForPosLifetimeTracks  default: 1e-1
    cutCompPrimaryVertexForNegLifetimeTracks  default: 5e-2

    input:             name: The name of the tool (should be unique).
      useBTagFlagsDefaults : Whether to use BTaggingFlags defaults for options that are not specified.
                  **options: Python dictionary with options for the tool.
    output: The actual tool, which can then by added to ToolSvc via ToolSvc += output."""
    acc = ComponentAccumulator()
    if useBTagFlagsDefaults:
        jetFitterExtrapolator= acc.popToolsAndMerge(AtlasExtrapolatorCfg(flags,'JFExtrapolator'+suffix))
        inDetJetFitterUtils = acc.popToolsAndMerge(InDetJetFitterUtilsCfg(flags,'InDetJFUtils'+suffix))
        inDetImprovedJetFitterTrackSelectorTool = acc.popToolsAndMerge(InDetImprovedJetFitterTrackSelectorToolCfg('InDetImprovedJFTrackSelTool'+suffix))
        defaults = { 'revertFromPositiveToNegativeTags' : False,
                     'cutCompPrimaryVertexForPosLifetimeTracks' : 0.1,
                     'cutCompPrimaryVertexForNegLifetimeTracks' : 0.05,
                     'Extrapolator' : jetFitterExtrapolator ,
                     'InDetJetFitterUtils' : inDetJetFitterUtils,
                     'TrackSelector' : inDetImprovedJetFitterTrackSelectorTool }
        for option in defaults:
            options.setdefault(option, defaults[option])
            
    options['name'] = name
    acc.setPrivateTools( InDet__JetFitterTrackSelectorTool(**options) )
    return acc



