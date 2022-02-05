GainKey="FREE" 
# Number of events to be processed (default is 10)
theApp.EvtMax = 2


# ***************************************************************
include( "ByteStreamCnvSvc/ByteStreamSelector_jobOptions.py" )
ByteStreamAddressProviderSvc = Service( "ByteStreamAddressProviderSvc" )
ByteStreamAddressProviderSvc.TypeNames += ["LArRawChannelContainer/LArRawChannels"]
ByteStreamAddressProviderSvc.TypeNames += ["LArDigitContainer/FREE"] 
include( "LArDetMgrDetDescrCnv/LArDetMgrDetDescrCnv_H8_joboptions.py" )

if GainKey=="FREE":
  ToolSvc.LArRodDecoder.FirstSample=2

# Set output level threshold (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
MessageSvc = Service( "MessageSvc" )
MessageSvc.OutputLevel =DEBUG
AthenaEventLoopMgr = Service ("AthenaEventLoopMgr")
AthenaEventLoopMgr.OutputLevel=4

theApp.Dlls += [ "LArEventTest"]
theApp.topAlg+=["DumpLArDigits"]
DumpLArDigits=Algorithm("DumpLArDigits")
DumpLArDigits.LArDigitContainerName = "FREE" 
DumpLArDigits.OutputFileName="LArDigits.txt"

## theApp.Dlls+= ["LArROD"]
## theApp.TopAlg += ["ReadLArDigits"]
## ReadLArDigits = Algorithm( "ReadLArDigits" )
## ReadLArDigits.ContainerKey=GainKey;


## theApp.Dlls += [ "RootHistCnv" ]; 
## theApp.HistogramPersistency = "ROOT" 
## NtupleSvc = Service( "NtupleSvc" )
## NtupleSvc.Output=["FILE1 DATAFILE='dummyfile.root' TYP='ROOT' OPT='NEW'"]
