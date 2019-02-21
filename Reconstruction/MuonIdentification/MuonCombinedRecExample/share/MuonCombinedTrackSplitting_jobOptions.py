#-----------------------------------------------------------------------------
# configuration of Split track analysis
#
#-----------------------------------------------------------------------------

if muonCombinedRecFlags.doCombinedMuonFit():
  CombinedTrackCollection = "Combined_Tracks"


  # split COMBINED tracks
  from InDetTrackSplitterTool.InDetTrackSplitterToolConf import InDet__InDetTrackSplitterTool
  CombinedTrackSplitterTool=InDet__InDetTrackSplitterTool(name="CombinedTrackSplitterTool")
#  CombinedTrackSplitterTool.TrackFitter=CombMuonTrackFitter 
  CombinedTrackSplitterTool.TrackFitter= ToolSvc.MuonCombinedTrackFitter
  ToolSvc+=CombinedTrackSplitterTool

  from InDetTrackValidation.InDetTrackValidationConf import InDet__InDetSplittedTracksCreator
  CombinedTrackSplitterAlg=InDet__InDetSplittedTracksCreator(name="CombinedTrackSplitterAlg")
  CombinedTrackSplitterAlg.TrackCollection=CombinedTrackCollection
  CombinedTrackSplitterAlg.OutputTrackCollection=CombinedTrackCollection+"_split"
  CombinedTrackSplitterAlg.TrackSplitterTool=CombinedTrackSplitterTool #CAREFUL!!  
  topSequence +=  CombinedTrackSplitterAlg

  #truth tracks
  if rec.doTruth():
    include ("InDetRecExample/ConfiguredInDetTrackTruth.py")
        
    CombinedTracksTruth = ConfiguredInDetTrackTruth(CombinedTrackCollection+"_split",
                                                    CombinedTrackCollection+"DetailedTruth_split",
                                                    CombinedTrackCollection+"_Truth_split" )
if muonCombinedRecFlags.doStaco():
    theStacoMaster.IDTrackParticleCollectionLocation="TrackParticleCandidate_split" 
    theStaco.SelectUpDown   = -2
    theStacoUP.SelectUpDown =  2

# the following is recommended as a postExec to Reco_trf:
# from AthenaCommon.AppMgr import theApp
# StreamESD = theApp.getOutputStream( "StreamESD" )
# StreamESD.ItemList+=["TrackCollection#Combined_Tracks_split"]

