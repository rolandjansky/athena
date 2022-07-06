#
#  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
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
    from InDetConfig.InDetTrackSelectionToolConfig import IDAlignMonTrackSelectionToolCfg

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

        inDetAlignMonGenericTracksAlg.TrackSelectionTool = acc.popToolsAndMerge(IDAlignMonTrackSelectionToolCfg(flags))

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

        inDetAlignMonResidualsAlg.TrackSelectionTool = acc.popToolsAndMerge(IDAlignMonTrackSelectionToolCfg(flags))
    
        IDAlignMonResidualsAlgCfg(helper, inDetAlignMonResidualsAlg, **kwargsIDAlignMonResidualsAlg)
        
        ########### here ends InDetAlignMonResidualsAlg ###########

    acc.merge(helper.result())
    return acc
