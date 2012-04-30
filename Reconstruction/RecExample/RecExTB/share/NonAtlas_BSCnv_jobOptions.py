ToolSvc = Service( "ToolSvc" )
theApp.Dlls +=["TBCnv"]
StreamBS = Algorithm( "StreamBS" )
StreamBS.ItemList +=["TBTDC#*"]; 
StreamBS.ItemList +=["TBBPCRawCont#*"]; 
StreamBS.ItemList +=["TBTriggerPatternUnit#*"]; 

ToolSvc.TBByteStreamCnvTool.ForceHchoice = TRUE
ToolSvc.TBByteStreamCnvTool.isH6Run = FALSE
ToolSvc.TBByteStreamCnvTool.isH8Run = TRUE
#ToolSvc.TBByteStreamCnvTool.SubDetID = 118 # = 0x76
#ToolSvc.TBByteStreamCnvTool.SubDetID = 129 # = 0x81
ToolSvc.TBByteStreamCnvTool.SubDetID = 118
ToolSvc.TBByteStreamCnvTool.Keys += ["TBTDC/TBTDC"]                               
ToolSvc.TBByteStreamCnvTool.Keys += ["TBTDCRawCont/TDCRawCont"]                   
ToolSvc.TBByteStreamCnvTool.Keys += ["TBADCRawCont/ADCRawCont"]                   
ToolSvc.TBByteStreamCnvTool.Keys += ["TBScintillatorRawCont/ScintillatorRawCont"] 
ToolSvc.TBByteStreamCnvTool.Keys += ["TBBPCRawCont/BPCRawCont"]                   
ToolSvc.TBByteStreamCnvTool.Keys += ["TBTriggPatternUnit/TBTrigPat"]              
ToolSvc.TBByteStreamCnvTool.Keys += ["TBTailCatcherRaw/TailCatcherRaw"]

ByteStreamAddressProviderSvc = Service( "ByteStreamAddressProviderSvc" )
ByteStreamAddressProviderSvc.TypeNames += ToolSvc.TBByteStreamCnvTool.Keys

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
#                               "TBBPCRawCont",
#                               "TBTriggPatternUnit"
#  ] ; 
# theApp.ExtSvc += [ "ByteStreamCnvSvc" ] 

#MessageSvc = Service( "MessageSvc" )
#MessageSvc.OutputLevel      = 3
#theApp.EvtMax = 5
#ApplicationMgr.HistogramPersistency="HBOOK";
#NTupleSvc.Output    = { "FILE1 DATAFILE='bs_read.ntup' OPT='NEW'" };

ToolSvc.TBByteStreamCnvTool.Dump = FALSE
