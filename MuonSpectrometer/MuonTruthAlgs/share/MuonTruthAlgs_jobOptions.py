# Need to update this for new truth.
# SG keys for MuonPRD_MultiTruthMaker are set up in C++ and genConf, nothing to do for it.
# For MuonDetailedTrackTruthMaker, need to set truth maps to 
# ["MDT_TruthMap"    , "CSC_TruthMap"    , "RPC_TruthMap"    , "TGC_TruthMap"]
# because this is not in C++
# To handle several track collections like MuonTrackTruthMaker did, just add
# several instances of MuonDetailedTrackTruthMaker, with proper settings, to the top AlgSequence.
#


## if not 'MuonTruthAlgs' in theApp.Dlls:
##   theApp.Dlls += ["MuonTruthAlgs"]
## 
## if not 'TrkTruthCreatorTools' in theApp.Dlls:
##   theApp.Dlls += ["TrkTruthCreatorTools"]
## 
## #Should probably make it possible to run one without the other (i.e. maybe redo Tracktruth from ESD PRD collections)
## theApp.TopAlg += ["MuonPrepRawDataTruthMaker"]
## theApp.TopAlg += ["MuonTrackTruthMaker"]
## MuonPrepRawDataTruthMaker = Algorithm( "MuonPrepRawDataTruthMaker" )
## 
## # Convention is 1st = mdts, 2nd = cscs, 3rd = rpcs, 4th = tgcs
## MuonPrepRawDataTruthMaker.PrepRawDataContainers   = ["MDT_PREPDATA_NEW", "CSC_PREPDATA_NEW", "RPC_PREPDATA_NEW", "TGC_PREPDATA_NEW"]
## MuonPrepRawDataTruthMaker.PrepDataTruthContainers = ["MDT_TruthMap"    , "CSC_TruthMap"    , "RPC_TruthMap"    , "TGC_TruthMap"]
## MuonPrepRawDataTruthMaker.SdoContainers           = ["MDT_SDO"         , "CSC_SDO"         , "RPC_SDO"         , "TGC_SDO"]
## MuonPrepRawDataTruthMaker.OutputLevel = INFO
## 
## MuonTrackTruthMaker = Algorithm( "MuonTrackTruthMaker" )
## 
## MuonTrackTruthMaker.TrackCollNames         = ["ConvertedMooreTracks", "ConvertedMBoyMuonSpectroOnlyTracks", "ConvertedMBoyTracks", "ConvertedMuTagTracks", "ConvertedStacoTracks"]
## MuonTrackTruthMaker.TrackTruthMapNames     = ["ConvertedMooreTracksTruth", "ConvertedMBoyMuonSpectroOnlyTracksTruth", "ConvertedMBoyTracksTruth", "ConvertedMuTagTracksTruth", "ConvertedStacoTracksTruth"]
## MuonTrackTruthMaker.TrackTruthToolName     = "Trk::TrackTruthMaker"
## MuonTrackTruthMaker.TrackTruthToolInstance = "TracksTruthMaker"
## 
## MuonTrackTruthMakerTool=Algorithm("ToolSvc.TracksTruthMaker")
## MuonTrackTruthMakerTool.DoInDet=False
## MuonTrackTruthMaker.OutputLevel = INFO
