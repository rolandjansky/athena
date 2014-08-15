#DEPRECATED DO NOT USE
from AthenaCommon.AlgSequence import AlgSequence
from AthenaCommon.AppMgr import ServiceMgr 
from IOVDbSvc.CondDB import conddb
#force oracle
from DBReplicaSvc.DBReplicaSvcConf import DBReplicaSvc
svcMgr+=DBReplicaSvc(COOLSQLiteVetoPattern="/DBRelease/")
##PIX####################################################################################
pass
##SCT####################################################################################
pass
##TRT####################################################################################
pass
##LAr####################################################################################
#preopts
#conddb.addFolderWithTag('LAR_OFL','/LAR/BadChannelsOfl/BadChannels','LArBadChannelsOflBadChannels-IOVDEP-00',forceData=True)
#conddb.addFolderWithTag('LAR_OFL','/LAR/BadChannelsOfl/MissingFEBs','LArBadChannelsOflMissingFEBs-IOVDEP-00',forceData=True)
#if not hasattr (ServiceMgr.ToolSvc, 'LArBadChanTool'):
#    from LArBadChannelTool.LArBadChannelToolConf import LArBadChanTool
#    ServiceMgr.ToolSvc += LArBadChanTool()
#    ServiceMgr.ToolSvc.LArBadChanTool.CoolFolder="/LAR/BadChannelsOfl/BadChannels"
#    ServiceMgr.ToolSvc.LArBadChanTool.CoolMissingFEBsFolder="/LAR/BadChannelsOfl/MissingFEBs"
conddb.blockFolder('/LAR/BadChannels/BadChannels')
conddb.addFolderWithTag('LAR_OFL','/LAR/BadChannels/BadChannels','LArBadChannelsBadChannels-IOVDEP-01',force=True)
conddb.blockFolder('/LAR/BadChannels/MissingFEBs')
conddb.addFolderWithTag('LAR_OFL','/LAR/BadChannels/MissingFEBs','LArBadChannelsMissingFEBs-IOVDEP-01',force=True)
if not hasattr (ServiceMgr.ToolSvc, 'LArBadChanTool'):
    from LArBadChannelTool.LArBadChannelToolConf import LArBadChanTool
    ServiceMgr.ToolSvc += LArBadChanTool()
    ServiceMgr.ToolSvc.LArBadChanTool.CoolFolder="/LAR/BadChannels/BadChannels"
    ServiceMgr.ToolSvc.LArBadChanTool.CoolMissingFEBsFolder="/LAR/BadChannels/MissingFEBs"
##Tile####################################################################################
pass
##Muons###################################################################################
#from MDT_Digitization.MDT_DigitizationConf import MdtDigitizationTool as mdt
#mdt.UseDeadChamberSvc = True

