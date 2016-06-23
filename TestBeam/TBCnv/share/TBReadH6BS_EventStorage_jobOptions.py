#**************************************************************
#
# jopOptions file for reading EventStorage
#
#==============================================================
# Input
ToolSvc = Service( "ToolSvc" )
#from AthenaCommon.AppMgr import ToolSvc

# H6:
#include( "ByteStreamCnvSvc/ByteStreamSelector_jobOptions.py")
include( "ByteStreamCnvSvc/TBEventSelector_jobOptions.py" )

include( "ByteStreamCnvSvcBase/BSAddProvSvc_RDO_jobOptions.py" )



theApp.Dlls +=["TBCnv", "DBDataModelAthenaPoolPoolCnv" ]
#StreamBS.ItemList +=["TBTDC#*"]; 
#StreamBS.ItemList +=["TBBPCRawCont#*"]; 
#StreamBS.ItemList +=["TBTriggerPatternUnit#*"];
from TBCnv.TBCnvConf import  TBByteStreamCnvTool
ToolSvc += TBByteStreamCnvTool()
ToolSvc.TBByteStreamCnvTool.ForceHchoice = TRUE
ToolSvc.TBByteStreamCnvTool.isH6Run = TRUE
ToolSvc.TBByteStreamCnvTool.isH8Run = FALSE
#ToolSvc.TBByteStreamCnvTool.SubDetID = 118 # = 0x76
#ToolSvc.TBByteStreamCnvTool.SubDetID = 112 # = 0x70
#ToolSvc.TBByteStreamCnvTool.SubDetID = 129 # = 0x81
ToolSvc.TBByteStreamCnvTool.SubDetID = 112
ToolSvc.TBByteStreamCnvTool.Keys += ["TBTDC/TBTDC"]                               
ToolSvc.TBByteStreamCnvTool.Keys += ["TBTDCRawCont/TDCRawCont"]                   
ToolSvc.TBByteStreamCnvTool.Keys += ["TBADCRawCont/ADCRawCont"]                   
ToolSvc.TBByteStreamCnvTool.Keys += ["TBScintillatorRawCont/ScintillatorRawCont"] 
ToolSvc.TBByteStreamCnvTool.Keys += ["TBBPCRawCont/BPCRawCont"]
ToolSvc.TBByteStreamCnvTool.Keys += ["TBMWPCRawCont/MWPCRawCont"]
ToolSvc.TBByteStreamCnvTool.Keys += ["TBTriggerPatternUnit/TBTrigPat"]              
ToolSvc.TBByteStreamCnvTool.Keys += ["TBTailCatcherRaw/TailCatcherRaw"]
ToolSvc.TBByteStreamCnvTool.Keys += ["TBEventInfo/TBEventInfo"]
ToolSvc.TBByteStreamCnvTool.Keys += ["TBLArDigitContainer/FREE"]
ToolSvc.TBByteStreamCnvTool.Keys += ["TBLArDigitContainer/LOW"]
ToolSvc.TBByteStreamCnvTool.Keys += ["TBLArDigitContainer/HIGH"]
ToolSvc.TBByteStreamCnvTool.Keys += ["TBLArDigitContainer/MEDIUM"]

svcMgr.ByteStreamAddressProviderSvc.TypeNames += ToolSvc.TBByteStreamCnvTool.Keys

# ByteStreamAddressProviderSvc.TypeNames += ["TBTDC/TBTDC"]
# ByteStreamAddressProviderSvc.TypeNames += ["TBTDCRawCont/TDCRawCont"]
# ByteStreamAddressProviderSvc.TypeNames += ["TBADCRawCont/ADCRawCont"]
# ByteStreamAddressProviderSvc.TypeNames += ["TBScintillatorRawCont/ScintillatorRawCont"] 
# ByteStreamAddressProviderSvc.TypeNames += ["TBBPCRawCont/BPCRawCont"] 
# ByteStreamAddressProviderSvc.TypeNames += ["TBTriggPatternUnit/TBTrigPat"]
# ByteStreamAddressProviderSvc.TypeNames += ["TBTailCatcherRaw/TailCatcherRaw"]           

#force creation of Converter in init
# ByteStreamCnvSvc = Service( "ByteStreamCnvSvc" )
# ByteStreamCnvSvc.InitCnvs += [  "TBTDC",
# 				"TBBPCRawCont",
# 				"TBTriggPatternUnit"
#  ] ; 
# theApp.ExtSvc += [ "ByteStreamCnvSvc" ] 

