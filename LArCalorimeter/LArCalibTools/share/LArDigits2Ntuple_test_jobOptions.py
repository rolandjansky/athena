# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
import os


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
   
if not 'OutputDir' in dir():
   OutputDir = os.getcwd()

if not 'SCDecodeAllContainers' in dir():
   SCDecodeAllContainers=False

if not 'SCProtectSourceId' in dir():
   SCProtectSourceId=False

if not 'FilePrefix' in dir():
   if (int(RunNumberList[0]))<99800 :
      FilePrefix = "daq.Ramp"
   else :
      FilePrefix = "data*"
     
if not 'InputDir' in dir():
   InputDir = "/castor/cern.ch/grid/atlas/DAQ/lar/ElecCalib/2014/"+RunNumberList[0]

if not 'FillLatomeSourceID' in dir():
   FillLatomeSourceID=-1
if not 'OverwriteEventNumber' in dir():
   OverwriteEventNumber=False

if not 'BSConverter_Add_BCID_LATOMEID' in dir():
   BSConverter_Add_BCID_LATOMEID=False
   
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

if not 'SuperCells' in dir():
   SuperCells=False

if not SuperCells: include("LArCalibProcessing/LArCalib_Flags.py")
else: include("LArCalibProcessing/LArCalib_FlagsSC.py")
include("RecExCommission/GetInputFiles.py")

if SuperCells:
   from AthenaCommon.GlobalFlags import globalflags
   globalflags.DetGeo.set_Value_and_Lock('atlas')
   globalflags.Luminosity.set_Value_and_Lock('zero')
   globalflags.DataSource.set_Value_and_Lock('data')
   globalflags.InputFormat.set_Value_and_Lock('bytestream')
   globalflags.DatabaseInstance.set_Value_and_Lock('CONDBR2')
   
   from AthenaCommon.JobProperties import jobproperties
   jobproperties.Global.DetDescrVersion = "ATLAS-R2-2016-01-00-01"
   
   from AthenaCommon.DetFlags import DetFlags
   DetFlags.Calo_setOn()  
   DetFlags.ID_setOff()
   DetFlags.Muon_setOff()
   DetFlags.Truth_setOff()
   DetFlags.LVL1_setOff()
   DetFlags.digitize.all_setOff()
   #DetFlags.Print()
   
   #Set up GeoModel (not really needed but crashes without)
   from AtlasGeoModel import SetGeometryVersion
   from AtlasGeoModel import GeoModelInit
   
   #Get identifier mapping
   include( "LArConditionsCommon/LArIdMap_comm_jobOptions.py" )
   include( "LArIdCnv/LArIdCnv_joboptions.py" )
   include( "ByteStreamCnvSvc/BSEventStorageEventSelector_jobOptions.py" )
else:
   include ("LArConditionsCommon/LArMinimalSetup.py")

from LArCabling.LArCablingAccess import  LArOnOffIdMapping,LArFebRodMapping,LArCalibIdMapping
LArOnOffIdMapping()
LArFebRodMapping()
LArCalibIdMapping()
if SuperCells:
   from LArCabling.LArCablingAccess import  LArOnOffIdMappingSC, LArCalibIdMappingSC
   LArOnOffIdMappingSC()
   LArCalibIdMappingSC()


svcMgr.IOVDbSvc.GlobalTag=LArCalib_Flags.globalFlagDB
svcMgr.IOVDbSvc.DBInstance=""

# from LArBadChannelTool.LArBadChannelAccess import LArBadChannelAccess

if 'BadChannelsFolder' not in dir():
   BadChannelsFolder="/LAR/BadChannels/BadChannels"
if 'MissingFEBsFolder' not in dir():
   MissingFEBsFolder="/LAR/BadChannels/MissingFEBs"

if not 'ReadBadChannelFromCOOL' in dir():
   ReadBadChannelFromCOOL = True   

if ( ReadBadChannelFromCOOL ):      
   if 'InputBadChannelSQLiteFile' in dir():
      InputDBConnectionBadChannel = DBConnectionFile(InputBadChannelSQLiteFile)
   else:
      if 'InputDBConnectionBadChannel' not in dir():
         InputDBConnectionBadChannel = "COOLOFL_LAR/" + conddb.dbname

if 'BadChannelsLArCalibFolderTag' in dir() :
   BadChannelsTagSpec = LArCalibFolderTag (BadChannelsFolder,BadChannelsLArCalibFolderTag) 
   conddb.addFolder("",BadChannelsFolder+"<tag>"+BadChannelsTagSpec+"</tag>"+"<dbConnection>"+InputDBConnectionBadChannel+"</dbConnection>",
                    className="CondAttrListCollection")
else :
   conddb.addFolder("",BadChannelsFolder+"<dbConnection>"+InputDBConnectionBadChannel+"</dbConnection>",className="CondAttrListCollection")


if 'MissingFEBsLArCalibFolderTag' in dir() :
   MissingFEBsTagSpec = LArCalibFolderTag (MissingFEBsFolder,MissingFEBsLArCalibFolderTag)   
   conddb.addFolder("",MissingFEBsFolder+"<tag>"+MissingFEBsTagSpec+"</tag>"+"<dbConnection>"+InputDBConnectionBadChannel+"</dbConnection>",className='AthenaAttributeList')
else :
   conddb.addFolder("",MissingFEBsFolder+"<dbConnection>"+InputDBConnectionBadChannel+"</dbConnection>",className='AthenaAttributeList')


from LArBadChannelTool.LArBadChannelToolConf import LArBadChannelCondAlg, LArBadFebCondAlg
theLArBadChannelCondAlg=LArBadChannelCondAlg(ReadKey=BadChannelsFolder)
condSeq+=theLArBadChannelCondAlg

theLArBadFebCondAlg=LArBadFebCondAlg(ReadKey=MissingFEBsFolder)
condSeq+=theLArBadFebCondAlg

if SuperCells:
   conddb.addFolder("","/LAR/IdentifierOfl/OnOffIdMap_SC<db>COOLOFL_LAR/OFLP200</db><tag>LARIdentifierOflOnOffIdMap_SC-000</tag>")




   

from AthenaCommon.AlgSequence import AlgSequence 
topSequence = AlgSequence()  


## get a handle to the ApplicationManager, to the ServiceManager and to the ToolSvc
from AthenaCommon.AppMgr import (theApp, ServiceMgr as svcMgr,ToolSvc)

theByteStreamInputSvc=svcMgr.ByteStreamInputSvc
if not 'FullFileName' in dir():
   RampLog.info( "No FullFileName! Please give a FullFileName list." )
   theApp.exit(-1)

else :   
   svcMgr.EventSelector.Input=FullFileName
   
svcMgr.EventSelector.MaxBadEvents = 0

svcMgr.ByteStreamCnvSvc.InitCnvs += [ "EventInfo"]

theByteStreamAddressProviderSvc =svcMgr.ByteStreamAddressProviderSvc

if not SuperCells:
   from LArByteStream.LArByteStreamConf import LArRodDecoder
   svcMgr.ToolSvc += LArRodDecoder()
   theByteStreamAddressProviderSvc.TypeNames += ["LArFebHeaderContainer/LArFebHeader"]
   theByteStreamAddressProviderSvc.TypeNames += [ "LArDigitContainer/HIGH"  ]
   theByteStreamAddressProviderSvc.TypeNames += [ "LArDigitContainer/MEDIUM"]
   theByteStreamAddressProviderSvc.TypeNames += [ "LArDigitContainer/LOW"   ]
   theByteStreamAddressProviderSvc.TypeNames += [ "LArDigitContainer/FREE"   ]

   # this will go outside SC loop
   include ("LArROD/LArFebErrorSummaryMaker_jobOptions.py")       
   topSequence.LArFebErrorSummaryMaker.CheckAllFEB=False

   from LArCalibDataQuality.LArCalibDataQualityConf import LArBadEventCatcher
   theLArBadEventCatcher=LArBadEventCatcher()
   theLArBadEventCatcher.CheckAccCalibDigitCont=False
   theLArBadEventCatcher.CheckBSErrors=True
   theLArBadEventCatcher.KeyList=[Gain]
   theLArBadEventCatcher.StopOnError=False
   topSequence+=theLArBadEventCatcher    
else:
   from LArByteStream.LArByteStreamConf import LArLATOMEDecoder 
   from LArByteStream.LArByteStreamConf import LArRawSCDataReadingAlg
   LArRawSCDataReadingAlg = LArRawSCDataReadingAlg()
   LArRawSCDataReadingAlg.LATOMEDecoder = LArLATOMEDecoder("LArLATOMEDecoder")
   LArRawSCDataReadingAlg.LATOMEDecoder.latomeInfoFileName = LatomeInfo
   LArRawSCDataReadingAlg.LATOMEDecoder.DumpFile = SC_DumpFile
   LArRawSCDataReadingAlg.LATOMEDecoder.RawDataFile = SC_RawDataFile
   LArRawSCDataReadingAlg.LATOMEDecoder.ProtectSourceId = SCProtectSourceId
   topSequence+=LArRawSCDataReadingAlg

from LArCalibTools.LArCalibToolsConf import *

LArDigits2Ntuple=LArDigits2Ntuple("LArDigits2Ntuple")
contkeys = []
contkeys.append(Gain)


if SuperCells and SCDecodeAllContainers:
   contkeys.append("SC_ADC_BAS")
   contkeys.append("SC_ET")
   contkeys.append("SC_ET_ID")
   contkeys.append("SC_LATOME_HEADER")
LArDigits2Ntuple.ContainerKeys = contkeys
LArDigits2Ntuple.AddFEBTempInfo=False
if 'FTlist' in dir():
   LArDigits2Ntuple.FTlist=FTlist

LArDigits2Ntuple.isSC = SuperCells

if SuperCells:
   LArDigits2Ntuple.RealGeometry = True
   LArDigits2Ntuple.OffId = True
   LArDigits2Ntuple.FillBCID = True
   LArDigits2Ntuple.AddBadChannelInfo = False
   LArDigits2Ntuple.OverwriteEventNumber = OverwriteEventNumber
else:
   #LArDigits2Ntuple.RealGeometry = True
   #LArDigits2Ntuple.OverwriteEventNumber = OverwriteEventNumber
   LArDigits2Ntuple.FillBCID = True


topSequence+= LArDigits2Ntuple

theApp.HistogramPersistency = "ROOT"
from GaudiSvc.GaudiSvcConf import NTupleSvc
svcMgr += NTupleSvc()
svcMgr.NTupleSvc.Output = [ "FILE1 DATAFILE='"+OutputDir + "/" +OutputRootFileName+"' OPT='NEW'" ]

AthenaEventLoopMgr=Service("AthenaEventLoopMgr")
AthenaEventLoopMgr.OutputLevel=WARNING

theApp.EvtMax=EvtMax
svcMgr.MessageSvc.OutputLevel=WARNING

LArDigits2Ntuple.OutputLevel=WARNING

#DetStore=Service("DetectorStore");
#DetStore.dump=TRUE
#from StoreGate.StoreGateConf import StoreGateSvc
#sgStore = StoreGateSvc("StoreGateSvc")
#sgStore.Dump = True
#sgStore.OutputLevel = DEBUG

#dtStore = StoreGateSvc("DetectorStore")
#dtStore.Dump = True
#dtStore.OutputLevel = DEBUG

from AthenaCommon.AlgSequence import *
dumpSequence(topSequence)

svcMgr.StoreGateSvc.Dump = True;
