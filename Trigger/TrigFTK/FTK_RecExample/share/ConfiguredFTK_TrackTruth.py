# Blocking the include for after first inclusion of the TrackTruth module
include.block ('FTK_RecExample/ConfiguredFTK_TrackTruth.py')

# -------------------------------------------------------------------------
#
# ------- fragment to handle track truth association
#
# -------------------------------------------------------------------------

class  ConfiguredFTK_TrackTruth:
    
    def __init__(self, Tracks = None, DetailedTruth = None, TracksTruth = None):

        #
        # get ToolSvc and topSequence
        #
        from AthenaCommon.AppMgr                import ToolSvc
        from AthenaCommon.AlgSequence           import AlgSequence
        topSequence = AlgSequence()
        #
        # --- Enable the detailed track truth
        #
        
        from InDetTruthAlgs.InDetTruthAlgsConf import InDet__InDetDetailedTrackTruthMaker
        theFTK_DetailedTruthMaker = InDet__InDetDetailedTrackTruthMaker(name                   = DetailedTruth+"Maker",
                                                                        TrackCollectionName    = Tracks,
                                                                        DetailedTrackTruthName = DetailedTruth,
                                                                        TruthNamePixel         = "FTK_PRD_MultiTruthPixel",
                                                                        TruthNameSCT           = "FTK_PRD_MultiTruthSCT",
                                                                        TruthNameTRT           = "")
        topSequence += theFTK_DetailedTruthMaker
        

        #
        # --- Detailed to old TrackTruth
        #
        from TrkTruthCreatorTools.TrkTruthCreatorToolsConf import Trk__TruthMatchRatio as InDetTruthMatchTool

        theFTK_TruthMatchSimilarityTool = InDetTruthMatchTool(name        = "FTK_TruthMatchTool",
                                                              WeightPixel =  10.,
                                                              WeightSCT   =   5.,
                                                              WeightTRT   =   1.)
        
        # --- only add this once !!!
        if not hasattr(ToolSvc, "FTK_TruthMatchTool"):
            ToolSvc += theFTK_TruthMatchSimilarityTool


        from TrkTruthAlgs.TrkTruthAlgsConf import TrackTruthSimilaritySelector
        theFTK_TruthSimilaritySelector = TrackTruthSimilaritySelector(name                     = TracksTruth+"Selector",
                                                                      DetailedTrackTruthName   = DetailedTruth,
                                                                      OutputName               = TracksTruth,
                                                                      TrackTruthSimilarityTool = theFTK_TruthMatchSimilarityTool)
        topSequence += theFTK_TruthSimilaritySelector


        # --- remember imput
        self.__Tracks        = Tracks
        self.__TracksTruth   = TracksTruth
        self.__DetailedTruth = DetailedTruth
        
            
    def Tracks (self):
        return self.__Tracks

    def TracksTruth (self):
        return self.__TracksTruth

    def DetailedTruth (self):
        return self.__DetailedTruth
