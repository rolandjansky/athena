# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from BTagging.ImprovedJetFitterInitializationHelperConfig import ImprovedJetFitterInitializationHelperCfg
from BTagging.TrkDistanceFinderNeutralNeutralConfig import TrkDistanceFinderNeutralNeutralCfg
from BTagging.TrkDistanceFinderNeutralChargedConfig import TrkDistanceFinderNeutralChargedCfg

def ImprovedJetFitterRoutinesCfg(name, useBTagFlagsDefaults = True, **options):
    """Sets up a ImprovedJetFitterRoutines tool and returns it.

    The following options have BTaggingFlags defaults:

    BeFast                              default: False
    maxDRshift                          default: 0.0
    noPrimaryVertexRefit                default: False

    input:             name: The name of the tool (should be unique).
      useBTagFlagsDefaults : Whether to use BTaggingFlags defaults for options that are not specified.
                  **options: Python dictionary with options for the tool.
    output: The actual tool, which can then by added to ToolSvc via ToolSvc += output."""
    acc = ComponentAccumulator()
    if useBTagFlagsDefaults:
        improvedJetFitterInitializationHelper = acc.popToolsAndMerge(ImprovedJetFitterInitializationHelperCfg('ImprovedJFInitHelper'))
        trkDistanceFinderNeutralNeutral = acc.popToolsAndMerge(TrkDistanceFinderNeutralNeutralCfg('TrkDistFinderNeutralNeutral'))
        trkDistanceFinderNeutralCharged = acc.popToolsAndMerge(TrkDistanceFinderNeutralChargedCfg('TrkDistFinderNeutralCharged'))
        #JFKalmanVertexOnJetAxisSmoother = acc.popToolsAndMerge(KalmanVertexOnJetAxisSmootherCfg('JFKalmanVertexOnJetAxisSmoother')
        defaults = {
                     'BeFast'               : False,
                     'maxDRshift'           : 0.0,
                     'noPrimaryVertexRefit' : False,
                     'JetFitterInitializationHelper' : improvedJetFitterInitializationHelper,
                     'JetFitterMinimumDistanceFinder' : trkDistanceFinderNeutralCharged,
                     'JetFitterMinimumDistanceFinderNeutral' : trkDistanceFinderNeutralNeutral, 
                     #'KalmanVertexOnJetAxisSmoother' : JFKalmanVertexOnJetAxisSmoother,
                   }
        for option in defaults:
            options.setdefault(option, defaults[option])
    options['name'] = name
    acc.setPrivateTools(CompFactory.Trk.JetFitterRoutines(**options))

    return acc
