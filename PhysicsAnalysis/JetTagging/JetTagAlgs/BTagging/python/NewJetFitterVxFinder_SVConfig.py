# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from BTagging.BTaggingFlags import BTaggingFlags
from BTagging.InDetJetFitterUtils_SVConfig import InDetJetFitterUtils_SVCfg
from TrkExTools.AtlasExtrapolatorConfig import AtlasExtrapolatorCfg
from BTagging.BTaggingConfiguration_NewJetFitterCollection import toolImprovedJetFitterRoutines, toolJetFitterMode3dTo1dFinder, toolInDetImprovedJetFitterTrackSelectorTool, toolJetFitterSequentialVertexFitter_SV, toolImprovedJetFitterInitializationHelper, toolVxInternalEdmFactory_SV


# importi the InDetImprovedJetFitterVxFinder configurable
from InDetSecVxFinderTool.InDetSecVxFinderToolConf import InDet__InDetImprovedJetFitterVxFinder

# define the class
class NewJetFitterVxFinder_SVCfg( InDet__InDetImprovedJetFitterVxFinder ):
     # constructor
    def __init__(self, name = 'JFVxFinder', suffix = "", useBTagFlagsDefaults = True, options = {}):
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
        if useBTagFlagsDefaults:
            if not 'InDetKeys' in dir():
                from InDetRecExample.InDetKeys import InDetKeys
            inDetJetFitterUtils = InDetJetFitterUtils_SVCfg('InDetJFUtils'+suffix)
            improvedJetFitterRoutines = toolImprovedJetFitterRoutines('ImprovedJFRoutines'+suffix)
            jetFitterMode3dTo1dFinder = toolJetFitterMode3dTo1dFinder('JFMode3dTo1dFinder'+suffix)
            inDetImprovedJetFitterTrackSelectorTool = toolInDetImprovedJetFitterTrackSelectorTool('InDetImprovedJFTrackSelTool'+suffix)
            jetFitterSequentialVertexFitter = toolJetFitterSequentialVertexFitter_SV('JFSeqVxFitter'+suffix)
            jetFitterExtrapolator = AtlasExtrapolatorCfg('JFExtrapolator'+suffix)
            improvedJetFitterInitializationHelper = toolImprovedJetFitterInitializationHelper('ImprovedJFInitHelper'+suffix)
            vertexEdmFactory = toolVxInternalEdmFactory_SV('VxInternalEdmFactory'+suffix)
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
        InDet__InDetImprovedJetFitterVxFinder.__init__(self, **options)
