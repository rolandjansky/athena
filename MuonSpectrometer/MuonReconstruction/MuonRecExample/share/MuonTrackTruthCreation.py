from AthenaCommon.CfgGetter import getPublicTool,getService


from MuonAssociationTools.MuonAssociationToolsConf import Muon__MuonPRD_Provider
ToolSvc += Muon__MuonPRD_Provider("MuonPRD_Provider")

from MuonTruthTools.MuonTruthToolsConf import Muon__MuonTruthTrackBuilder
ToolSvc += Muon__MuonTruthTrackBuilder( "MuonTruthTrackBuilder")

MTCols = ["MDT_TruthMap","CSC_TruthMap","RPC_TruthMap","TGC_TruthMap","STGC_TruthMap","MM_TruthMap"]
from TrkTruthTrackTools.TrkTruthTrackToolsConf import Trk__PRD_TruthTrajectoryBuilder
ToolSvc += Trk__PRD_TruthTrajectoryBuilder( "MuonPRD_TruthTrajectoryBuilder",
                                            MuonPRD_Provider=ToolSvc.MuonPRD_Provider,
                                            PRD_MultiTruthCollections = MTCols )


OutputTracks = "MuonTruthTracks"
OutputSegments = "MuonTruthSegments"
if muonStandaloneFlags.trackBuilder == 'TruthTracking':
    OutputTracks = "MuonSpectrometerTracks"

if muonStandaloneFlags.segmentOrigin == 'TruthTracking':
    OutputSegments = "MuonSegments"

from TrkTruthTrackAlgs.TrkTruthTrackAlgsConf import Trk__TruthTrackCreation
topSequence += Trk__TruthTrackCreation("MuonTruthTrackCreation",
                                       PRD_TruthTrajectoryBuilder=ToolSvc.MuonPRD_TruthTrajectoryBuilder,
                                       TruthTrackBuilder = ToolSvc.MuonTruthTrackBuilder,
                                       OutputTrackCollection = OutputTracks)
