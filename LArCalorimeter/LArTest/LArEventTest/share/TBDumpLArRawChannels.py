# Location & number of raw data file:
runnumber=1000185
RawDataDir="/castor/cern.ch/atlas/testbeam/combined/2004/"
RawDataFilePrefix="daq_SFI-51_calo"
#Gain, set HIGH,MEDIUM,LOW or FREE
GainKey="FREE" 
# Number of events to be processed (default is 10)
theApp.EvtMax = 100
#Result location (full path name, no ~ and trailing / please!)
ResultDir=""


# ***************************************************************
include( "ByteStreamCnvSvc/TBEventSelector_jobOptions.py" )

ByteStreamAddressProviderSvc = Service( "ByteStreamAddressProviderSvc" )
ByteStreamAddressProviderSvc.TypeNames += ["LArDigitContainer/HIGH"] 
ByteStreamAddressProviderSvc.TypeNames += ["LArDigitContainer/MEDIUM"] 
ByteStreamAddressProviderSvc.TypeNames += ["LArDigitContainer/LOW"] 
ByteStreamAddressProviderSvc.TypeNames += ["LArDigitContainer/FREE"] 
include( "LArDetMgrDetDescrCnv/LArDetMgrDetDescrCnv_H8_joboptions.py" )

if GainKey=="FREE":
  ToolSvc.LArRodDecoder.FirstSample=2


# Directory & Prefix & Runnumber 
ByteStreamInputSvc=Service("ByteStreamInputSvc")
ByteStreamInputSvc.InputDirectory += [RawDataDir]
ByteStreamInputSvc.FilePrefix += [RawDataFilePrefix]
ByteStreamInputSvc.RunNumber = [runnumber]


# Set output level threshold (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
MessageSvc = Service( "MessageSvc" )
MessageSvc.OutputLevel = DEBUG
AthenaEventLoopMgr = Service ("AthenaEventLoopMgr")
AthenaEventLoopMgr.OutputLevel=4

from LArRecUtils.LArADC2MeVCondAlgDefault import LArADC2MeVCondAlgDefault
LArADC2MeVCondAlgDefault()

theApp.topAlg+=["LArRawChannelSimpleBuilder/TBLArRawChannelBuilder"]
### Liquid argon reco
TBLArRawChannelBuilder = Algorithm("TBLArRawChannelBuilder");
TBLArRawChannelBuilder.RecoMode    = "FIXED"
TBLArRawChannelBuilder.maxSamp=2;
TBLArRawChannelBuilder.CubicAdcCut = 50.
TBLArRawChannelBuilder.DataLocation=GainKey


theApp.Dlls += [ "LArEventTest"]
theApp.topAlg+=["DumpLArRawChannels"]
DumpLArRawChannels=Algorithm("DumpLArRawChannels")
DumpLArRawChannels.LArRawChannelContainerName = "LArRawChannels" 
DumpLArRawChannels.OutputFileName="LArRawChannels.txt"
