# Location & number of raw data file:
runnumber=3004171
#RawDataDir="/castor/cern.ch/atlas/testbeam/combined/2004/"
RawDataDir="/castor/cern.ch/atlas/testbeam/lar/2004/"
#RawDataDir="/afs/cern.ch/user/l/lafaye/scratch0/data"
RawDataFilePrefix="daq_ROS-41_LargCalib"
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


ToolSvc = Service( "ToolSvc" )
ToolSvc.LArRoI_Map.Print=FALSE
# Set output level threshold (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
MessageSvc = Service( "MessageSvc" )
MessageSvc.OutputLevel =3
AthenaEventLoopMgr = Service ("AthenaEventLoopMgr")
AthenaEventLoopMgr.OutputLevel=4

#Necessary DLL's 
theApp.Dlls += [ "LArRawUtils","LArTools"]
theApp.Dlls += [ "LArByteStream"]
#theApp.Dlls += [ "CaloDetMgrDetDescrCnv" ]

theApp.Dlls += [ "LArEventTest"]
theApp.topAlg+=["DumpLArDigits"]
DumpLArDigits=Algorithm("DumpLArDigits")
DumpLArDigits.LArDigitContainerName = "FREE" 
DumpLArDigits.OutputFileName="LArDigits.txt"
