# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

print "Adding FTK Truth Information"

#Configure truth tool for FTK tracks
from InDetTruthAlgs.InDetTruthAlgsConf import InDet__InDetDetailedTrackTruthMaker
from InDetRecExample.InDetJobProperties import InDetFlags
from TrkTruthAlgs.TrkTruthAlgsConf import TrackTruthSimilaritySelector
from AthenaCommon.AppMgr                import ToolSvc

print "Printing tool service"
print ToolSvc

class ConfiguredFTK_DetailedTruthMaker(InDet__InDetDetailedTrackTruthMaker):
    def __init__(self,
                 name = "FTKDetailedTruthMaker",
                 TrackCollectionName="FTK_TrackCollection",
                 DetailedTrackTruthName = "FTK_Tracks_DetailedTruthCollection",
                 TruthNamePixel         = "PRD_MultiTruthPixel_FTK",
                 TruthNameSCT           = "PRD_MultiTruthSCT_FTK"):
        
        super(ConfiguredTrigFTKDetailedTruthMaker,self).__init__(
            name                   = name,         
            TrackCollectionName    = TrackCollectionName,
            DetailedTrackTruthName = DetailedTrackTruthName,
            TruthNamePixel         = TruthNamePixel,
            TruthNameSCT           = TruthNameSCT,
            TruthNameTRT           = "")

class ConfiguredFTK_TruthMaker(TrackTruthSimilaritySelector):
    def __init__(self,
                 name = "FTKTruthMaker",
                 TrackCollectionName="FTK_TrackCollection",
                 TrackTruthName = "FTK_Tracks_TruthCollection",
                 TrackDetailedTruthName = "FTK_Tracks_DetailedTruthCollection"):
       
        if InDetFlags.truthMatchStrategy() == 'TruthMatchRatio':
            from TrkTruthCreatorTools.TrkTruthCreatorToolsConf import Trk__TruthMatchRatio as InDetTruthMatchTool
        elif InDetFlags.truthMatchStrategy() == 'TruthMatchTanimoto':
            from TrkTruthCreatorTools.TrkTruthCreatorToolsConf import Trk__TruthMatchTanimoto as InDetTruthMatchTool
        else:
            print "ConfiguredInDetTrackTruth: error! InDetFlags.truthMatchStrategy must be TruthMatchRatio or TruthMatchTanimoto but is: "+InDetFlags.truthMatchStrategy()
        
        FTKTruthMatchSimilarityTool = InDetTruthMatchTool(
            name        = "FTKTruthMatchTool",
            WeightPixel =  10.,
            WeightSCT   =   5.,
            WeightTRT   =   1.)
        from AthenaCommon.AppMgr                import ToolSvc
        print "Trying to print toolsvc again"
        print ToolSvc.__class__
         # --- only add this once !!!
        if not hasattr(ToolSvc, "InDetTruthMatchTool"):
             ToolSvc += FTKTruthMatchSimilarityTool
             if (InDetFlags.doPrintConfigurables()):
                 print FTKTruthMatchSimilarityTool
                 
             
        super(ConfiguredFTK_TruthMaker,self).__init__(
            name                   = name,         
            TrackCollectionName    = TrackCollectionName,
            DetailedTrackTruthName = TrackDetailedTruthName,
            OutputName             = TrackTruthName,
            TrackTruthSimilarityTool = FTKTruthMatchSimilarityTool)
