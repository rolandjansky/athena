# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory

from BTagging.ImprovedJetFitterInitializationHelperConfig import ImprovedJetFitterInitializationHelperCfg
from BTagging.ImprovedJetFitterRoutinesConfig import ImprovedJetFitterRoutinesCfg
from BTagging.InDetImprovedJetFitterTrackSelectorToolConfig import InDetImprovedJetFitterTrackSelectorToolCfg

# import the InDetDetailedTrackSelectorTool configurable
InDet__InDetJetFitterVxFinder=CompFactory.InDet.InDetJetFitterVxFinder

def InDetInDetJetFitterVxFinderCfg(name, suffix="", useBTagFlagsDefaults = True, **options):
    """Sets up a InDetJetFitterVxFinder  tool and returns it.

    The following options have BTaggingFlags defaults:
    
    MaxNumDeleteIterations                  default: 30
    VertexProbCut                           default: 0.001
    MaxClusteringIterations                 default: 30
    VertexClusteringProbabilityCut          default: 0.005
    UseFastClustering                       default: False
    MaxTracksToFitAtOnce                    default: 15

    input:             name: The name of the tool (should be unique).
      useBTagFlagsDefaults : Whether to use BTaggingFlags defaults for options that are not specified.
                  **options: Python dictionary with options for the tool.
    output: The actual tool, which can then by added to ToolSvc via ToolSvc += output."""
    acc = ComponentAccumulator()
    if useBTagFlagsDefaults:
        improvedJetFitterInitializationHelper = acc.popToolsAndMerge(ImprovedJetFitterInitializationHelperCfg('ImprovedJFInitHelper'+suffix))
        improvedJetFitterRoutines = acc.popToolsAndMerge(ImprovedJetFitterRoutinesCfg('ImprovedJFRoutines'+suffix))
        inDetImprovedJetFitterTrackSelectorTool = acc.popToolsAndMerge(InDetImprovedJetFitterTrackSelectorToolCfg('InDetImprovedJFTrackSelTool'+suffix))
        defaults = { 'MaxNumDeleteIterations'              : 30,
                     'VertexProbCut'                       : 0.001,
                     'MaxClusteringIterations'             : 30,
                     'VertexClusteringProbabilityCut'      : 0.005 ,
                     'UseFastClustering'                   : False,
                     'MaxTracksToFitAtOnce' : 15,
                     'JetFitterInitializationHelper' : improvedJetFitterInitializationHelper,
#                     'JetFitterHelper' : ,
                     'JetFitterRoutines' : improvedJetFitterRoutines,
                     'TrackSelector' : inDetImprovedJetFitterTrackSelectorTool }

        for option in defaults:
            options.setdefault(option, defaults[option])

    options['name'] = name
    acc.setPrivateTools( InDet__InDetJetFitterVxFinder(**options) )
    return acc



