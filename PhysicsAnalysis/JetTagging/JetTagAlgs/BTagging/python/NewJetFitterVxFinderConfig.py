# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from BTagging.BTaggingFlags import BTaggingFlags
from BTagging.InDetJetFitterUtilsConfig import InDetJetFitterUtilsCfg
from BTagging.JetFitterSequentialVertexFitterConfig import JetFitterSequentialVertexFitterCfg
from BTagging.VxInternalEdmFactoryConfig import VxInternalEdmFactoryCfg
from BTagging.ImprovedJetFitterRoutinesConfig import ImprovedJetFitterRoutinesCfg
from TrkExTools.AtlasExtrapolatorConfig import AtlasExtrapolatorCfg
from BTagging.JetFitterMode3dTo1dFinderConfig import JetFitterMode3dTo1dFinderCfg
from BTagging.InDetImprovedJetFitterTrackSelectorToolConfig import InDetImprovedJetFitterTrackSelectorToolCfg
from BTagging.ImprovedJetFitterInitializationHelperConfig import ImprovedJetFitterInitializationHelperCfg

# import the InDetImprovedJetFitterVxFinder configurable
from InDetSecVxFinderTool.InDetSecVxFinderToolConf import InDet__InDetImprovedJetFitterVxFinder

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
        if not 'InDetKeys' in dir():
            from InDetRecExample.InDetKeys import InDetKeys
        accInDetJetFitterUtils = InDetJetFitterUtilsCfg(flags, 'InDetJFUtils'+suffix)
        inDetJetFitterUtils = accInDetJetFitterUtils.popPrivateTools()
        acc.merge(accInDetJetFitterUtils)
        accImprovedJetFitterRoutines = ImprovedJetFitterRoutinesCfg('ImprovedJFRoutines'+suffix)
        improvedJetFitterRoutines = accImprovedJetFitterRoutines.popPrivateTools()
        acc.merge(accImprovedJetFitterRoutines)
        jetFitterMode3dTo1dFinder = acc.popToolsAndMerge(JetFitterMode3dTo1dFinderCfg('JFMode3dTo1dFinder'+suffix))
        inDetImprovedJetFitterTrackSelectorTool = acc.popToolsAndMerge(InDetImprovedJetFitterTrackSelectorToolCfg('InDetImprovedJFTrackSelTool'+suffix))
        accJetFitterSequentialVertexFitter = JetFitterSequentialVertexFitterCfg('JFSeqVxFitter'+suffix)
        jetFitterSequentialVertexFitter = accJetFitterSequentialVertexFitter.popPrivateTools()
        acc.merge(accJetFitterSequentialVertexFitter)
        accExtrapolator = AtlasExtrapolatorCfg(flags, 'JFExtrapolator'+suffix)
        jetFitterExtrapolator = accExtrapolator.popPrivateTools()
        acc.merge(accExtrapolator)
        improvedJetFitterInitializationHelper = acc.popToolsAndMerge(ImprovedJetFitterInitializationHelperCfg('ImprovedJFInitHelper'+suffix))
        accVxInternalEdmFactory = VxInternalEdmFactoryCfg('VxInternalEdmFactory'+suffix)
        vertexEdmFactory = accVxInternalEdmFactory.popPrivateTools()
        acc.merge(accVxInternalEdmFactory)
        defaults = { 'VxPrimaryContainer'                  : BTaggingFlags.PrimaryVertexCollectionName,
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
