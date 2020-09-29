# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory

from BTagging.InDetJetFitterUtilsConfig import InDetJetFitterUtilsCfg
from BTagging.ImprovedJetFitterInitializationHelperConfig import ImprovedJetFitterInitializationHelperCfg
from BTagging.ImprovedJetFitterRoutinesConfig import ImprovedJetFitterRoutinesCfg

# import the InDetDetailedTrackSelectorTool configurable
InDet__JetFitterMultiStageFit = CompFactory.InDet.JetFitterMultiStageFit

def InDetJetFitterMultiStageFitCfg(flags, name, suffix="", useBTagFlagsDefaults = True, **options):
    """Sets up a JetFitterMultiStageFit  tool and returns it.

    The following options have BTaggingFlags defaults:
    
    MaxNumDeleteIterations                     default:30
    VertexProbCut                              default: 0.001
    MaxClusteringIterations                   default: 30
    UseFastClustering                          default: False
    maxTracksForDetailedClustering             default:25
    VertexClusteringProbabilityCut             default:0.005
    VertexClusteringProbabilityCutWithMass     default:0.05
    VertexClusteringProbabilityCutWithMass0010 default:0.002
    VertexClusteringProbabilityCutWithMass1015 default:0.002
    VertexClusteringProbabilityCutWithMass1520 default:0.050
    VertexClusteringProbabilityCutWithMass2025 default: 0.100
    VertexClusteringProbabilityCutWithMass2530 default:0.200
    VertexClusteringProbabilityCutWithMass3040 default:0.500
    VertexClusteringProbabilityCutWithMass4050 default:0.700
    VertexClusteringProbabilityCutWithMass5060 default:0.900
    VertexClusteringProbabilityCutWithMass6070 default:0.900

    input:             name: The name of the tool (should be unique).
      useBTagFlagsDefaults : Whether to use BTaggingFlags defaults for options that are not specified.
                  **options: Python dictionary with options for the tool.
    output: The actual tool, which can then by added to ToolSvc via ToolSvc += output."""

    acc = ComponentAccumulator()
    if useBTagFlagsDefaults:
        improvedJetFitterRoutines = acc.popToolsAndMerge(ImprovedJetFitterRoutinesCfg('ImprovedJFRoutines'+suffix))
        inDetJetFitterUtils = acc.popToolsAndMerge(InDetJetFitterUtilsCfg(flags,'InDetJFUtils'+suffix))
        improvedJetFitterInitializationHelper = acc.popToolsAndMerge(ImprovedJetFitterInitializationHelperCfg('ImprovedJFInitHelper'+suffix))
        defaults = { 'MaxNumDeleteIterations' : 30 ,
                     'VertexProbCut' : 0.001 ,
                     'MaxClusteringIterations' : 30 ,
                     'UseFastClustering' : False ,
                     'maxTracksForDetailedClustering' : 25 ,
                     'VertexClusteringProbabilityCut' : 0.005 ,
                     'VertexClusteringProbabilityCutWithMass' : 0.05 ,
                     'VertexClusteringProbabilityCutWithMass0010' : 0.002 ,
                     'VertexClusteringProbabilityCutWithMass1015' : 0.002 ,
                     'VertexClusteringProbabilityCutWithMass1520' : 0.050 ,
                     'VertexClusteringProbabilityCutWithMass2025' : 0.100 ,
                     'VertexClusteringProbabilityCutWithMass2530' : 0.200 ,
                     'VertexClusteringProbabilityCutWithMass3040' : 0.500 ,
                     'VertexClusteringProbabilityCutWithMass4050' : 0.700 ,
                     'VertexClusteringProbabilityCutWithMass5060' : 0.900 ,
                     'VertexClusteringProbabilityCutWithMass6070' : 0.900 ,
                     'JetFitterInitializationHelper' : improvedJetFitterInitializationHelper,
#                     'JetFitterHelper' : ,
                     'JetFitterRoutines' : improvedJetFitterRoutines,
                     'InDetJetFitterUtils' : inDetJetFitterUtils }
        for option in defaults:
            options.setdefault(option, defaults[option])

    options['name'] = name
    acc.setPrivateTools( InDet__JetFitterMultiStageFit(**options) )
    return acc



