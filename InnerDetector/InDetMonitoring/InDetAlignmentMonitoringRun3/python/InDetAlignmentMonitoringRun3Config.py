#
#  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
#

####################################################
#                                                  #
# InDetAlignmentManager top algorithm              #
#                                                  #
####################################################

def InDetAlignmentMonitoringRun3Config(flags):
    from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
    acc = ComponentAccumulator()
    
    from AthenaMonitoring import AthMonitorCfgHelper
    helper = AthMonitorCfgHelper(flags, "InDetAlignmentMonitoringRun3")
        
    from AthenaConfiguration.ComponentFactory import CompFactory

    if flags.DQ.Environment in ('online', 'tier0', 'tier0Raw'):

        ########### here begins InDetAlignMonGenericTracksAlg ###########
        kwargsIDAlignMonGenericTracksAlg = { 
            'vxPrimContainerName' : 'PrimaryVertices', #InDetKeys.xAODVertexContainer(),
            'TrackName'  : 'ExtendedTracks',  #Until new config ready
            'TrackName2' : 'ExtendedTracks',  #Until new config ready
        }

        from InDetAlignmentMonitoringRun3.IDAlignMonGenericTracksAlgCfg import IDAlignMonGenericTracksAlgCfg
        inDetAlignMonGenericTracksAlg = helper.addAlgorithm(CompFactory.IDAlignMonGenericTracksAlg, 'IDAlignMonGenericTracksAlg')
        for k, v in kwargsIDAlignMonGenericTracksAlg.items():
            setattr(inDetAlignMonGenericTracksAlg, k, v)

        inDetAlignMonGenericTracksAlg.TrackSelectionTool = CompFactory.InDet.InDetTrackSelectionTool('IDAlignMonGenericTracksAlg_TrackSelectionTool')
        inDetAlignMonGenericTracksAlg.TrackSelectionTool.UseTrkTrackTools            = True
        inDetAlignMonGenericTracksAlg.TrackSelectionTool.CutLevel                    = "TightPrimary"
        inDetAlignMonGenericTracksAlg.TrackSelectionTool.maxNPixelHoles              = 1
        inDetAlignMonGenericTracksAlg.TrackSelectionTool.minNBothInnermostLayersHits = 0
        inDetAlignMonGenericTracksAlg.TrackSelectionTool.minNInnermostLayerHits      = 1
        inDetAlignMonGenericTracksAlg.TrackSelectionTool.minPt                       = 5000
        inDetAlignMonGenericTracksAlg.TrackSelectionTool.maxD0                       = 100000 
        inDetAlignMonGenericTracksAlg.TrackSelectionTool.maxZ0SinTheta               = 150 
        IDAlignMonGenericTracksAlgCfg(helper, inDetAlignMonGenericTracksAlg, **kwargsIDAlignMonGenericTracksAlg)
        
        ########### here ends InDetAlignMonGenericTracksAlg ###########
   

        ########### here starts InDetAlignMonResidualAlgs ###########
     
        kwargsIDAlignMonResidualsAlg = { 
            'TrackName'  : 'CombinedInDetTracks',  #Until new config ready
            'TrackName2' : 'CombinedInDetTracks',  #Until new config ready
        }

        from InDetAlignmentMonitoringRun3.IDAlignMonResidualsAlgCfg import IDAlignMonResidualsAlgCfg
        inDetAlignMonResidualsAlg = helper.addAlgorithm(CompFactory.IDAlignMonResidualsAlg, 'IDAlignMonResidualsAlg')
        
        for k, v in kwargsIDAlignMonResidualsAlg.items():
            setattr(inDetAlignMonResidualsAlg, k, v)

        inDetAlignMonResidualsAlg.TrackSelectionTool = CompFactory.InDet.InDetTrackSelectionTool('IDAlignMonResidualsAlg_TrackSelectionTool')
        inDetAlignMonResidualsAlg.TrackSelectionTool.UseTrkTrackTools            = True
        inDetAlignMonResidualsAlg.TrackSelectionTool.CutLevel                    = "TightPrimary"
        inDetAlignMonResidualsAlg.TrackSelectionTool.maxNPixelHoles              = 1
        inDetAlignMonResidualsAlg.TrackSelectionTool.minNBothInnermostLayersHits = 0
        inDetAlignMonResidualsAlg.TrackSelectionTool.minNInnermostLayerHits      = 1
        inDetAlignMonResidualsAlg.TrackSelectionTool.minPt                       = 5000
        inDetAlignMonResidualsAlg.TrackSelectionTool.maxD0                       = 100000 
        inDetAlignMonResidualsAlg.TrackSelectionTool.maxZ0SinTheta               = 150 
    
        IDAlignMonResidualsAlgCfg(helper, inDetAlignMonResidualsAlg, **kwargsIDAlignMonResidualsAlg)
        
        ########### here ends InDetAlignMonResidualsAlg ###########

    acc.merge(helper.result())
    return acc
