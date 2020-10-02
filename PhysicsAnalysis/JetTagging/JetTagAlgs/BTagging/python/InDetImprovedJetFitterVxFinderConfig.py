# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory

from BTagging.JetFitterMultiStageFitConfig import InDetJetFitterMultiStageFitCfg
from BTagging.JetFitterTrackSelectorToolConfig import InDetJetFitterTrackSelectorToolCfg 
from BTagging.JetFitterTwoTrackVtxFinderToolConfig import InDetJetFitterTwoTrackVtxFinderToolCfg
from BTagging.JetFitterV0FinderToolConfig import JetFitterV0FinderToolCfg

# import the InDetDetailedTrackSelectorTool configurable
InDet__InDetImprovedJetFitterVxFinder=CompFactory.InDet.InDetImprovedJetFitterVxFinder

def InDetImprovedJetFitterVxFinderCfg(flags, name, suffix = "", useBTagFlagsDefaults = True, **options):
    """Sets up a InDetJetFitterTrackSelectorTool  tool and returns it.

    The following options have BTaggingFlags defaults:
    
    input:             name: The name of the tool (should be unique).
      useBTagFlagsDefaults : Whether to use BTaggingFlags defaults for options that are not specified.
                  **options: Python dictionary with options for the tool.
    output: The actual tool, which can then by added to ToolSvc via ToolSvc += output."""
    acc = ComponentAccumulator()
    if useBTagFlagsDefaults:
        InDetJetFitterMultiStageFit = acc.popToolsAndMerge( InDetJetFitterMultiStageFitCfg(flags,"JetFitterMultiStageFit"+suffix,suffix) )
        InDetJetFitterTrackSelectorTool = acc.popToolsAndMerge( InDetJetFitterTrackSelectorToolCfg(flags,"JetFitterTrackSelectorTool"+suffix,suffix) )
        InDetJetFitterTwoTrackVtxFinderTool = acc.popToolsAndMerge( InDetJetFitterTwoTrackVtxFinderToolCfg("InDetJetFitterTwoTrackVtxFinderTool"+suffix,suffix) )
        InDetJetFitterV0FinderTool = acc.popToolsAndMerge( JetFitterV0FinderToolCfg(flags,"JetFitterV0FinderTool"+suffix,suffix) )
        defaults = { 'JetFitterTrackSelectorTool' : InDetJetFitterTrackSelectorTool ,
                     'JetFitterTwoTrackVtxFinderTool' : InDetJetFitterTwoTrackVtxFinderTool ,
                     'JetFitterV0FinderTool' : InDetJetFitterV0FinderTool ,
                     'JetFitterMultiStageFit' : InDetJetFitterMultiStageFit }
        for option in defaults:
            options.setdefault(option, defaults[option])

    options['name'] = name
    acc.setPrivateTools( InDet__InDetImprovedJetFitterVxFinder(**options) )
    return acc



