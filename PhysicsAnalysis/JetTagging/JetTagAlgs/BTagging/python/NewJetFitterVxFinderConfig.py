# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from BTagging.InDetJetFitterUtilsConfig import InDetJetFitterUtilsCfg
from BTagging.JetFitterSequentialVertexFitterConfig import JetFitterSequentialVertexFitterCfg
from BTagging.VxInternalEdmFactoryConfig import VxInternalEdmFactoryCfg
from BTagging.ImprovedJetFitterRoutinesConfig import ImprovedJetFitterRoutinesCfg
from TrkConfig.AtlasExtrapolatorConfig import AtlasExtrapolatorCfg
from BTagging.JetFitterMode3dTo1dFinderConfig import JetFitterMode3dTo1dFinderCfg
from BTagging.InDetImprovedJetFitterTrackSelectorToolConfig import InDetImprovedJetFitterTrackSelectorToolCfg
from BTagging.ImprovedJetFitterInitializationHelperConfig import ImprovedJetFitterInitializationHelperCfg

# import the InDetImprovedJetFitterVxFinder configurable
InDet__InDetImprovedJetFitterVxFinder=CompFactory.InDet.InDetImprovedJetFitterVxFinder

# define the class
def NewJetFitterVxFinderCfg(flags, name = 'JFVxFinder', suffix = "", useBTagFlagsDefaults = True, options = {}):
    """Sets up a NewJetFitterVxFinder tool and returns it.

    The following options have BTaggingFlags defaults:

    VxPrimaryContainer                  default: BTaggingFlags.PrimaryVertexCollectionName
    MaxNumDeleteIterations              default: 30
    VertexProbCut                       default: 0.001
    MaxClusteringIterations             default: 30
    VertexClusteringProbabilityCut      default: 0.005

    input:             name: The name of the tool (should be unique).
         useBTagFlagsDefaults : Whether to use BTaggingFlags defaults for options that are not specified.
                  **options: Python dictionary with options for the tool.
    output: The actual tool."""
    acc = ComponentAccumulator()
    if useBTagFlagsDefaults:
        inDetJetFitterUtils = acc.popToolsAndMerge(InDetJetFitterUtilsCfg(flags, 'InDetJFUtils'+suffix))
        improvedJetFitterRoutines = acc.popToolsAndMerge(ImprovedJetFitterRoutinesCfg('ImprovedJFRoutines'+suffix))
        jetFitterMode3dTo1dFinder = acc.popToolsAndMerge(JetFitterMode3dTo1dFinderCfg('JFMode3dTo1dFinder'+suffix))
        inDetImprovedJetFitterTrackSelectorTool = acc.popToolsAndMerge(InDetImprovedJetFitterTrackSelectorToolCfg('InDetImprovedJFTrackSelTool'+suffix))
        jetFitterSequentialVertexFitter = acc.popToolsAndMerge(JetFitterSequentialVertexFitterCfg('JFSeqVxFitter'+suffix))
        jetFitterExtrapolator = acc.popToolsAndMerge(AtlasExtrapolatorCfg(flags, 'JFExtrapolator'+suffix))
        improvedJetFitterInitializationHelper = acc.popToolsAndMerge(ImprovedJetFitterInitializationHelperCfg('ImprovedJFInitHelper'+suffix))
        vertexEdmFactory = acc.popToolsAndMerge(VxInternalEdmFactoryCfg('VxInternalEdmFactory'+suffix))
        defaults = { 'VxPrimaryContainer'                  : flags.BTagging.PrimaryVertexCollectionName,
                     'MaxNumDeleteIterations'              : 30,
                     'VertexProbCut'                       : 0.001,
                     'MaxClusteringIterations'             : 30,
                     'VertexClusteringProbabilityCut'      : 0.005 ,
                     'VertexEdmFactory'                    : vertexEdmFactory,
                     'JetFitterInitializationHelper'       : improvedJetFitterInitializationHelper,
                     'InDetJetFitterUtils'                 : inDetJetFitterUtils,
                     'Extrapolator'                        : jetFitterExtrapolator,
                     'JetFitterRoutines'                   : improvedJetFitterRoutines,
                     'TrackSelector'                       : inDetImprovedJetFitterTrackSelectorTool,
                     'Mode3dFinder'                        : jetFitterMode3dTo1dFinder,
                     'SequentialVertexFitter'              : jetFitterSequentialVertexFitter}
        for option in defaults:
            options.setdefault(option, defaults[option])
    options['name'] = name + suffix
    acc.setPrivateTools(InDet__InDetImprovedJetFitterVxFinder(**options))

    return acc
