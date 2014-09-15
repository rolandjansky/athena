ToolSvc.MuonClusterOnTrackCreator.FixedErrorTgcEta = 80.

#mgmAlignTool = ToolSvc.MGM_AlignmentDbTool
#mgmAlignTool.OutputLevel = INFO 
#mgmAlignTool.ALinesFile = 'newCSCALines.txt'
#print ToolSvc.MGM_AlignmentDbTool

include("MuidExample/MuonIdentification_jobOptions.py")

from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

MCTBSLFitter = MuonRecTools.getPublicToolClone("myMCTBSLFitter","MCTBSLFitter")
MCTBSLFitter.FillDerivativeMatrix = False
MCTBSLFitter.SortHits = False
MCTBSLFitter.OutputLevel = INFO
MCTBSLFitter.GetMaterialFromTrack = True
MCTBSLFitter.Momentum=0. 
ToolSvc += MCTBSLFitter

# create instance of MuonTrackTagTestTool
from MuonTrackThroughCalo.ConfMuonTrackThroughCalo import ConfMuonTrackThroughCalo
MuonTTC = ConfMuonTrackThroughCalo("MuonTrackThroughCalo")

from AthenaCommon.AppMgr import ToolSvc
ToolSvc += MuonTTC

from MuonRecExample import MuonRecTools            
atlasExtrapolator=MuonRecTools.getPublicTool('AtlasExtrapolator')
from MuonCombinedTestTools.MuonCombinedTestToolsConf import MuonCombined__MuonTrackTagTestTool
combinedMuonTag = MuonCombined__MuonTrackTagTestTool(name = "CombinedMuonTag",
                                                     ExtrapolatorTool = atlasExtrapolator,
                                                     TTCTool          = MuonTTC,
                                                     Chi2Cut          = 2500.)
ToolSvc += combinedMuonTag

from MuonAlignGenTools.MuonAlignGenToolsConf import Muon__UnbiasedMuonResiduals
topSequence += Muon__UnbiasedMuonResiduals("UnbiasedMuonResiduals",
                                     WhichSegmCollections = [1,1],
                                     SegmentCollectionName = ["MooreSegments","ConvertedMBoySegments"],
                                     #MuonTrackCollections = ["MuidMuonCollection","StacoMuonCollection"],
                                     MuonTrackCollections  = ["MooreTracks","ConvertedMBoyTracks"],
                                     InDetTrackCollections = ["Tracks","Tracks"],
                                     TagTool = combinedMuonTag,
                                     Fitter = ToolSvc.myMCTBSLFitter,
                                     CombinedTrackBuilder = CombinedMuonTrackBuilder,
                                     TrackFittingType = 4, # MS standalone with ID track match
                                     UseTrackContainer=True,
                                     #maxIDTrackd0 = 5,
                                     #maxIDTrackz0 = 200,
                                     minMuonMom   = 4.0,
                                     #minPIXHits   = 1,
                                     #minSCTHits   = 4,
                                     #minTRTHits   = 0,
                                     #minMDTHits   = 10,
                                     #minCSCHits   = 0,
                                     TriggerList  = [],
                                     doMdt=True,
				     doCsc=True,
				     MakeTgcOutliers=True, 
                                     OutputLevel  = INFO) 

