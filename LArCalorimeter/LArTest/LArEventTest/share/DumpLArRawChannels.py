# Location & number of raw data file:
runnumber=1000669
RawDataDir="/castor/cern.ch/atlas/testbeam/combined/2004/"
RawDataFilePrefix="daq_SFI-51_calo"
#Gain, set HIGH,MEDIUM,LOW or FREE
GainKey="FREE" 
# Number of events to be processed (default is 10)
theApp.EvtMax = 2


# ***************************************************************
include( "ByteStreamCnvSvc/TBEventSelector_jobOptions.py" )
ByteStreamAddressProviderSvc = Service( "ByteStreamAddressProviderSvc" )
ByteStreamAddressProviderSvc.TypeNames += ["LArRawChannelContainer/LArRawChannels"]
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
MessageSvc.OutputLevel =3
AthenaEventLoopMgr = Service ("AthenaEventLoopMgr")
AthenaEventLoopMgr.OutputLevel=4

theApp.Dlls += [ "LArEventTest"]
theApp.topAlg+=["DumpLArRawChannels"]
DumpLArRawChannels=Algorithm("DumpLArRawChannels")
DumpLArRawChannels.LArRawChannelContainerName = "LArRawChannels" 
DumpLArRawChannels.OutputFileName="LArRawChannels.txt"
