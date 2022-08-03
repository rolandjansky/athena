#
#  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
#

####################################################
#                                                  #
# InDetGlobalManager top algorithm                 #
#                                                  #
####################################################



def InDetGlobalMonitoringRun3TestConfig(flags):
    from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
    acc = ComponentAccumulator()
    
    from AthenaMonitoring import AthMonitorCfgHelper
    helper = AthMonitorCfgHelper(flags, "InDetGlobalMonitoringRun3Test")
        
    from AthenaConfiguration.ComponentFactory import CompFactory

    from AthenaMonitoring.FilledBunchFilterToolConfig import FilledBunchFilterToolCfg

    # run on RAW only
    if flags.DQ.Environment in ('online', 'tier0', 'tier0Raw'):
        
        ########### here begins InDetGlobalTrackMonAlg ###########
        from InDetGlobalMonitoringRun3Test.InDetGlobalTrackMonAlgCfg import InDetGlobalTrackMonAlgCfg 

        
        inDetGlobalTrackMonAlg = helper.addAlgorithm(CompFactory.InDetGlobalTrackMonAlg, 'InDetGlobalTrackMonAlg',addFilterTools = [FilledBunchFilterToolCfg(flags)])

        from InDetConfig.InDetTrackSelectionToolConfig import InDetTrackSelectionTool_TightPrimary_TrackTools_Cfg
        TrackSelectionTool = acc.popToolsAndMerge(
            InDetTrackSelectionTool_TightPrimary_TrackTools_Cfg(flags, name='TrackSelectionTool',
                                                                maxNPixelHoles = 1, # Default for TightPrimary is 0
                                                                minPt = 5000))
        Tight_TrackSelectionTool = acc.popToolsAndMerge(
            InDetTrackSelectionTool_TightPrimary_TrackTools_Cfg(flags, name='TightTrackSelectionTool',
                                                                minPt = 5000))

        Loose_TrackSelectionTool = acc.popToolsAndMerge(
            InDetTrackSelectionTool_TightPrimary_TrackTools_Cfg(flags, name='LooseTrackSelectionTool',
                                                                minPt = 1000))

        from TrkConfig.TrkVertexFitterUtilsConfig import TrackToVertexIPEstimatorCfg
        TrackToVertexIPEstimator = acc.popToolsAndMerge(
            TrackToVertexIPEstimatorCfg(flags, name='TrackToVertexIPEstimator'))

        inDetGlobalTrackMonAlg.TrackSelectionTool = TrackSelectionTool
        inDetGlobalTrackMonAlg.Tight_TrackSelectionTool = Tight_TrackSelectionTool
        inDetGlobalTrackMonAlg.Loose_TrackSelectionTool = Loose_TrackSelectionTool
        inDetGlobalTrackMonAlg.TrackToVertexIPEstimator = TrackToVertexIPEstimator 

        InDetGlobalTrackMonAlgCfg(helper, inDetGlobalTrackMonAlg)
        ########### here ends InDetGlobalTrackMonAlg ###########


    if flags.DQ.Environment in ('online', 'tier0', 'tier0Raw') and (flags.InDet.Tracking.doLargeD0 or flags.InDet.Tracking.doR3LargeD0 or flags.InDet.Tracking.doLowPtLargeD0):
        ########### here begins InDetGlobalLRTMonAlg ###########
        from InDetGlobalMonitoringRun3Test.InDetGlobalLRTMonAlgCfg import InDetGlobalLRTMonAlgCfg
        inDetGlobalLRTMonAlg = helper.addAlgorithm(CompFactory.InDetGlobalLRTMonAlg, 'InDetGlobalLRTMonAlg',addFilterTools = [FilledBunchFilterToolCfg(flags)])

        from InDetConfig.InDetTrackSelectionToolConfig import InDetGlobalLRTMonAlg_TrackSelectionToolCfg
        inDetGlobalLRTMonAlg.TrackSelectionTool = acc.popToolsAndMerge(InDetGlobalLRTMonAlg_TrackSelectionToolCfg(flags))

        InDetGlobalLRTMonAlgCfg(helper, inDetGlobalLRTMonAlg)
        ########### here ends InDetGlobalLRTMonAlg ###########







        
    # run on ESD
    if flags.DQ.Environment != 'tier0Raw':
        ########### here begins InDetGlobalPrimaryVertexMonAlg ###########
        from InDetGlobalMonitoringRun3Test.InDetGlobalPrimaryVertexMonAlgCfg import InDetGlobalPrimaryVertexMonAlgCfg 
        
        myInDetGlobalPrimaryVertexMonAlg = helper.addAlgorithm(CompFactory.InDetGlobalPrimaryVertexMonAlg,
                                                               'InDetGlobalPrimaryVertexMonAlg',addFilterTools = [FilledBunchFilterToolCfg(flags)])
        
        kwargsInDetGlobalPrimaryVertexMonAlg = { 
            'vxContainerName'                      : 'PrimaryVertices', #InDetKeys.xAODVertexContainer(),
            'doEnhancedMonitoring'                 : True # InDetFlags.doMonitoringPrimaryVertexingEnhanced()
        }
        
        for k, v in kwargsInDetGlobalPrimaryVertexMonAlg.items():
            setattr(myInDetGlobalPrimaryVertexMonAlg, k, v)
            
        InDetGlobalPrimaryVertexMonAlgCfg(helper, myInDetGlobalPrimaryVertexMonAlg, **kwargsInDetGlobalPrimaryVertexMonAlg)

        ########### here ends InDetGlobalPrimaryVertexMonAlg ###########

        ########### here begins InDetGlobalBeamSpotMonAlg ###########
        from BeamSpotConditions.BeamSpotConditionsConfig import BeamSpotCondAlgCfg
        acc.merge(BeamSpotCondAlgCfg(flags))
       
        from InDetGlobalMonitoringRun3Test.InDetGlobalBeamSpotMonAlgCfg import InDetGlobalBeamSpotMonAlgCfg 
        
        myInDetGlobalBeamSpotMonAlg = helper.addAlgorithm(CompFactory.InDetGlobalBeamSpotMonAlg,
                                                          'InDetGlobalBeamSpotMonAlg',addFilterTools = [FilledBunchFilterToolCfg(flags)])
        
        kwargsInDetGlobalBeamSpotMonAlg = { 
            'BeamSpotKey'                      : 'BeamSpotData', #InDetKeys.BeamSpotData(),
            'vxContainerName'                  : 'PrimaryVertices', #InDetKeys.xAODVertexContainer(),
            'trackContainerName'               : 'InDetTrackParticles', #InDetKeys.xAODTrackParticleContainer(),
            'useBeamspot'                      : True, # InDetFlags.useBeamConstraint()
            'vxContainerWithBeamConstraint'    : False # InDetFlags.useBeamConstraint()
        }
        
        for k, v in kwargsInDetGlobalBeamSpotMonAlg.items():
            setattr(myInDetGlobalBeamSpotMonAlg, k, v)

        InDetGlobalBeamSpotMonAlgCfg(helper, myInDetGlobalBeamSpotMonAlg, **kwargsInDetGlobalBeamSpotMonAlg)

        ########### here ends InDetGlobalBeamSpotMonAlg ###########
        
    acc.merge(helper.result())
    return acc
