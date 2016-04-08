
# specify database and folders
from AthenaCommon.AppMgr import ServiceMgr as svcMgr
svcMgr.IOVDbSvc.dbConnection="sqlite://X_muon;schema=mycool.db;dbname=OFLP200"
    
from IOVDbSvc.CondDB import conddb
conddb.addFolder("","<dbConnection>sqlite://X_muon;schema=mycool.db;dbname=OFLP200</dbConnection> /MUON/TrkErrorScaling" + "<tag>MuonTrkErrorScaling_nominal</tag>", force=True)
conddb.addFolderWithTag('mycool.db','/MUON/TrkErrorScaling','MuonTrkErrorScaling_nominal')


# configure muon hit creator tools to use error scaling
from MuonRecExample import MuonRecTools

from MuonErrorScalingTools.MuonErrorScalingToolsConf import Muon__MuonErrorScaleDbTool,Muon__MuonRIO_OnTrackErrorScalingTool
muonErrorScaleDbTool = Muon__MuonErrorScaleDbTool("MuonErrorScaleDbTool")
ToolSvc += muonErrorScaleDbTool

muonRIO_OnTrackErrorScalingTool = \
    Muon__MuonRIO_OnTrackErrorScalingTool("MuonRIO_OnTrackErrorScalingTool",
                                          ErrorScaleDbTool=muonErrorScaleDbTool)
                                                                        
ToolSvc += muonRIO_OnTrackErrorScalingTool

muonClusterOnTrackCreator = MuonRecTools.getPublicTool( "MuonClusterOnTrackCreator" )
muonClusterOnTrackCreator.OutputLevel = 1

muonClusterOnTrackCreator.ErrorScalingTool = ToolSvc.MuonRIO_OnTrackErrorScalingTool
muonClusterOnTrackCreator.ErrorScalingTool.OutputLevel = 1
    
mdtDriftCircleOnTrackCreator = MuonRecTools.getPublicTool( "MdtDriftCircleOnTrackCreator" )
mdtDriftCircleOnTrackCreator.DoFixedError = False
mdtDriftCircleOnTrackCreator.DoErrorScaling = True
mdtDriftCircleOnTrackCreator.OutputLevel = 1

mdtDriftCircleOnTrackCreator.ErrorScalingTool = ToolSvc.MuonRIO_OnTrackErrorScalingTool
mdtDriftCircleOnTrackCreator.ErrorScalingTool.OutputLevel = 1
