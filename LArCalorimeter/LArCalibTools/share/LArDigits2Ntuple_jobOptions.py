import commands

include("LArCalibProcessing/LArCalib_Flags.py")
include("RecExCommission/GetInputFiles.py")

if not 'SubDet' in dir():
   SubDet = "Barrel"

if not 'RunNumberList' in dir():
   RunNumberList = [ '0018660' ]
   
if not 'BaseFileName' in dir():
   BaseFileName = "LArDigits"
   
   for RunNumber in RunNumberList :
      BaseFileName = BaseFileName+"_"+str(RunNumber)

if not 'OutputRootFileName' in dir():
   OutputRootFileName = BaseFileName+".root"
   
if not 'FilePrefix' in dir():
   if (int(RunNumberList[0]))<99800 :
      FilePrefix = "daq.Ramp"
   else :
      FilePrefix = "data*"
     
if not 'InputDir' in dir():
   InputDir = "/castor/cern.ch/grid/atlas/DAQ/lar/ElecCalib/2014/"+RunNumberList[0]
   
if not 'FullFileName' in dir():
   if not 'Trigger' in dir():
      if (int(RunNumberList[0]))<99800 :
         Trigger = "*"+Partition
      else :
         if 'Partition' in dir():
            Trigger = "calibration_"+".*"+Partition   
         else:   
            Trigger = "calibration_"+"*"   
   
   FullFileName = []
   for RunNumber in RunNumberList :
       FullFileName+=GetInputFilesFromTokens(InputDir,int(RunNumber),FilePrefix,Trigger)

if not 'Gain' in dir():
   Gain = "HIGH"
   
if not 'EvtMax' in dir():
   EvtMax=-1

if not 'WriteNtuple' in dir():
   WriteNtuple = LArCalib_Flags.WriteNtuple

include ("LArConditionsCommon/LArMinimalSetup.py")
svcMgr.IOVDbSvc.GlobalTag=LArCalib_Flags.globalFlagDB
svcMgr.IOVDbSvc.DBInstance=""

InputDBConnectionBadChannel = "COOLONL_LAR/CONDBR2"
BadChannelsFolder="/LAR/BadChannels/BadChannels"
BadChannelsTagSpec = LArCalibFolderTag (BadChannelsFolder,"-RUN2-UPD1-00")
conddb.addFolder("",BadChannelsFolder+"<tag>"+BadChannelsTagSpec+"</tag><dbConnection>"+InputDBConnectionBadChannel+"</dbConnection>")
MissingFEBsFolder="/LAR/BadChannels/MissingFEBs"
MissingFEBsTagSpec = LArCalibFolderTag (MissingFEBsFolder,"-RUN2-UPD1-01")
conddb.addFolder("",MissingFEBsFolder+"<tag>"+MissingFEBsTagSpec+"</tag> <dbConnection>"+InputDBConnectionBadChannel+"</dbConnection>")
from AthenaCommon.AlgSequence import AlgSequence 
topSequence = AlgSequence()  

## get a handle to the ApplicationManager, to the ServiceManager and to the ToolSvc
from AthenaCommon.AppMgr import (theApp, ServiceMgr as svcMgr,ToolSvc)

theByteStreamInputSvc=svcMgr.ByteStreamInputSvc
if not 'FullFileName' in dir():
   RampLog.info( "No FullFileName! Please give a FullFileName list." )
   theApp.exit(-1)

else :   
   theByteStreamInputSvc.FullFileName=FullFileName
   
theByteStreamInputSvc.MaxBadEvents=0
svcMgr.ByteStreamCnvSvc.InitCnvs += [ "EventInfo"]

from LArByteStream.LArByteStreamConf import LArRodDecoder
svcMgr.ToolSvc += LArRodDecoder()

theByteStreamAddressProviderSvc =svcMgr.ByteStreamAddressProviderSvc
theByteStreamAddressProviderSvc.TypeNames += ["LArFebHeaderContainer/LArFebHeader"]
theByteStreamAddressProviderSvc.TypeNames += [ "LArDigitContainer/HIGH"  ]
theByteStreamAddressProviderSvc.TypeNames += [ "LArDigitContainer/MEDIUM"]
theByteStreamAddressProviderSvc.TypeNames += [ "LArDigitContainer/LOW"   ]
theByteStreamAddressProviderSvc.TypeNames += [ "LArDigitContainer/FREE"   ]

include ("LArROD/LArFebErrorSummaryMaker_jobOptions.py")       
topSequence.LArFebErrorSummaryMaker.CheckAllFEB=False
from LArCalibDataQuality.LArCalibDataQualityConf import LArBadEventCatcher
theLArBadEventCatcher=LArBadEventCatcher()
theLArBadEventCatcher.CheckAccCalibDigitCont=False
theLArBadEventCatcher.CheckBSErrors=True
theLArBadEventCatcher.KeyList=[Gain]
theLArBadEventCatcher.StopOnError=False
topSequence+=theLArBadEventCatcher    

from LArCalibTools.LArCalibToolsConf import *

LArDigits2Ntuple=LArDigits2Ntuple("LArDigits2Ntuple")
LArDigits2Ntuple.ContainerKey = Gain
LArDigits2Ntuple.AddFEBTempInfo=False
if 'FTlist' in dir():
   LArDigits2Ntuple.FTlist=FTlist

topSequence+= LArDigits2Ntuple


theApp.HistogramPersistency = "ROOT"
from GaudiSvc.GaudiSvcConf import NTupleSvc
svcMgr += NTupleSvc()
svcMgr.NTupleSvc.Output = [ "FILE1 DATAFILE='"+OutputRootFileName+"' OPT='NEW'" ]

AthenaEventLoopMgr=Service("AthenaEventLoopMgr")
AthenaEventLoopMgr.OutputLevel=ERROR

theApp.EvtMax=EvtMax
svcMgr.MessageSvc.OutputLevel=WARNING

#DetStore=Service("DetectorStore");
#DetStore.dump=TRUE
#from StoreGate.StoreGateConf import StoreGateSvc
#sgStore = StoreGateSvc("StoreGateSvc")
#sgStore.Dump = True
#sgStore.OutputLevel = DEBUG

#dtStore = StoreGateSvc("DetectorStore")
#dtStore.Dump = True
#dtStore.OutputLevel = DEBUG

