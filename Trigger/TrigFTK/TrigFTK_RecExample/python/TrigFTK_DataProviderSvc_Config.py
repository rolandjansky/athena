# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from FTK_DataProviderSvc.FTK_DataProviderSvcConf import FTK_DataProviderSvc, FTK_UncertaintyTool


class TrigFTK_DataProviderSvc(FTK_DataProviderSvc) :

    def __init__(self, name = "TrigFTK_DataProviderSvc"):
        print "In FTK_DataProviderSvc_Config.py"  
        FTK_DataProviderSvc.__init__(self, name) 

        from AthenaCommon.AppMgr import ToolSvc


        from TrigInDetConf.TrigInDetRecToolsFTK import InDetTrigTrackFitterFTK, InDetTrigRotCreatorFTK, \
        TrigFTK_UncertaintyTool,TrigFTK_RawVertexFinderTool,InDetTrigTrackParticleCreatorToolFTK,InDetTrigTrackSummaryToolFTK
        from InDetTrigRecExample.InDetTrigConfigRecLoadToolsPost import InDetTrigPriVxFinderTool
        
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
        self.VertexFinderTool=InDetTrigPriVxFinderTool
        self.ROTcreatorTool= InDetTrigRotCreatorFTK 


        from RecExConfig.RecFlags import rec
        self.doTruth= rec.doTruth()
        self.TrainingBeamspotX= 0.0
        self.TrainingBeamspotY= 0.0
        self.TrainingBeamspotZ = 0.0
        self.TrainingBeamspotTiltX= 0.0
        self.TrainingBeamspotTiltY= 0.0
        self. PixelBarrelPhiOffsets=[0.,0.,0.,0.]
        self. PixelBarrelEtaOffsets=[0.,0.,0.,0.]
        self. PixelEndCapPhiOffsets=[0,0.,0.]
        self. PixelEndCapEtaOffsets=[0.,0.,0.]

        


