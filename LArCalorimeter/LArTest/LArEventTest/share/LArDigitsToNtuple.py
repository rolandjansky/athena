# Location & number of raw data file:
#runnumber=4462
#runnumber=2102986
#runnumber=1001054
#runnumber=5134
RunNumber=1000990
#runnumber=2102102
RawDataDir="/castor/cern.ch/atlas/testbeam/combined/2004/"
#RawDataDir="/castor/cern.ch/atlas/testbeam/lar/2004/"
#RawDataFilePrefix="daq_ROS-41_LargCalib"
#RawDataFilePrefix="daq_SFO-51_combined"
#RawDataFilePrefix="daq_SFI-51_combined"
RawDataFilePrefix="daq_SFI-51_calo"
#Gain, set HIGH,MEDIUM,LOW or FREE
GainKey="HIGH"
# Number of events to be processed (default is 10)
theApp.EvtMax = 200000
#theApp.EvtMax = 20

# ***************************************************************
include( "ByteStreamCnvSvc/TBEventSelector_jobOptions.py" )
ByteStreamAddressProviderSvc = Service( "ByteStreamAddressProviderSvc" )
ByteStreamAddressProviderSvc.TypeNames += ["LArRawChannelContainer/LArRawChannels"]
ByteStreamAddressProviderSvc.TypeNames += ["LArDigitContainer/HIGH"] 
ByteStreamAddressProviderSvc.TypeNames += ["LArDigitContainer/MEDIUM"] 
ByteStreamAddressProviderSvc.TypeNames += ["LArDigitContainer/LOW"] 
ByteStreamAddressProviderSvc.TypeNames += ["LArDigitContainer/FREE"] 
ByteStreamAddressProviderSvc.TypeNames += ["LArFebHeaderContainer/LArFebHeader"]
include( "LArDetMgrDetDescrCnv/LArDetMgrDetDescrCnv_H8_joboptions.py" )

if GainKey=="FREE":
  ToolSvc.LArRodDecoder.FirstSample=2

# Directory & Prefix & Runnumber 
ByteStreamInputSvc=Service("ByteStreamInputSvc")
ByteStreamInputSvc.InputDirectory += [RawDataDir]
ByteStreamInputSvc.FilePrefix += [RawDataFilePrefix]
ByteStreamInputSvc.RunNumber = [RunNumber]

ToolSvc = Service( "ToolSvc" )
#Fix for swapped LArFEBs
if (RunNumber>=1000931):
  if (RunNumber<=1000969):
    ToolSvc.LArRodDecoder.FebExchange=True
    ToolSvc.LArRodDecoder.FebId1=0x39020000
    ToolSvc.LArRodDecoder.FebId2=0x39040000
    
# Set output level threshold (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
MessageSvc = Service( "MessageSvc" )
MessageSvc.OutputLevel = INFO
#AthenaEventLoopMgr = Service ("AthenaEventLoopMgr")
#AthenaEventLoopMgr.OutputLevel= INFO

#----- JobOptions for Ntuple-output
theApp.Dlls += [ "RootHistCnv" ]; 
theApp.HistogramPersistency = "ROOT" 
HistogramPersistencySvc = Service( "HistogramPersistencySvc" )
HistogramPersistencySvc.OutputFile  = "histo.root"
NtupleSvc = Service( "NtupleSvc" )
NtupleSvc.Output=["FILE1 DATAFILE='lardigits.root' TYP='ROOT' OPT='NEW'"]

#Top algorithm

include( "TBCnv/TBReadH8BS_jobOptions.py" )
doTBRec=False
if doTBRec:
  theApp.Dlls += [ "TBRec"]
  theApp.TopAlg += ["TBScintillatorRec"]
  TBScintRec =  Algorithm("TBScintillatorRec")
  TBScintRec.SGkey="ScintillatorRawCont"
  include( "TBRec/H8ScintRec_jobOptions.py" )
  theApp.TopAlg += ["TBPhaseRec/TBPhaseRec" ]
  TBPhaseRec = Algorithm( "TBPhaseRec" )
  include( "TBRec/H8PhaseRec_jobOptions.py" )
  TBPhaseRec.OutputLevel = INFO
theApp.Dlls += [ "LArEventTest"]
theApp.topAlg+=["LArDigitsToNtuple"]
LArDigitsToNtuple=Algorithm("LArDigitsToNtuple")
LArDigitsToNtuple.ContainerKey = GainKey
LArDigitsToNtuple.NSamples=6
LArDigitsToNtuple.accept=14
LArDigitsToNtuple.reject=1
LArDigitsToNtuple.ReadPhase=0
LArDigitsToNtuple.ReadScint=0
LArDigitsToNtuple.ReadSCA=0
LArDigitsToNtuple.ReadPedestal=1
#theApp.TopAlg += ["CheckLArFebHeader"]

include("LArCondCnv/LArCondCnv_TB04_jobOptions.py")

LArCondCnvDbServer = 'atlobk02.cern.ch'
#LArCondCnvDbServer = "atlasdev1.cern.ch"
# Tags for CondDB folders
LArTB04FolderTag_Pedestal = "TB04-Default"
#LArTB04FolderTag_Pedestal = "TB04-RndTrig-2"
doSim=False
include( "RecExTB/RecExTB_CondDB.py" )

EventSelector = Service("EventSelector")
EventSelector.SkipEvents = 1

#StoreGateSvc = Service( "StoreGateSvc" )
#StoreGateSvc.dump=TRUE
