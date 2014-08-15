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
#print "I want LARBadChannelsOflBadChannels-UPD4-00."
conddb.addFolderWithTag('LAR_OFL','/LAR/BadChannelsOfl/BadChannels','LARBadChannelsOflBadChannels-UPD4-00',forceData=True)
conddb.addFolderWithTag('LAR_OFL','/LAR/BadChannelsOfl/MissingFEBs','LARBadChannelsOflMissingFEBs-UPD4-01',forceData=True)
if not hasattr (ServiceMgr.ToolSvc, 'LArBadChanTool'):
    from LArBadChannelTool.LArBadChannelToolConf import LArBadChanTool
    ServiceMgr.ToolSvc += LArBadChanTool()
ServiceMgr.ToolSvc.LArBadChanTool.CoolFolder="/LAR/BadChannelsOfl/BadChannels"
ServiceMgr.ToolSvc.LArBadChanTool.CoolMissingFEBsFolder="/LAR/BadChannelsOfl/MissingFEBs"
##Tile####################################################################################
pass
##Muons###################################################################################
from MDT_Digitization.MDT_DigitizationConf import MdtDigitizationTool as mdt
mdt.UseDeadChamberSvc = True

