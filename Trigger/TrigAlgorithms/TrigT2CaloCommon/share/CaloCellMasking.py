include.block("TrigT2CaloCommon/CaloCellMasking.py")

from AthenaCommon.AppMgr import ToolSvc #if not already imported
from LArBadChannelTool.LArBadChannelToolConf import LArBadFebMasker, LArBadChannelMasker

#The masker tool name must be "LArBadFebMasker", the same as retrieved in LArCellCont.cxx
ToolSvc+=LArBadFebMasker("LArBadFebMasker")
ToolSvc.LArBadFebMasker.DoMasking = False #Masking can easily be turned on and off here.
#ToolSvc.LArBadFebMasker.OutputLevel=DEBUG

#The masker tool name must be "LArBadChannelMasker", the same as retrieved in LArCellCont.cxx
ToolSvc+=LArBadChannelMasker("LArBadChannelMasker")
ToolSvc.LArBadChannelMasker.ProblemsToMask=["unstable", "short", "highNoiseHG", "highNoiseMG","highNoiseLG", "problematicForUnknownReason"] #See LArBadChanBitPacking.cxx for the list of strings that can be used.
ToolSvc.LArBadChannelMasker.DoMasking = False #Masking can easily be turned on and off here.
#ToolSvc.LArBadChannelMasker.OutputLevel=DEBUG

#IOVDbSvc=theApp.service('IOVDbSvc')
#if ( svcMgr.IOVDbSvc.GlobalTag!="COMCOND-002-00" and svcMgr.IOVDbSvc.GlobalTag!="COMCOND-003-00" and svcMgr.IOVDbSvc.GlobalTag!="COMCOND-004-00" and svcMgr.IOVDbSvc.GlobalTag!="COMCOND-005-00" ):
#else:
if not conddb.folderRequested('/LAR/BadChannels/BadChannels') :
   conddb.addFolder(LArDB,"/LAR/BadChannels/BadChannels")
conddb.addFolder(LArDB,"/LAR/BadChannels/BadChannelsOnl")

from TileConditions.TileInfoConfigurator import TileInfoConfigurator
tIC=TileInfoConfigurator()
tIC.setupCOOL()
