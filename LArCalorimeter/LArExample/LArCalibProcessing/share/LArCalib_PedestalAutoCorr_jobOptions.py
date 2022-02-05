# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from future import standard_library
standard_library.install_aliases()
import subprocess

###############################################################################
#
# <Marco.Delmastro@cern.ch>
#
# Example jobOptions to reconstruction one or more pedestal runs from data
#                                                                             
# Last update: 09/12/2008 <Fabien.Tarrade@cern.ch>                
#                                                                         
###############################################################################

if not "SuperCells" in dir():
   SuperCells=False
#if not 'SC_SampleShift' in dir():
#   SC_SampleShift=0

if not SuperCells: include("LArCalibProcessing/LArCalib_Flags.py")
if SuperCells:     include("LArCalibProcessing/LArCalib_FlagsSC.py")
include("LArCalibProcessing/GetInputFiles.py")

#######################################################
#                Run properties
#######################################################

if not 'SubDet' in dir():
   SubDet = "Barrel"
   
if not 'RunNumberList' in dir():
   RunNumberList = [ '0026924' ]

if not 'FilePrefix' in dir():
   if (int(RunNumberList[0]))<99800 :
      FilePrefix = "daq.Pedestal"
   else :
      FilePrefix = "data*"
   
if not 'InputDir' in dir():
   InputDir = "/castor/cern.ch/grid/atlas/DAQ/lar/ElecCalib/2011/"+RunNumberList[0]
            
if not 'Partition' in dir():
   if (SubDet=='EndCap'):
      Partition      = "EB-EC*"
   else:
      Partition      = "EB-EMB*"     

if not 'FullFileName' in dir():
   if 'Trigger' not in dir():
      if (int(RunNumberList[0]))<99800 :
         Trigger = "*"+Partition
      else :
         Trigger = "calibration_LArElec-Pedestal"+".*"+Partition
         # Hack to allow ALL pedestals read properly
         Trigger = "calibration_LArElec-Pedestal"+".*"

      
   FullFileName =[]
   for RunNumber in RunNumberList :
       FullFileName+=GetInputFilesFromTokens(InputDir,int(RunNumber),FilePrefix,Trigger)
 
       
if not 'GainList' in dir():
   if not SuperCells:
      GainList = [ "HIGH", "MEDIUM", "LOW" ]
   else:
      GainList = [ "SC" ]

if not 'GroupingType' in dir():
   if not SuperCells: GroupingType = "ExtendedSubDetector"
   if SuperCells:     GroupingType = "SuperCells"

if not 'ChannelSelection' in dir():
   # read all
   ChannelSelection = " "
   ## example to read only cool for Barrel C : Strips->Back
   #ChannelSelection = "<channelSelection>0,3:34</channelSelection>"   

if not 'runAccumulator' in dir(): 
   runAccumulator = False # averaged mode

if not 'SkipEvents' in dir():
   SkipEvents=0

if not 'doBadCatcher' in dir():
   doBadCatcher=True

from string import *
def DBConnectionFile(sqlitefile):
   return "sqlite://;schema="+sqlitefile+";dbname=CONDBR2"


#######################################################
#                Monitoring properties
#######################################################

if not 'doMonitoring' in dir():
   doMonitoring = True

## LArFEBMon
if not 'doLArFEBMon' in dir():
   doLArFEBMon = True

## LArOddCellMonTool
if not 'doLArOddCellMonTool' in dir():
   doLArOddCellMonTool= False

## LArScaNoiseTool
if not 'doLArScaNoiseTool' in dir():
   doLArScaNoiseTool = False

### LArDigitNoiseMonTool
#if not 'doLArDigitNoiseMonTool' in dir():
#   doLArDigitNoiseMonTool= False
   
if not 'doLArCalibDataQuality' in dir():
   doLArCalibDataQuality = True

## LAr Coherent Noise Mon
if not 'doLArCoherentNoise' in dir():
   doLArCoherentNoise = False


if not 'online' in dir():
   online = False

if 'coherentNoiseFEBS' not in dir():
   coherentNoiseFEBS=[]

if 'coherNoisePartSums' not in dir():
   coherNoisePartSums=False
   
#######################################################
#       Pedestal and AutoCorrelation  properties
#######################################################

if not 'Pedestal' in dir():
   Pedestal = True
   
if not 'AutoCorr' in dir():
   AutoCorr = True
   
if not 'EventsRef' in dir():
   EventsRef     = 0
   
if not 'NSigma' in dir():
   NSigma = 5

if not 'NSamples' in dir():
   NSamples = 32   
   
if not 'WhichSample' in dir():
   WhichSample = -1

if not 'MinSample' in dir():
   MinSample=-1

if not 'MaxSample' in dir():
   MaxSample=-1

#######################################################
#      Pedestal and AutoCorrelation  output name
#######################################################

if not 'WriteNtuple' in dir():
   WriteNtuple = LArCalib_Flags.WriteNtuple

if not 'WritePoolFile' in dir():
   WritePoolFile = LArCalib_Flags.WritePoolFile

if not 'WriteIOV' in dir():
   WriteIOV = LArCalib_Flags.WriteIOV

if not 'IOVBegin' in dir():
   IOVBegin = int(RunNumberList[0])
   
if not 'IOVEnd' in dir():
   IOVEnd = LArCalib_Flags.IOVEnd

if not 'DBConnectionCOOL' in dir():  
   DBConnectionCOOL = "oracle://ATLAS_COOLPROD;schema=ATLAS_COOLOFL_LAR;dbname=CONDBR2;"   

if not 'OutputPedAutoCorrRootFileDir' in dir():
   OutputPedAutoCorrRootFileDir  = subprocess.getoutput("pwd")
   
if not 'OutputPedAutoCorrPoolFileDir' in dir():
   OutputPedAutoCorrPoolFileDir  = subprocess.getoutput("pwd")

if not 'PedLArCalibFolderTag' in dir():
   rs=FolderTagResover()
   PedLArCalibFolderTag = rs.getFolderTagSuffix(LArCalib_Flags.LArPedestalFolder)
   ACLArCalibFolderTag = rs.getFolderTagSuffix(LArCalib_Flags.LArAutoCorrFolder)
   del rs #Close DB connection
   
   
if not 'OutputDB' in dir():
   OutputDB = LArCalib_Flags.OutputDB

if 'OutputSQLiteFile' in dir():
   OutputDB = DBConnectionFile(OutputSQLiteFile)   

if not 'KeyOutputAC' in dir():  
   KeyOutputAC      = "LArAutoCorr" 

if not 'KeyOutputPed' in dir():  
   KeyOutputPed      = "Pedestal" # need Pedestal to merge with oracle, LArPedestal doesn't work

if not 'BaseFileNamePedAutoCorr' in dir():
   BaseFileNamePedAutoCorr = "LArPedAutoCorr"
    
for RunNumber in RunNumberList :
   BaseFileNamePedAutoCorr = BaseFileNamePedAutoCorr + "_"
   BaseFileNamePedAutoCorr = BaseFileNamePedAutoCorr + str(RunNumber)+"_"+Partition.replace("*","") 
   
if not 'OutputPedAutoCorrRootFileName' in dir():
   OutputPedAutoCorrRootFileName  = BaseFileNamePedAutoCorr+ ".root"
    
if not 'OutputPedAutoCorrPoolFileName' in dir():
   OutputPedAutoCorrPoolFileName  = BaseFileNamePedAutoCorr + ".pool.root"

if not 'OutputObjectSpecPed' in dir():
   OutputObjectSpecPed = "LArPedestalComplete#"+KeyOutputPed+"#"+LArCalib_Flags.LArPedestalFolder
   OutputTagSpecPed=LArCalibFolderTag(LArCalib_Flags.LArPedestalFolder,PedLArCalibFolderTag)

if not 'OutputObjectSpecAutoCorr' in dir():
   OutputObjectSpecAutoCorr = "LArAutoCorrComplete#"+KeyOutputAC+"#"+LArCalib_Flags.LArAutoCorrFolder
   OutputTagSpecAutoCorr=LArCalibFolderTag(LArCalib_Flags.LArAutoCorrFolder,ACLArCalibFolderTag)

## Bad Channel   
   
if not 'ReadBadChannelFromCOOL' in dir():
   ReadBadChannelFromCOOL = True   


#######################################################################################
# print summary
#######################################################################################

PedestalAutoCorrLog = logging.getLogger( "PedestalAutoCorrLog" )
PedestalAutoCorrLog.info( " ======================================================== " )
PedestalAutoCorrLog.info( " ***           LAr Pedestal/AutoCorr summary          *** " )
PedestalAutoCorrLog.info( " ======================================================== " )
PedestalAutoCorrLog.info( " RunNumber                         = "+str(RunNumberList) )
PedestalAutoCorrLog.info( " SubDetector                       = "+SubDet )
PedestalAutoCorrLog.info( " Partition                         = "+Partition )
PedestalAutoCorrLog.info( " Type                              = Pedestal " )
PedestalAutoCorrLog.info( " LArGain                           = "+str(GainList) )
for i in range(len(FullFileName)):
   PedestalAutoCorrLog.info( " FullFileName                      = "+FullFileName[i] )
PedestalAutoCorrLog.info( " PedLArCalibFolderTag              = "+PedLArCalibFolderTag )
PedestalAutoCorrLog.info( " OutputPedAutoCorrRootFullFileName = "+OutputPedAutoCorrRootFileDir + "/" + OutputPedAutoCorrRootFileName )
PedestalAutoCorrLog.info( " OutputPedAutoCorrPoolFullFileName    = "+OutputPedAutoCorrPoolFileDir + "/" + OutputPedAutoCorrPoolFileName )
PedestalAutoCorrLog.info( " OutputObjectSpecPed               = "+OutputObjectSpecPed )
PedestalAutoCorrLog.info( " OutputTagSpecPed                  = "+OutputTagSpecPed )
PedestalAutoCorrLog.info( " OutputObjectSpecAutoCorr          = "+OutputObjectSpecAutoCorr )
PedestalAutoCorrLog.info( " OutputTagSpecAutoCorr             = "+OutputTagSpecAutoCorr )
PedestalAutoCorrLog.info( " IOVBegin                          = "+str(IOVBegin) )
PedestalAutoCorrLog.info( " IOVEnd                            = "+str(IOVEnd) )
PedestalAutoCorrLog.info( " LArCalibOutputDB                  = "+OutputDB )
PedestalAutoCorrLog.info( " ======================================================== " )

#######################################################################################
include ("LArConditionsCommon/LArMinimalSetup.py")
# new way to configure mapping:
# we need standard mapping for BadChannelAlg, there is no SC bad channel DB yet
from LArCabling.LArCablingAccess import LArCalibIdMapping,LArOnOffIdMapping
LArOnOffIdMapping()
LArCalibIdMapping()
if SuperCells:
   from LArCabling.LArCablingAccess import LArCalibIdMappingSC,LArOnOffIdMappingSC,LArLATOMEMappingSC
   LArOnOffIdMappingSC()
   LArCalibIdMappingSC()
   LArLATOMEMappingSC()

#
# Provides ByteStreamInputSvc name of the data file to process in the offline context
#

## get a handle to the default top-level algorithm sequence
from AthenaCommon.AlgSequence import AlgSequence 
topSequence = AlgSequence()
from AthenaCommon.AlgSequence import AthSequencer
condSeq = AthSequencer("AthCondSeq")


## get a handle to the ApplicationManager, to the ServiceManager and to the ToolSvc
from AthenaCommon.AppMgr import theApp, ServiceMgr, ToolSvc



## define the DB Gobal Tag :
ServiceMgr.IOVDbSvc.GlobalTag   = LArCalib_Flags.globalFlagDB
try:
   ServiceMgr.IOVDbSvc.DBInstance=""
except: 
   pass

theByteStreamInputSvc=ServiceMgr.ByteStreamInputSvc
if not 'FullFileName' in dir():
   PedestalAutoCorrLog.info( "No FullFileName! Please give a FullFileName list")
   theApp.exit(-1)
else :   
   ServiceMgr.EventSelector.Input=FullFileName
   
ServiceMgr.EventSelector.MaxBadEvents = 0

##############################################################################################
#                                                                                            #
# FullFileName is already the list of files compute by the fonction GetInputFilesFromTokens  #
#                                                                                            #
# maybe useful one day                                                                       #
#                                                                                            #
# else                                                                                        #
#   ServiceMgr.EventSelector.Input=OneFileName                                           #
#   for i in range(len(ServiceMgr.EventSelector.Input)):                                 #
#      theByteStreamInputSvc.NumFile+=[10000]                                                #
##############################################################################################


###############################################################
#                                                             #
#                To read BS for one or more FT                #
#                                                             #
###############################################################

## For splitting the delay runs, there are three new jobOptions.
## All three are vectors of integers
#################################################################

if runAccumulator:
 if SuperCells:
   from LArByteStream.LArByteStreamConf import LArLATOMEDecoder
   theLArLATOMEDecoder = LArLATOMEDecoder("LArLATOMEDecoder")
   theLArLATOMEDecoder.DumpFile = SC_DumpFile
   theLArLATOMEDecoder.RawDataFile = SC_RawDataFile

   from LArByteStream.LArByteStreamConf import LArRawSCDataReadingAlg
   larRawSCDataReadingAlg = LArRawSCDataReadingAlg() 
   larRawSCDataReadingAlg.adcCollKey = Gain
   larRawSCDataReadingAlg.adcBasCollKey = ""
   larRawSCDataReadingAlg.etCollKey = ""
   larRawSCDataReadingAlg.etIdCollKey = ""
   larRawSCDataReadingAlg.LATOMEDecoder = theLArLATOMEDecoder
   larRawSCDataReadingAlg.OutputLevel = INFO
   topSequence += larRawSCDataReadingAlg

 else:
   ServiceMgr.ByteStreamAddressProviderSvc.TypeNames += ["LArDigitContainer/HIGH"]
   ServiceMgr.ByteStreamAddressProviderSvc.TypeNames += ["LArDigitContainer/MEDIUM"]
   ServiceMgr.ByteStreamAddressProviderSvc.TypeNames += ["LArDigitContainer/LOW"]
   ServiceMgr.ByteStreamAddressProviderSvc.TypeNames += ["LArFebHeaderContainer/LArFebHeader"]

 from LArROD.LArRODConf import LArDigitsAccumulator
 larDigitsAccumulator = LArDigitsAccumulator("LArDigitsAccumulator")
 larDigitsAccumulator.KeyList = [Gain]
 larDigitsAccumulator.LArAccuDigitContainerName = ""
 larDigitsAccumulator.NTriggersPerStep = 100
 larDigitsAccumulator.OutputLevel = INFO

 topSequence += larDigitsAccumulator

else:
   
   # Need to debug the raw data reading algo for legacy
   from LArByteStream.LArByteStreamConf import LArRawCalibDataReadingAlg

   theLArRawCalibDataReadingAlg=LArRawCalibDataReadingAlg()
   theLArRawCalibDataReadingAlg.LArAccDigitKey=Gain
   theLArRawCalibDataReadingAlg.LArFebHeaderKey="LArFebHeader"

   topSequence+=theLArRawCalibDataReadingAlg


from IOVDbSvc.CondDB import conddb

if 'BadChannelsFolder' not in dir():
   BadChannelsFolder="/LAR/BadChannelsOfl/BadChannels"
if 'MissingFEBsFolder' not in dir():
   MissingFEBsFolder="/LAR/BadChannelsOfl/MissingFEBs"


if ( ReadBadChannelFromCOOL ):      
   if 'InputBadChannelSQLiteFile' in dir():
      InputDBConnectionBadChannel = DBConnectionFile(InputBadChannelSQLiteFile)
   else:
      #InputDBConnectionBadChannel = "oracle://ATLAS_COOLPROD;schema=ATLAS_COOLONL_LAR;dbname=CONDBR2;"
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


## This algorithm verifies that no FEBs are dropping out of the run
## If it finds corrupt events, it breaks the event loop and terminates the job rapidly
if not SuperCells:
   include ("LArROD/LArFebErrorSummaryMaker_jobOptions.py")
   topSequence.LArFebErrorSummaryMaker.CheckAllFEB=False

   if doBadCatcher:
      from LArCalibDataQuality.LArCalibDataQualityConf import LArBadEventCatcher
      theLArBadEventCatcher=LArBadEventCatcher()
      theLArBadEventCatcher.CheckAccCalibDigitCont=True
      theLArBadEventCatcher.CheckBSErrors=True
      theLArBadEventCatcher.KeyList=GainList
      theLArBadEventCatcher.StopOnError=False
      topSequence+=theLArBadEventCatcher 

#######################################################
#                                                     #
#      Pedestal run reconstruction                    #
#                                                     #
#######################################################



if not 'InputBadChannelSQLiteFile' in dir():
   PedestalAutoCorrLog.info( "Read Bad Channels from Oracle DB")
else :   
   PedestalAutoCorrLog.info( "Read Bad Channels from SQLite file") 

from LArCalibProcessing.LArCalibCatalogs import larCalibCatalogs
ServiceMgr.PoolSvc.ReadCatalog += larCalibCatalogs

## define the DB Gobal Tag :
ServiceMgr.IOVDbSvc.GlobalTag   = LArCalib_Flags.globalFlagDB
try:
   ServiceMgr.IOVDbSvc.DBInstance=""
except: 
   pass

# Temperature folder
#conddb.addFolder("DCS_OFL","/LAR/DCS/FEBTEMP")
#ServiceMgr.EventSelector.InitialTimeStamp = 1284030331
#import cx_Oracle
#import time
#import datetime
#try:   
#   connection=cx_Oracle.connect("ATLAS_SFO_T0_R/readmesfotz2008@atlr")
#   cursor=connection.cursor()
#   sRequest=("SELECT RUNNR,CREATION_TIME FROM SFO_TZ_RUN WHERE RUNNR='%s'")%(RunNumberList[0])
#   cursor.execute(sRequest)
#   times= cursor.fetchall()
#   d=times[0][1]
#   iovtemp=int(time.mktime(d.timetuple()))
#except:
#   iovtemp=1284030331

#printfunc ("Setting timestamp for run ",RunNumberList[0]," to ",iovtemp)
#ServiceMgr.IOVDbSvc.forceTimestamp = 1283145454
#ServiceMgr.IOVDbSvc.forceTimestamp = iovtemp

if ( doLArCalibDataQuality  ) :
   if  Pedestal :
      ## The reference is the Oracle DB
      conddb.addFolder("LAR_ONL","/LAR/ElecCalibOnl/Pedestal<key>PedestalRef</key>"+ChannelSelection)

   if AutoCorr :
      ## The reference is the Oracle DB
      conddb.addFolder("",LArCalib_Flags.LArAutoCorrFolder+"<key>LArAutoCorrRef</key><dbConnection>"+DBConnectionCOOL+"</dbConnection>"+ChannelSelection)


"""
if runAccumulator:
   if Pedestal :
      from LArCalibUtils.LArCalibUtilsConf import LArPedestalMaker
      LArPedestalMaker=LArPedestalMaker("LArPedestalMaker")
      LArPedestalMaker.events_ref   = EventsRef
      LArPedestalMaker.nsigma       = NSigma
      LArPedestalMaker.which_sample = WhichSample
      LArPedestalMaker.KeyList      = GainList
      LArPedestalMaker.KeyOutput    = KeyOutputPed
      LArPedestalMaker.GroupingType = GroupingType
      if not SuperCells:
         LArPedestalMaker.sample_min   = MinSample
         LArPedestalMaker.sample_max   = MaxSample

      topSequence += LArPedestalMaker

   if AutoCorr :
      from LArCalibUtils.LArCalibUtilsConf import LArAutoCorrMaker
      LArAutoCorrMaker=LArAutoCorrMaker("LArAutoCorrMaker")
      LArAutoCorrMaker.KeyList = GainList
      LArAutoCorrMaker.events_ref = EventsRef
      LArAutoCorrMaker.nsigma     = NSigma
      #LArAutoCorrMaker.Nsamples     = NSamples
      LArAutoCorrMaker.KeyOutput  = KeyOutputAC
      LArAutoCorrMaker.GroupingType = GroupingType
      topSequence += LArAutoCorrMaker

else :
"""
# pre computation in the DSP or acummulator
from LArCalibUtils.LArCalibUtilsConf import LArPedestalAutoCorrBuilder
LArPedACBuilder=LArPedestalAutoCorrBuilder("LArPedestalAutoCorrBuilder")
#LArPedACBuilder.isSC            = SuperCells
LArPedACBuilder.KeyList         = GainList
LArPedACBuilder.PedestalKey     = KeyOutputPed
LArPedACBuilder.AutoCorrKey     = KeyOutputAC      
LArPedACBuilder.GroupingType    = GroupingType
if not SuperCells:
   LArPedACBuilder.sample_min      = MinSample
   LArPedACBuilder.sample_max      = MaxSample

LArPedACBuilder.OutputLevel     = WARNING
topSequence += LArPedACBuilder

      
######################################################################
#                                                                    #
#                          Output                                    #
#                                                                    #
######################################################################


if ( doLArCalibDataQuality  ) :
   
   if  Pedestal :
      from LArCalibDataQuality.Thresholds import pedThr,rmsThr, pedThrFEB,rmsThrFEB
      from LArCalibDataQuality.LArCalibDataQualityConf import LArPedestalValidationAlg
      thePedestalValidationAlg=LArPedestalValidationAlg("PedestalVal")
      thePedestalValidationAlg.ValidationKey="Pedestal"
      thePedestalValidationAlg.ReferenceKey="PedestalRef"
      thePedestalValidationAlg.PedestalTolerance=pedThr
      thePedestalValidationAlg.PedestalRMSTolerance=rmsThr
      thePedestalValidationAlg.PedestalToleranceFEB=pedThrFEB
      thePedestalValidationAlg.PedestalRMSToleranceFEB=rmsThrFEB
      thePedestalValidationAlg.MsgLevelForDeviations=WARNING
      thePedestalValidationAlg.ListOfDevFEBs="pedFebs.txt"
      thePedestalValidationAlg.CheckCompletness=True
      thePedestalValidationAlg.PatchMissingFEBs=True
      thePedestalValidationAlg.CheckNumberOfCoolChannels=True
      thePedestalValidationAlg.UseCorrChannels=False #Corrections go into the regular data channels
      topSequence+=thePedestalValidationAlg

      ## second instance of the validation tool to detect "bad" channel
      theBadPedestal=LArPedestalValidationAlg("PedestalFail")
      theBadPedestal.ValidationKey="Pedestal"
      theBadPedestal.ReferenceKey="PedestalRef"
      theBadPedestal.PedestalTolerance       = ["10,10,10"]
      theBadPedestal.PedestalRMSTolerance    = ["0.5, 0.5, 0.5"]
      theBadPedestal.PedestalToleranceFEB    = ["8, 8, 8"]
      theBadPedestal.PedestalRMSToleranceFEB = ["1.2, 1.2, 1.2"]

      theBadPedestal.MsgLevelForDeviations=ERROR      
      theBadPedestal.CheckCompletness=False
      theBadPedestal.CheckNumberOfCoolChannels=False
      theBadPedestal.ListOfDevFEBs="Bad_pedFebs.txt"
      topSequence+=theBadPedestal

   if AutoCorr :
      from LArCalibDataQuality.Thresholds import acThr, acThrFEB
      from LArCalibDataQuality.LArCalibDataQualityConf import LArAutoCorrValidationAlg
      theAutoCorrValidationAlg=LArAutoCorrValidationAlg("AutoCorrVal")
      theAutoCorrValidationAlg.ValidationKey="LArAutoCorr"
      theAutoCorrValidationAlg.ReferenceKey="LArAutoCorrRef"
      theAutoCorrValidationAlg.AutoCorrTolerance=acThr
      theAutoCorrValidationAlg.AutoCorrToleranceFEB=acThrFEB
      theAutoCorrValidationAlg.MsgLevelForDeviations=WARNING
      theAutoCorrValidationAlg.ListOfDevFEBs="ACFebs.txt"
      theAutoCorrValidationAlg.CheckCompletness=True
      theAutoCorrValidationAlg.PatchMissingFEBs=True
      theAutoCorrValidationAlg.CheckNumberOfCoolChannels=True
      theAutoCorrValidationAlg.UseCorrChannels=False #Corrections go into the regular data channels
      topSequence+=theAutoCorrValidationAlg
      
      ## second instance of the validation tool to detect "bad" channel     
      theBadAutoCorr=LArAutoCorrValidationAlg("AutoCorrFail")
      theBadAutoCorr.ValidationKey="LArAutoCorr"
      theBadAutoCorr.ReferenceKey="LArAutoCorrRef"
      theBadAutoCorr.AutoCorrTolerance    = ["0.15, 0.15, 0.15"]
      theBadAutoCorr.AutoCorrToleranceFEB = ["0.15, 0.15, 0.15"]
      theBadAutoCorr.MsgLevelForDeviations=ERROR
      theBadAutoCorr.CheckFifthSample=True
      theBadAutoCorr.ListOfDevFEBs="Bad_ACFebs.txt"
      theBadAutoCorr.CheckCompletness=False
      theBadAutoCorr.CheckNumberOfCoolChannels=False
      
      topSequence+=theBadAutoCorr

      
      
if ( doMonitoring ) :

   from AthenaMonitoring.DQMonFlags import DQMonFlags
   DQMonFlags.enableLumiAccess.set_Value_and_Lock(False)

   from AthenaMonitoring.AthenaMonitoringConf import AthenaMonManager
   topSequence += AthenaMonManager( "LArMon" )
   LArMon = topSequence.LArMon

   ## tier0 style
   LArMon.FileKey = "GLOBAL"
   LArMon.ManualDataTypeSetup = True
   LArMon.Environment         = "tier0"
   LArMon.ManualRunLBSetup    = True
   LArMon.Run                 = int(RunNumberList[0])
   LArMon.LumiBlock           = 1
   LArMon.CheckEveryNoEvents  = 999999 #to do the check only at the end of the run
  
   LArDigitKey = Gain_forCoherentNoise


   if ( doLArCoherentNoise ) :
      from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
      athenaCommonFlags.isOnline=online
      from LArMonTools.LArMonFlags import LArMonFlags
      ProblemsToMask=[
         "deadReadout","deadPhys","short","almostDead",
         "highNoiseHG","highNoiseMG","highNoiseLG","sporadicBurstNoise"
         ]
      from LArRecUtils.LArRecUtilsConf import LArFlatConditionsAlg_LArPedestalFlat_ as LArPedestalCondAlg 
      svcMgr.IOVDbSvc.Folders.append("<db>COOLONL_LAR/CONDBR2</db>/LAR/ElecCalibFlat/Pedestal")
      from AthenaCommon.AlgSequence import AthSequencer
      condSeq = AthSequencer("AthCondSeq")
      condSeq+=LArPedestalCondAlg(ReadKey="/LAR/ElecCalibFlat/Pedestal",WriteKey="Pedestal2")
      condSeq.CondInputLoader.Load.append(("CondAttrListCollection","/LAR/ElecCalibFlat/Pedestal"))

      ## Coherent noise plots
      include("LArMonTools/LArNoiseCorrelationMon_jobOptions.py")    
      for item in topSequence.LArMon.AthenaMonTools:
          if item.getName()=='LArNoiseCorrelationMon':
            item.LArPedestalKey="Pedestal2"
            item.IsCalibrationRun=True
            item.LArDigitContainerKey=Gain_forCoherentNoise
            item.TriggerChain=""
            item.TrigDecisionTool=""
            item.FEBsToMonitor = coherentNoiseFEBS
            item.PublishPartialSums = coherNoisePartSums
      if 'EvtNo' in dir():  
         theApp.EvtMax = EvtNo
         pass
      pass


   if ( doLArFEBMon ) :
      ## LArFEBMon
      include("LArMonTools/LArFEBMon_jobOptions.py")
      
   if ( doLArOddCellMonTool ) :
      ## LArOddCellMonTool
      ReadCOOL = True ## Read pedestal from the DB
      include("LArMonTools/LArOddCellsMonTool_jobOptions.py")
      theLArOddCellsMonTool.LArPedestalKey = "PedestalRef"

   if ( doLArScaNoiseTool  ) :
      ## LArScaNoiseTool
      include("LArRawConditions/LArIdMap_ATLAS_jobOptions.py")
      include("LArMonTools/LArScaNoiseMonTool_jobOptions.py")

   RootHistOutputFileName='RootHistos_'+BaseFileNamePedAutoCorr+".root"

   theApp.HistogramPersistency = "ROOT"
   from GaudiSvc.GaudiSvcConf import THistSvc
   if os.path.exists(OutputPedAutoCorrRootFileDir + "/" +RootHistOutputFileName): 
      os.remove(OutputPedAutoCorrRootFileDir + "/" +RootHistOutputFileName)
   ServiceMgr += THistSvc()
   ServiceMgr.THistSvc.Output = ["GLOBAL DATAFILE='"+OutputPedAutoCorrRootFileDir + "/" +RootHistOutputFileName+"' OPT='RECREATE'"]



if ( WriteNtuple ) :
   
   if Pedestal :
      from LArCalibTools.LArCalibToolsConf import LArPedestals2Ntuple
      LArPedestals2Ntuple = LArPedestals2Ntuple("LArPedestals2Ntuple")
      LArPedestals2Ntuple.RealGeometry = True
      LArPedestals2Ntuple.OffId = True
      LArPedestals2Ntuple.ContainerKey = KeyOutputPed
      LArPedestals2Ntuple.AddFEBTempInfo = False
      LArPedestals2Ntuple.isSC = SuperCells
      if SuperCells:
         LArPedestals2Ntuple.CablingKey = "LArOnOffIdMapSC"
         LArPedestals2Ntuple.CalibMapKey = "LArCalibIdMapSC"

      topSequence += LArPedestals2Ntuple

   if AutoCorr :
      from LArCalibTools.LArCalibToolsConf import LArAutoCorr2Ntuple
      LArAutoCorr2Ntuple = LArAutoCorr2Ntuple( "LArAutoCorr2Ntuple" )
      LArAutoCorr2Ntuple.RealGeometry = True
      LArAutoCorr2Ntuple.OffId = True
      #LArAutoCorr2Ntuple.Nsamples     = NSamples
      LArAutoCorr2Ntuple.AddFEBTempInfo  = False
      LArAutoCorr2Ntuple.ContainerKey = KeyOutputAC
      LArAutoCorr2Ntuple.isSC = SuperCells
      if SuperCells:
         LArAutoCorr2Ntuple.CablingKey = "LArOnOffIdMapSC"
         LArAutoCorr2Ntuple.CalibMapKey = "LArCalibIdMapSC"
      topSequence += LArAutoCorr2Ntuple

   theApp.HistogramPersistency = "ROOT"
   from GaudiSvc.GaudiSvcConf import NTupleSvc
   if os.path.exists(OutputPedAutoCorrRootFileDir + "/" + OutputPedAutoCorrRootFileName): 
      os.remove(OutputPedAutoCorrRootFileDir + "/" + OutputPedAutoCorrRootFileName)
   ServiceMgr += NTupleSvc()
   ServiceMgr.NTupleSvc.Output = [ "FILE1 DATAFILE='"+OutputPedAutoCorrRootFileDir + "/" + OutputPedAutoCorrRootFileName+"' OPT='NEW'" ]


if ( WritePoolFile ) :

        from RegistrationServices.OutputConditionsAlg import OutputConditionsAlg
        
        if os.path.exists(OutputPedAutoCorrPoolFileDir + "/" + OutputPedAutoCorrPoolFileName): 
           os.remove(OutputPedAutoCorrPoolFileDir + "/" + OutputPedAutoCorrPoolFileName)
        OutputConditionsAlg=OutputConditionsAlg("OutputConditionsAlg",OutputPedAutoCorrPoolFileDir + "/" + OutputPedAutoCorrPoolFileName,
                                                [OutputObjectSpecPed],[OutputTagSpecPed],WriteIOV)
        OutputConditionsAlg.ObjectList +=  [OutputObjectSpecAutoCorr]
        OutputConditionsAlg.IOVTagList += [OutputTagSpecAutoCorr]

        OutputConditionsAlg.Run1 = IOVBegin
        if IOVEnd>0:
              OutputConditionsAlg.Run2=IOVEnd
        ServiceMgr.IOVDbSvc.dbConnection  = OutputDB
        
        from RegistrationServices.RegistrationServicesConf import IOVRegistrationSvc
        ServiceMgr += IOVRegistrationSvc()
        ServiceMgr.IOVRegistrationSvc.OutputLevel = WARNING
        ServiceMgr.IOVRegistrationSvc.RecreateFolders = False
       
        

###########################################################################

ServiceMgr.EventSelector.SkipEvents = SkipEvents

ServiceMgr.MessageSvc.OutputLevel  = INFO
ServiceMgr.MessageSvc.defaultLimit = 1000000000
ServiceMgr.MessageSvc.infoLimit = 1000000000
ServiceMgr.MessageSvc.Format       = "% F%20W%S%7W%R%T %0W%M"

ServiceMgr+=CfgMgr.AthenaEventLoopMgr(OutputLevel = INFO)

from AthenaCommon.AppMgr import theAuditorSvc
from AthenaCommon.ConfigurableDb import getConfigurable
theAuditorSvc += getConfigurable("MemStatAuditor")(OutputLevel = WARNING)
theApp.AuditAlgorithms=True
theAuditorSvc += getConfigurable("ChronoAuditor")(OutputLevel = WARNING)
theAuditorSvc += getConfigurable("NameAuditor")(OutputLevel = WARNING)
ServiceMgr.ChronoStatSvc.OutputLevel  = INFO

###########################################################################
#printfunc (condSeq)
