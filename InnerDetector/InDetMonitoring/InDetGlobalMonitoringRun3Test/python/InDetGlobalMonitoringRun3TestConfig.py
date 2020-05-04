#
#  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
#



####################################################
#                                                  #
# InDetGlobalManager top algorithm                 #
#                                                  #
####################################################

def InDetGlobalMonitoringRun3TestConfig(flags):
    from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
    acc = ComponentAccumulator()
    
    # run on RAW only
    if flags.DQ.Environment in ('online', 'tier0', 'tier0Raw'):
        ##        from InDetRecExample.InDetKeys import InDetKeys    ## not sure it works now
        
        ########### here begins InDetGlobalTrackMonAlg ###########
        kwargsInDetGlobalTrackMonAlg = { 
            'DoIBL' : True,                       #InDetFlags.doIBL(), #Turn on/off IBL histograms 
            'TrackName'  : 'CombinedInDetTracks',  #Until new config ready
            'TrackName2' : 'CombinedInDetTracks',  #Until new config ready
            'TrackName3' : 'CombinedInDetTracks',  #Until new config ready
        }
        
        
        from AthenaMonitoring import AthMonitorCfgHelper
        helper = AthMonitorCfgHelper(flags, "InDetGlobalMonitoringRun3Test")
        
        from AthenaConfiguration.ComponentFactory import CompFactory
        #        from InDetGlobalMonitoringRun3Test.InDetGlobalMonitoringRun3TestConf import InDetGlobalTrackMonAlg
        from InDetGlobalMonitoringRun3Test.InDetGlobalTrackMonAlgCfg import InDetGlobalTrackMonAlgCfg 

        inDetGlobalTrackMonAlg = helper.addAlgorithm(CompFactory.InDetGlobalTrackMonAlg, 'InDetGlobalTrackMonAlg')
        for k, v in kwargsInDetGlobalTrackMonAlg.items():
            setattr(inDetGlobalTrackMonAlg, k, v)

        inDetGlobalTrackMonAlg.TrackSelectionTool.UseTrkTrackTools = True
        inDetGlobalTrackMonAlg.TrackSelectionTool.CutLevel         = "TightPrimary"
        inDetGlobalTrackMonAlg.TrackSelectionTool.maxNPixelHoles   = 1
        inDetGlobalTrackMonAlg.TrackSelectionTool.minPt            = 5000
        #        InDetGlobalTrackMonAlg.Baseline_TrackSelectionTool.TrackSummaryTool = InDetTrackSummaryTool
        #        InDetGlobalTrackMonAlg.Baseline_TrackSelectionTool.Extrapolator     = InDetExtrapolator
        #
        inDetGlobalTrackMonAlg.Tight_TrackSelectionTool.UseTrkTrackTools = True
        inDetGlobalTrackMonAlg.Tight_TrackSelectionTool.CutLevel         = "TightPrimary"
        inDetGlobalTrackMonAlg.Tight_TrackSelectionTool.minPt            = 5000
        #        InDetGlobalTrackMonAlg.Tight_TrackSelectionTool.TrackSummaryTool = InDetTrackSummaryTool
        #        InDetGlobalTrackMonAlg.Tight_TrackSelectionTool.Extrapolator     = InDetExtrapolator
        

        # Run 3 configs - stolen from SCT
        from SCT_Monitoring.TrackSummaryToolWorkaround import TrackSummaryToolWorkaround
        inDetGlobalTrackMonAlg.TrackSelectionTool.TrackSummaryTool = acc.popToolsAndMerge(TrackSummaryToolWorkaround(flags))
        inDetGlobalTrackMonAlg.TrackSelectionTool.Extrapolator     = acc.getPublicTool("InDetExtrapolator")
        inDetGlobalTrackMonAlg.Tight_TrackSelectionTool.TrackSummaryTool = acc.popToolsAndMerge(TrackSummaryToolWorkaround(flags))
        inDetGlobalTrackMonAlg.Tight_TrackSelectionTool.Extrapolator     = acc.getPublicTool("InDetExtrapolator")
        
        InDetGlobalTrackMonAlgCfg(helper, inDetGlobalTrackMonAlg, **kwargsInDetGlobalTrackMonAlg)
        ########### here ends InDetGlobalTrackMonAlg ###########
        
        
        ########### here begins InDetGlobalPrimaryVertexMonAlg ###########
        from InDetGlobalMonitoringRun3Test.InDetGlobalMonitoringRun3TestConf import InDetGlobalPrimaryVertexMonAlg
        from InDetGlobalMonitoringRun3Test.InDetGlobalPrimaryVertexMonAlgCfg import InDetGlobalPrimaryVertexMonAlgCfg 
        
        myInDetGlobalPrimaryVertexMonAlg = helper.addAlgorithm(InDetGlobalPrimaryVertexMonAlg, 'InDetGlobalPrimaryVertexMonAlg')
        
        kwargsInDetGlobalPrimaryVertexMonAlg = { 
            'vxContainerName'                      : 'PrimaryVertices', #InDetKeys.xAODVertexContainer(),
            'vxContainerNameWithOutBeamConstraint' : 'VxPrimaryCandidateWithBeamConstraint', #InDetKeys.PrimaryVerticesWithoutBeamConstraint(),
            'vxContainerNameSplit'                 : 'VxPrimaryCandidateSplitStream', #InDetKeys.PrimaryVerticesSplitStream(),
            'doEnhancedMonitoring'                 : True # InDetFlags.doMonitoringPrimaryVertexingEnhanced()
        }
        
        for k, v in kwargsInDetGlobalPrimaryVertexMonAlg.items():
            setattr(kwargsInDetGlobalPrimaryVertexMonAlg, k, v)
            
        InDetGlobalPrimaryVertexMonAlgCfg(helper, myInDetGlobalPrimaryVertexMonAlg, **kwargsInDetGlobalPrimaryVertexMonAlg)

        ########### here ends InDetGlobalPrimaryVertexMonAlg ###########

        ########### here begins InDetGlobalBeamSpotMonAlg ###########
        
        from InDetGlobalMonitoringRun3Test.InDetGlobalMonitoringRun3TestConf import InDetGlobalBeamSpotMonAlg
        from InDetGlobalMonitoringRun3Test.InDetGlobalBeamSpotMonAlgCfg import InDetGlobalBeamSpotMonAlgCfg 
        
        myInDetGlobalBeamSpotMonAlg = helper.addAlgorithm(InDetGlobalBeamSpotMonAlg, 'InDetGlobalBeamSpotMonAlg')
        
        kwargsInDetGlobalBeamSpotMonAlg = { 
            'BeamSpotKey'                      : 'BeamSpotData', #InDetKeys.BeamSpotData(),
            'vxContainerName'                  : 'PrimaryVertices', #InDetKeys.xAODVertexContainer(),
            'trackContainerName'               : 'InDetTrackParticles', #InDetKeys.xAODTrackParticleContainer(),
            'useBeamspot'                      : True, # InDetFlags.useBeamConstraint()
            'vxContainerWithBeamConstraint'    : False # InDetFlags.useBeamConstraint()
        }
        
        for k, v in kwargsInDetGlobalBeamSpotMonAlg.items():
            setattr(kwargsInDetGlobalBeamSpotMonAlg, k, v)

        InDetGlobalBeamSpotMonAlgCfg(helper, myInDetGlobalBeamSpotMonAlg, **kwargsInDetGlobalBeamSpotMonAlg)

        ########### here ends InDetGlobalBeamSpotMonAlg ###########
        
    return acc
