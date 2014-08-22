from OutputStreamAthenaPool.MultipleStreamManager import MSMgr
if MSMgr.StreamExists("StreamESD"):
    myStream=MSMgr.GetStream("StreamESD")
    myStream.AddItem("TrackCollection#Tracks_split")
    myStream.AddItem("Rec::TrackParticleContainer#TrackParticleCandidate_split")

from MuonCombinedRecExample.MuonCombinedRecFlags import muonCombinedRecFlags
muonCombinedRecFlags.doTrackNtuple = True 




if hasattr(runArgs,"inputBSFile"):


    from InDetTrackSplitterTool.InDetTrackSplitterToolConf import InDet__InDetTrackSplitterTool
    splittertoolcomb= InDet__InDetTrackSplitterTool(name='MuonCombinedSplitterTool',
                                                    TrackFitter=ToolSvc.MuonCombinedTrackFitter)
    ToolSvc += splittertoolcomb
    
    from InDetTrackValidation.InDetTrackValidationConf import InDet__InDetSplittedTracksCreator
    splittercomb=InDet__InDetSplittedTracksCreator(name='MuonCombinedTrackSplitter',
                                                   TrackSplitterTool     = splittertoolcomb,
                                                   TrackCollection       = "Combined_Tracks",
                                                   OutputTrackCollection = "Combined_Tracks_split")
    topSequence+=splittercomb
    
    from InDetTrackSplitterTool.InDetTrackSplitterToolConf import InDet__InDetTrackSplitterTool
    splittertoolcomb= InDet__InDetTrackSplitterTool(name='MuonCombinedSplitterTool',
                                                    TrackFitter=ToolSvc.MuonCombinedTrackFitter)
    ToolSvc += splittertoolcomb
    
    from InDetTrackValidation.InDetTrackValidationConf import InDet__InDetSplittedTracksCreator
    splittercomb=InDet__InDetSplittedTracksCreator(name='MuonCombinedTrackSplitter',
                                                   TrackSplitterTool     = splittertoolcomb,
                                                   TrackCollection       = "Combined_Tracks",
                                                   OutputTrackCollection = "Combined_Tracks_split")
    topSequence+=splittercomb 
    
    theStacoMaster.IDTrackParticleCollectionLocation="TrackParticleCandidate_split"
    theStaco.SelectUpDown = -2
    theStacoUP.SelectUpDown - 2


   
