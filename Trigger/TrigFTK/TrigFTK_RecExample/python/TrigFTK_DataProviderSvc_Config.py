# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from FTK_DataProviderSvc.FTK_DataProviderSvcConf import FTK_DataProviderSvc, FTK_UncertaintyTool


class TrigFTK_DataProviderSvc(FTK_DataProviderSvc) :

    def __init__(self, name = "TrigFTK_DataProviderSvc", SCTLorentzAngleTool=None):
        print "In FTK_DataProviderSvc_Config.py"  
        FTK_DataProviderSvc.__init__(self, name) 

        from TrigInDetConf.TrigInDetRecToolsFTK import InDetTrigTrackFitterFTK, InDetTrigRotCreatorFTK,  \
        TrigFTK_UncertaintyTool,TrigFTK_RawVertexFinderTool,InDetTrigTrackParticleCreatorToolFTK,InDetTrigTrackSummaryToolFTK,TrigFTK_HashIDTool
        
        self.TrackCollectionName= "FTK_TrackCollection"
        self.TrackParticleContainerName= "FTK_TrackParticleContainer"
        self.VxContainerName="notused"
        self.VertexContainerName="FTK_VertexContainer"	  
        self.PixelTruthName="FTK_PRD_MultiTruthPixel"
        self.SctTruthName="FTK_PRD_MultiTruthSCT"
        self.PixelClusterContainerName= "FTK_PixelClusterContainer"
        self.SCT_ClusterContainerName= "FTK_SCT_ClusterContainer"
        self.CorrectPixelClusters=True
        self.CorrectSCTClusters=True
        
        self.TrackFitter = InDetTrigTrackFitterFTK
        self.UncertaintyTool=TrigFTK_UncertaintyTool
        self.TrackSummaryTool=InDetTrigTrackSummaryToolFTK
        self.TrackParticleCreatorTool=InDetTrigTrackParticleCreatorToolFTK
        self.RawVertexFinderTool=TrigFTK_RawVertexFinderTool
        self.ROTcreatorTool= InDetTrigRotCreatorFTK
        self.HashIDTool=TrigFTK_HashIDTool

        from AthenaCommon.BeamFlags import jobproperties

        if (jobproperties.Beam.beamType() != 'cosmics'):
            from InDetTrigRecExample.InDetTrigConfigRecLoadToolsPost import InDetTrigPriVxFinderTool
            from TrigInDetConf.TrigInDetRecToolsFTK import TrigFTK_VertexCollectionSortingTool
            self.DoVertexing=True
            self.VertexFinderTool=InDetTrigPriVxFinderTool
            self.VertexCollectionSortingTool=TrigFTK_VertexCollectionSortingTool
        else:
            self.DoVertexing=False
        from RecExConfig.RecFlags import rec
        self.doTruth= rec.doTruth()

        if SCTLorentzAngleTool is not None:
            FTK_DataProviderSvc.SCTLorentzAngleTool = SCTLorentzAngleTool
