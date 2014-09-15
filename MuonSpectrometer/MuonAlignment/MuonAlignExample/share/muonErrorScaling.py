outputLevel=INFO

from IOVDbSvc.CondDB import conddb
IOVDbSvc.dbConnection="sqlite://X;schema=mycool.db;dbname=OFLP200"
conddb.addFolder("","<dbConnection>sqlite://X;schema=mycool.db;dbname=OFLP200</dbConnection> /MUON/TrkErrorScaling" + "<tag>MuonTrkErrorScaling_nominal</tag>", force=True)
conddb.addFolderWithTag('mycool.db','/MUON/TrkErrorScaling','MuonTrkErrorScaling_nominal')

from MuonErrorScalingTools.MuonErrorScalingToolsConf import Muon__MuonRIO_OnTrackErrorScalingTool,Muon__MuonErrorScaleDbTool
muonErrorScaleDbTool = Muon__MuonErrorScaleDbTool("MuonErrorScaleDbTool",
                                                  OutputLevel=outputLevel)
ToolSvc += muonErrorScaleDbTool
muonErrorScalingTool = Muon__MuonRIO_OnTrackErrorScalingTool("MuonRIO_OnTrackErrorScalingTool",
                                                             OutputLevel=outputLevel,
                                                             ErrorScaleDbTool=muonErrorScaleDbTool)
ToolSvc += muonErrorScalingTool

from MuonRecExample import MuonRecTools
MdtDriftCircleOnTrackCreator  = MuonRecTools.getPublicTool("MdtDriftCircleOnTrackCreator")
MdtDriftCircleOnTrackCreator.DoErrorScaling = True
MdtDriftCircleOnTrackCreator.ScaleErrorsManually=False
MdtDriftCircleOnTrackCreator.OutputLevel = outputLevel
MdtDriftCircleOnTrackCreator.ErrorScalingTool=muonErrorScalingTool
    
CscClusterOnTrackCreator = MuonRecTools.getPublicTool("CscClusterOnTrackCreator")
CscClusterOnTrackCreator.DoRpcErrorScaling = True
CscClusterOnTrackCreator.DoTgcErrorScaling = True
CscClusterOnTrackCreator.DoCscErrorScaling = True
CscClusterOnTrackCreator.OutputLevel = outputLevel
CscClusterOnTrackCreator.ErrorScalingTool=muonErrorScalingTool
    
MuonClusterOnTrackCreator = MuonRecTools.getPublicTool("MuonClusterOnTrackCreator")
MuonClusterOnTrackCreator.DoRpcErrorScaling = True
MuonClusterOnTrackCreator.DoTgcErrorScaling = True
MuonClusterOnTrackCreator.DoCscErrorScaling = True
MuonClusterOnTrackCreator.OutputLevel = outputLevel
MuonClusterOnTrackCreator.ErrorScalingTool=muonErrorScalingTool

MuonSegmentTrackBuilder = MuonRecTools.getPublicTool("MuonSegmentTrackBuilder")
MuonSegmentTrackBuilder.OutputLevel = outputLevel

#CscBroadClusterOnTrackCreator
#FixedErrorMuonClusterOnTrackCreator
