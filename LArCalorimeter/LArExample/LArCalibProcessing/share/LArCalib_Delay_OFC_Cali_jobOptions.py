# Modified version:
# - doOFC=False switch off the OFC computation and storing
# - skipEvents added to EventSelector - doesn't help for Xtalk
# - maxEvents added also - doesnt help for Xtalk
# - allCells will reconstruct also not-pulsed waves
# - usePatt - if <0 all patterns, if <0,15> or <0,13> if EMEC, fill only those pattern
# - numPatt - either 16 for EMB or 14 for EMEC (with standard configuration)

from future import standard_library
standard_library.install_aliases()
import subprocess

###########################################################################
#
# <Marco.Delmastro@cern.ch>
#
# Example jobOptions to reconstruction a DELAY runs in all 3 gains
#
# Last update: 09/12/2008 <Fabien.Tarrade@cern.ch>
#
###########################################################################

if not "SuperCells" in dir():
   SuperCells=False
   
if not SuperCells: include("LArCalibProcessing/LArCalib_Flags.py")
if SuperCells:     include("LArCalibProcessing/LArCalib_FlagsSC.py")
#include("RecExCommission/GetInputFiles.py")
include("LArCalibProcessing/GetInputFiles.py")

#######################################################
#       Run properties
#######################################################

if not 'SubDet' in dir():
   SubDet = "Barrel"

if not 'RunNumberList' in dir():
   RunNumberList = [ '0029143' ]
   
if not 'FilePrefix' in dir():
   if (int(RunNumberList[0]))<99800 :
      FilePrefix = "daq.Delay"
   else :
      FilePrefix = "data*"
     
if not 'InputDir' in dir():
   InputDir = "/castor/cern.ch/grid/atlas/DAQ/lar/ElecCalib/2008/"+RunNumberList[0]
   
if not 'Partition' in dir():
   if (SubDet=='EndCap'):
      Partition = "EB-EC*"
   else:
      Partition = "EB-EMB*"

if not 'doCaliWaveSelector' in dir():
   doCaliWaveSelector = False      

if not 'FullFileName' in dir():
   if not 'Trigger' in dir():
     if (int(RunNumberList[0]))<99800 :
       Trigger = "*"+Partition
     else :
       Trigger = "calibration_LArElec-Delay"+".*"+Partition
       #Trigger = "calibration_LArElec-Delay"+".*"
   
   FullFileName = []
   for RunNumber in RunNumberList :
       FullFileName+=GetInputFilesFromTokens(InputDir,int(RunNumber),FilePrefix,Trigger)

if not 'GainList' in dir():
   GainList = [ "HIGH", "MEDIUM", "LOW" ]

if not 'GroupingType' in dir():
   GroupingType = "ExtendedSubDetector"

if not 'ChannelSelection' in dir():
   # read all
   ChannelSelection = " "
   ## example to read only cool for Barrel C : Strips->Back
   #ChannelSelection = "<channelSelection>0,3:34</channelSelection>"

if not 'runAccumulator' in dir(): 
   runAccumulator = False # averaged mode

if not 'RecAllCells' in dir():
   RecAllCells=False # do not reco non-pulsed cells

if not 'doBadCatcher' in dir():
   doBadCatcher = True

from string import *
def DBConnectionFile(sqlitefile):  
   return "sqlite://;schema="+sqlitefile+";dbname=CONDBR2"

if not 'skipEvents' in dir():
   skipEvents=0

if not 'maxEvents' in dir():
   maxEvents=-1

if not 'allCells' in dir():
   allCells=False

if not 'usePatt' in dir():
   usePatt = -1

if not 'numPatt' in dir():
   if 'EMB' in Partition:
      numPatt = 16
   else:
      numPatt = 14

#######################################################
#                Monitoring properties
#######################################################

if not 'doMonitoring' in dir():
   doMonitoring = True

if not 'doLArCalibDataQuality' in dir():
   doLArCalibDataQuality = True

#######################################################
#       Delay properties
#######################################################

if not 'StripsXtalkCorr' in dir():
   StripsXtalkCorr = True # Xtalk correction for strips

if not 'ShortCorrector' in dir():
   ShortCorrector = True # correctio for short

if not "ADCsaturation" in dir():
   ADCsaturation  = 4095 # Set to 0 if you want to keep saturating pulses otherwise 4095
   
if not 'SubtractPed' in dir(): # Set whether to substract pedestals. Pedestals come from COOL, or local Pool file if 'PedPoolFileName' is defined, or fake pedestal subraction as 'extrema ratio'
   SubtractPed = True

if not 'CheckEmptyPhases' in dir():
   CheckEmptyPhases = True

if not 'SaveDerivedInfo' in dir():
   SaveDerivedInfo = True

if not 'SaveJitter' in dir():
   SaveJitter = True

if not 'CorrectBadChannels' in dir():
   CorrectBadChannels=True
  

###########################################################################
#                             OFC properties
###########################################################################
if not 'doOFC' in dir():
   doOFC = True

if doOFC:
 if not 'Nsamples' in dir():
   Nsamples = 5

 if not 'Nphases' in dir():
   Nphases = 50

 if not 'Dphases' in dir():
   Dphases = 1
   
 if not 'Ndelays' in dir():
   Ndelays = 24
	
 if not 'ContainerKey' in dir():
   ContainerKey = "LArCaliWave"
   
 if not 'OFCKey' in dir():
   OFCKey = "LArOFC"

 if not 'Normalize' in dir():
   Normalize = True

 if not 'TimeShift' in dir() :
   TimeShift = False

 if not 'TimeShiftByIndex' in dir() :
   TimeShiftByIndex = -1

 if not 'DumpOFC' in dir():
   DumpOFC = False

if not 'ForceShift' in dir():
   ForceShift = False
   
#######################################################
#      Delay output name
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

if not 'DBConnectionCOOLONL' in dir():  
   DBConnectionCOOLONL = "oracle://ATLAS_COOLPROD;schema=ATLAS_COOLONL_LAR;dbname=CONDBR2;"

## Pedestal
if not 'ReadPedFromCOOL' in dir():
   ReadPedFromCOOL = True
   
if not 'InputPedPoolFileDir' in dir():
   InputPedPoolFileDir  = subprocess.getoutput("pwd")
   
## AutoCorr
if not 'ReadAutoCorrFromCOOL' in dir():
   ReadAutoCorrFromCOOL = True 

if not 'InputAutoCorrPoolDir' in dir():
   InputAutoCorrPoolDir = subprocess.getoutput("pwd")

## Output
if 'dbname' in dir():
   rs=FolderTagResover(dbname=dbname)
else:
   rs=FolderTagResover()
if not 'PedLArCalibFolderTag' in dir():
   PedLArCalibFolderTag=rs.getFolderTagSuffix(LArCalib_Flags.LArPedestalFolder)

if not 'AutoCorrLArCalibFolderTag' in dir():
   AutoCorrLArCalibFolderTag=rs.getFolderTagSuffix(LArCalib_Flags.LArAutoCorrFolder)
   
if not 'LArCalibWaveFolderXtlkOutputTag' in dir():
   LArCalibWaveFolderXtlkOutputTag=rs.getFolderTagSuffix(LArCalib_Flags.LArCaliWaveFolderXtlk)
if not 'LArCalibWaveFolderOutputTag' in dir():
   LArCalibWaveFolderOutputTag=rs.getFolderTagSuffix(LArCalib_Flags.LArCaliWaveFolder)
if doOFC and not 'LArCalibOFCXtlkFolderOutputTag' in dir():
   LArCalibOFCXtlkFolderOutputTag=rs.getFolderTagSuffix(LArCalib_Flags.LArOFCCaliFolderXtlk)
if doOFC and not 'LArCalibOFCFolderOutputTag' in dir():
   LArCalibOFCFolderOutputTag=rs.getFolderTagSuffix(LArCalib_Flags.LArOFCCaliFolder)
del rs #Close database
   
if not 'OutputRootFileDir' in dir():
   OutputRootFileDir  = subprocess.getoutput("pwd")
   
if not 'OutputPoolFileDir' in dir():
   OutputPoolFileDir  = subprocess.getoutput("pwd")

if not 'OutputDB' in dir():
   OutputDB = LArCalib_Flags.OutputDB

if 'OutputSQLiteFile' in dir():
   OutputDB = DBConnectionFile(OutputSQLiteFile)      

if not 'KeyOutput' in dir():  
   KeyOutput = "LArCaliWave" # Key of LArCaliWaveContainer saved in Pool file
   
if not 'BaseFileName' in dir():
   BaseFileName = "LArCaliWave_OFC_Cali"
   BaseFileNameCaliWave = "LArCaliWave"
   
if StripsXtalkCorr :
   for RunNumber in RunNumberList :
      BaseFileName = BaseFileName+"_"+str(RunNumber)+"_"+Partition.replace("*","") +"_StripsXtalkCorr"
      BaseFileNameCaliWave = BaseFileNameCaliWave+"_"+str(RunNumber)+"_"+Partition.replace("*","") +"_StripsXtalkCorr"
else :
   for RunNumber in RunNumberList :
      BaseFileName = BaseFileName+"_"+str(RunNumber)+"_"+Partition.replace("*","")
      BaseFileNameCaliWave = BaseFileNameCaliWave+"_"+str(RunNumber)+"_"+Partition.replace("*","")

if not 'OutputCaliWaveRootFileName' in dir():
   OutputCaliWaveRootFileName = BaseFileNameCaliWave+".root"
   
if not 'OutputPoolFileName' in dir():
   OutputPoolFileName = BaseFileName+".pool.root"

if not 'OutputObjectSpecCaliWave' in dir():
   if StripsXtalkCorr:
      OutputObjectSpecCaliWave = "LArCaliWaveContainer#"+KeyOutput+"#"+LArCalib_Flags.LArCaliWaveFolderXtlk
      OutputTagSpecCaliWave = LArCalibFolderTag(LArCalib_Flags.LArCaliWaveFolderXtlk,LArCalibWaveFolderXtlkOutputTag)
   else:
      OutputObjectSpecCaliWave = "LArCaliWaveContainer#"+KeyOutput+"#"+LArCalib_Flags.LArCaliWaveFolder
      OutputTagSpecCaliWave = LArCalibFolderTag(LArCalib_Flags.LArCaliWaveFolder,LArCalibWaveFolderOutputTag)
   
if ( ReadPedFromCOOL ):      
   if 'InputPedSQLiteFile' in dir():
      InputDBConnectionPed = DBConnectionFile(InputPedSQLiteFile)
   else:
      InputDBConnectionPed = DBConnectionCOOLONL

if ( ReadAutoCorrFromCOOL ):      
   if 'InputAutoCorrSQLiteFile' in dir():
      InputDBConnectionAutoCorr = DBConnectionFile(InputAutoCorrSQLiteFile)
   else:
      InputDBConnectionAutoCorr = DBConnectionCOOL

if doOFC and not 'OutputObjectSpecOFC' in dir():
   if   ( ContainerKey == "LArCaliWave" ):
      if ( not StripsXtalkCorr ):
         OutputObjectSpecOFC   = "LArOFCComplete#"  +OFCKey  +"#"+ LArCalib_Flags.LArOFCCaliFolder	
         OutputObjectSpecTagOFC    = LArCalibFolderTag(LArCalib_Flags.LArOFCCaliFolder,LArCalibOFCFolderOutputTag)
      else:
         OutputObjectSpecOFC   = "LArOFCComplete#"  +OFCKey  +"#"+ LArCalib_Flags.LArOFCCaliFolderXtlk
         OutputObjectSpecTagOFC    = LArCalibFolderTag(LArCalib_Flags.LArOFCCaliFolderXtlk,LArCalibWaveFolderXtlkOutputTag)
			
   elif ( ContainerKey == "LArMasterWave" ):
      if ( not StripsXtalkCorr ):
         OutputObjectSpecOFC   = "LArOFCComplete#"+OFCKey+"#"+ LArCalib_Flags.LArOFCMasterWaveFolder
         OutputObjectSpecTagOFC    = LArCalibFolderTag(LArCalib_Flags.LArOFCMasterWaveFolder,LArCalibOFCFolderOutputTag)
      else:
         OutputObjectSpecOFC   = "LArOFCComplete#"+OFCKey+"#"+ LArCalib_Flags.LArOFCMasterWaveFolderXtlk
         OutputObjectSpecTagOFC    = LArCalibFolderTag(LArCalib_Flags.LArOFCMasterWaveFolderXtlk,LArCalibOFCXtlkFolderOutputTag)

if doOFC:
  OFCFileTag = str(RunNumber)+"_"+Partition.replace("*","")

  if (StripsXtalkCorr):
    OFCFileTag += "_StripsXtalkCorr"

  OFCFileTag += "_"+str(Nsamples)+"samples"

  if (Dphases>1):
    OFCFileTag += "_"+str(Dphases)+"Dphase"

  if not 'OutputOFCRootFileName' in dir():
    OutputOFCRootFileName = "LArOFCCali_"+OFCFileTag + ".root"
   
## Bad Channel   
   
if not 'ReadBadChannelFromCOOL' in dir():
   ReadBadChannelFromCOOL = True   

if ( ReadBadChannelFromCOOL ):      
   if 'InputBadChannelSQLiteFile' in dir():
      InputDBConnectionBadChannel = DBConnectionFile(InputBadChannelSQLiteFile)
   else:
      #InputDBConnectionBadChannel = "oracle://ATLAS_COOLPROD;schema=ATLAS_COOLONL_LAR;dbname=CONDBR2;"
      if 'InputDBConnectionBadChannel' not in dir():
         InputDBConnectionBadChannel = "COOLOFL_LAR/CONDBR2"      
      
###########################################################################
#                             OFC properties
###########################################################################
if doOFC:
 if not 'Nsamples' in dir():
   Nsamples = 5

 if not 'Nphases' in dir():
   Nphases = 50

 if not 'Dphases' in dir():
   Dphases = 1
   
 if not 'Ndelays' in dir():
   Ndelays = 24
	
 if not 'ContainerKey' in dir():
   ContainerKey = "LArCaliWave"
   
 if not 'OFCKey' in dir():
   OFCKey = "LArOFC"

 if not 'Normalize' in dir():
   Normalize = True

 if not 'TimeShift' in dir() :
   TimeShift = False

 if not 'TimeShiftByIndex' in dir() :
   TimeShiftByIndex = -1

 if not 'DumpOFC' in dir():
   DumpOFC = False

if not 'doOFC' in dir():
   doOFC=True
#######################################################################################
# print summary
#######################################################################################
DelayOFCLog = logging.getLogger( "DelayOFCLog" )
DelayOFCLog.info( " ======================================================== " )
DelayOFCLog.info( " ***                 LAr Delay&&OFC summary           *** " )
DelayOFCLog.info( " ======================================================== " )
DelayOFCLog.info( " RunNumber                          = "+str(RunNumberList) )
DelayOFCLog.info( " SubDetector                        = "+SubDet )
DelayOFCLog.info( " Partition                          = "+Partition )
DelayOFCLog.info( " Type                               = Delay "  )
DelayOFCLog.info( " LArGain                            = "+str(GainList) )
for i in range(len(FullFileName)):
  DelayOFCLog.info( " FullFileName                       = "+FullFileName[i] )
if ( ReadPedFromCOOL ):
  DelayOFCLog.info( " InputDBConnectionPed               = "+InputDBConnectionPed)
else :
  DelayOFCLog.info( " InputPedPoolFileName               = "+InputPedPoolFileName)
if ( ReadAutoCorrFromCOOL ):
  DelayOFCLog.info( " InputDBConnectionAutoCorr          = "+InputDBConnectionAutoCorr )
else:   
  DelayOFCLog.info( " InputAutoCorrPoolFileName          = "+InputAutoCorrPoolFileName )
DelayOFCLog.info( " PedLArCalibFolderTag               = "+PedLArCalibFolderTag )
DelayOFCLog.info( " OutputCaliWaveRootFullFileName     = "+OutputRootFileDir+"/"+OutputCaliWaveRootFileName )
DelayOFCLog.info( " OutputObjectSpecCaliWave           = "+OutputObjectSpecCaliWave )
DelayOFCLog.info( " OutputTagSpecCaliWave              = "+OutputTagSpecCaliWave )
if doOFC:   
 DelayOFCLog.info( " OutputOFCRootFullFileName          = "+OutputRootFileDir+"/"+OutputOFCRootFileName )
 DelayOFCLog.info( " OutputPoolFullFileName             = "+OutputPoolFileDir+"/"+OutputPoolFileName )
 DelayOFCLog.info( " OutputObjectSpecOFC                = "+str(OutputObjectSpecOFC) )
 DelayOFCLog.info( " OutputObjectSpecTagOFC             = "+str(OutputObjectSpecTagOFC) )

DelayOFCLog.info( " IOVBegin                           = "+str(IOVBegin) )
DelayOFCLog.info( " IOVEnd                             = "+str(IOVEnd) )
DelayOFCLog.info( " LArCalibOutputDB                   = "+OutputDB )
DelayOFCLog.info( " ======================================================== " )
#######################################################################################


include ("LArConditionsCommon/LArMinimalSetup.py")
from LArCabling.LArCablingAccess import LArOnOffIdMapping
LArOnOffIdMapping()
if SuperCells:
  from LArCabling.LArCablingAccess import LArOnOffIdMappingSC,LArCalibIdMappingSC
  LArOnOffIdMappingSC()
  LArCalibIdMappingSC()
#from LArBadChannelTool.LArBadChannelAccess import LArBadChannelAccess
#LArBadChannelAccess()

#
# Provides ByteStreamInputSvc name of the data file to process in the offline context
#

## get a handle to the default top-level algorithm sequence
from AthenaCommon.AlgSequence import AlgSequence 
topSequence = AlgSequence()
from AthenaCommon.AlgSequence import AthSequencer
condSeq = AthSequencer("AthCondSeq")

## get a handle to the ApplicationManager, to the ServiceManager and to the ToolSvc
from AthenaCommon.AppMgr import (theApp, ServiceMgr as svcMgr,ToolSvc)

theByteStreamInputSvc=svcMgr.ByteStreamInputSvc
if not 'FullFileName' in dir():
   DelayOFCLog.info( "No FullFileName! Please give a FullFileName list.")
   theApp.exit(-1)
else :   
   #svcMgr.EventSelector.Input=FullFileName
   svcMgr.EventSelector.Input = FullFileName
   
scvMgr.EventSelector.MaxBadEvents = 0

##############################################################################################
#                                                                                            #
# FullFileName is already the list of files compute by the fonction GetInputFilesFromTokens  #
#                                                                                            #
# maybe useful one day                                                                       #
#                                                                                            #
# else                                                                                        #
#   svcMgr.EventSelector.Input=OneFileName                                           #
#   for i in range(len(svcMgr.EventSelector.Input)):                                 #
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

from LArByteStream.LArByteStreamConf import LArRodDecoder
svcMgr.ToolSvc += LArRodDecoder()

#ToolSvc.LArRodDecoder.BEPreselection     = [0]                                                   ## : [Barrel=0,Endcap=1]
#ToolSvc.LArRodDecoder.PosNegPreselection = [1]                                                   ## : [C-side (negative eta)=0, A-side (positive eta)=1]
#ToolSvc.LArRodDecoder.FTNumPreselection  = [0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15]               ## : first half of [EM barrel feedthrough numbers]
#ToolSvc.LArRodDecoder.FTNumPreselection  = [16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31]     ## : second half of [EM barrel feedthrough numbers]
#ToolSvc.LArRodDecoder.FTNumPreselection  = [0,1,4,7,8,11,12,13,14,17,18,19,20,23,24]             ## : [EMEC Standard feedthrough numbers]
#ToolSvc.LArRodDecoder.FTNumPreselection  = [2,9,15,21]                                           ## : [EMEC Special feedthrough numbers]
#ToolSvc.LArRodDecoder.FTNumPreselection  = [3,10,16,22]                                          ## : [HEC feedthrough numbers]  (note: slots 1&2 are EMEC slots)
#ToolSvc.LArRodDecoder.FTNumPreselection  = [6]                                                   ## : [FCAL feedthrough number]
   

###########################################################################
#                                                                         #
#                          Accumulator                                    #
#                                                                         #
###########################################################################

if ( runAccumulator ) :
   # this is a OLD jobOptions which can maybe work but only for the barrel                        #
   # can be used as a skeleton if needed but                                                      #
   # need to be updated for the barrel and the patterns for EMEC, HEC and FCAL need to be added   #
   ByteStreamAddressProviderSvc =svcMgr.ByteStreamAddressProviderSvc
   #if SuperCells is False:
   #   theByteStreamAddressProviderSvc.TypeNames += ["LArFebHeaderContainer/LArFebHeader"]
   
   include("./LArCalib_CalibrationPatterns.py")

else:
   theByteStreamAddressProviderSvc =svcMgr.ByteStreamAddressProviderSvc
   theByteStreamAddressProviderSvc.TypeNames += ["LArFebHeaderContainer/LArFebHeader"]
#   theByteStreamAddressProviderSvc.TypeNames += [ "LArAccumulatedCalibDigitContainer/FREE"  ]
   theByteStreamAddressProviderSvc.TypeNames += [ "LArAccumulatedCalibDigitContainer/HIGH"  ]
   theByteStreamAddressProviderSvc.TypeNames += [ "LArAccumulatedCalibDigitContainer/MEDIUM"]
   theByteStreamAddressProviderSvc.TypeNames += [ "LArAccumulatedCalibDigitContainer/LOW"   ]

## This algorithm verifies that no FEBs are dropping out of the run
## If it finds corrupt events, it breaks the event loop and terminates the job rapidly
if not SuperCells:
   include ("LArROD/LArFebErrorSummaryMaker_jobOptions.py")   
   topSequence.LArFebErrorSummaryMaker.CheckAllFEB=False
if CheckBadEvents:
   from LArCalibDataQuality.LArCalibDataQualityConf import LArBadEventCatcher
   theLArBadEventCatcher=LArBadEventCatcher()
   theLArBadEventCatcher.CheckAccCalibDigitCont=True
   theLArBadEventCatcher.CheckBSErrors=True
   theLArBadEventCatcher.KeyList=GainList
   theLArBadEventCatcher.StopOnError=False
   topSequence+=theLArBadEventCatcher      
      
svcMgr.EventSelector.SkipEvents=skipEvents
theApp.EvtMax = maxEvents

##########################################################################
#                                                                        #
#                      Delay run reconstruction                          #
#                                                                        #
##########################################################################

include("AthenaPoolCnvSvc/AthenaPool_jobOptions.py")
include("LArCondAthenaPool/LArCondAthenaPool_joboptions.py")

from IOVDbSvc.CondDB import conddb
PoolFileList     = []

#include ("LArCalibProcessing/LArCalib_BadChanTool.py")
if 'BadChannelsFolder' not in dir():
   BadChannelsFolder="/LAR/BadChannels/BadChannels"
if 'MissingFEBsFolder' not in dir():
   MissingFEBsFolder="/LAR/BadChannels/MissingFEBs"

if not 'InputBadChannelSQLiteFile' in dir():
   DelayOFCLog.info( "Read Bad Channels from Oracle DB")
else :   
   DelayOFCLog. info( "Read Bad Channels from SQLite file") 

if 'BadChannelsFolder' in dir():
 if 'BadChannelsLArCalibFolderTag' in dir() :
   BadChannelsTagSpec = LArCalibFolderTag (BadChannelsFolder,BadChannelsLArCalibFolderTag) 
   conddb.addFolder("",BadChannelsFolder+"<tag>"+BadChannelsTagSpec+"</tag>"+"<dbConnection>"+InputDBConnectionBadChannel+"</dbConnection>",className="CondAttrListCollection")
 else :
   conddb.addFolder("",BadChannelsFolder+"<dbConnection>"+InputDBConnectionBadChannel+"</dbConnection>",className="CondAttrListCollection")

if 'MissingFEBsFolder' in dir():
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


## define the DB Gobal Tag :
svcMgr.IOVDbSvc.GlobalTag   = LArCalib_Flags.globalFlagDB   
try:
   svcMgr.IOVDbSvc.DBInstance=""
except: 
   pass

# Temperature folder
#conddb.addFolder("DCS_OFL","/LAR/DCS/FEBTEMP")
#svcMgr.EventSelector.InitialTimeStamp = 1284030331
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
#   iovtemp=1283145454

#printfunc ("Setting timestamp for run ",RunNumberList[0]," to ",iovtemp)
#svcMgr.IOVDbSvc.forceTimestamp = 1283145454
#svcMgr.IOVDbSvc.forceTimestamp = iovtemp

from LArCalibProcessing.LArCalibCatalogs import larCalibCatalogs
svcMgr.PoolSvc.ReadCatalog += larCalibCatalogs

if ( doLArCalibDataQuality  ) :
   ## The reference is the Oracle DB
   if StripsXtalkCorr:
      conddb.addFolder("",LArCalib_Flags.LArCaliWaveFolderXtlk+"<key>LArCaliWaveRef</key><dbConnection>"+DBConnectionCOOL+"</dbConnection>"+ChannelSelection)
   else:
      conddb.addFolder("",LArCalib_Flags.LArCaliWaveFolder+"<key>LArCaliWaveRef</key><dbConnection>"+DBConnectionCOOL+"</dbConnection>"+ChannelSelection)

if (SubtractPed):
   if ( ReadPedFromCOOL ):
      if not 'InputPedSQLiteFile' in dir():
         DelayOFCLog.info( "Read Pedestal from Oracle DB")
         PedestalFolder = "/LAR/ElecCalibFlat/Pedestal"
         PedestalTagSpec = ""
         from LArRecUtils.LArRecUtilsConf import LArFlatConditionSvc
         theLArCondSvc=LArFlatConditionSvc(DoSuperCells=False,DoRegularCells=True)
         svcMgr+=theLArCondSvc
         svcMgr.ProxyProviderSvc.ProviderNames += [ "LArFlatConditionSvc" ]
         svcMgr.LArFlatConditionSvc.PedestalInput=PedestalFolder
         PedChannelSelection=""
      else :   
         DelayOFCLog.info( "Read Pedestal from SQLite file")
         PedestalFolder = LArCalib_Flags.LArPedestalFolder
         PedestalTagSpec = LArCalibFolderTag (PedestalFolder,PedLArCalibFolderTag)
         PedChannelSelection=ChannelSelection
         
      conddb.addFolder("",PedestalFolder+"<tag>"+PedestalTagSpec+"</tag>"+"<dbConnection>"+InputDBConnectionPed+"</dbConnection>"+PedChannelSelection)
   else:
      if 'InputPedPoolFileName' in dir():
         DelayOFCLog.info( "Read Pedestal from POOL file") 
         PoolFileList += [ InputPedPoolFileDir+"/"+InputPedPoolFileName ]

      else:
         DelayOFCLog.info( "LArCaliWaveBuilder WARNING : Using fake pedestals subtraction for all channels.")
   
if ( len(PoolFileList)>0 ):
   
   from AthenaCommon.ConfigurableDb import getConfigurable
   svcMgr += getConfigurable( "ProxyProviderSvc" )()
   svcMgr.ProxyProviderSvc.ProviderNames += [ "CondProxyProvider" ]
   
   svcMgr += getConfigurable( "CondProxyProvider" )()
   svcMgr.CondProxyProvider.InputCollections += PoolFileList

if ( ShortCorrector ):
   
   from LArCalibUtils.LArCalibUtilsConf import LArCalibShortCorrector
   theLArCalibShortCorrector = LArCalibShortCorrector()
   theLArCalibShortCorrector.KeyList = GainList
   theLArCalibShortCorrector.OutputLevel=INFO
   topSequence +=theLArCalibShortCorrector
   
if ( StripsXtalkCorr ) :

   from LArBadChannelTool.LArBadChannelToolConf import LArBadChannelMasker
   from LArCalibUtils.LArCalibUtilsConf import LArStripsCrossTalkCorrector
   theLArStripsCrossTalkCorrector = LArStripsCrossTalkCorrector()
   theLArStripsCrossTalkCorrector.KeyList = GainList
   theLArStripsCrossTalkCorrector.ADCsaturation = ADCsaturation
   theLArStripsCrossTalkCorrector.NoXtalkCorr=LArBadChannelMasker("NoXtalkCorr",
                                                               DoMasking=True,
                                                               ProblemsToMask=["deadReadout","deadPhys","deadCalib","almostDead"]
                                                               )
   theLArStripsCrossTalkCorrector.DontUseForXtalkCorr=LArBadChannelMasker("DontUseForXtalkCorr",
                                                                       DoMasking=True,
                                                                       ProblemsToMask=["short","peculiarCalibrationLine", "deadReadout", "deadPhys"]
                                                                       )                           
   theLArStripsCrossTalkCorrector.AcceptableDifference=25.0 #in per-cent                                                                       
   topSequence +=theLArStripsCrossTalkCorrector
 
from LArCalibUtils.LArCalibUtilsConf import LArCaliWaveBuilder
LArCaliWaveBuilder = LArCaliWaveBuilder()
LArCaliWaveBuilder.KeyList          = GainList
if (doCaliWaveSelector) :
   LArCaliWaveBuilder.KeyOutput        = KeyOutput+"multi"
else :   
   LArCaliWaveBuilder.KeyOutput        = KeyOutput
LArCaliWaveBuilder.GroupingType     = GroupingType
LArCaliWaveBuilder.SubtractPed      = SubtractPed
LArCaliWaveBuilder.CheckEmptyPhases = CheckEmptyPhases
LArCaliWaveBuilder.NBaseline        = 0 # to avoid the use of the baseline when Pedestal are missing
LArCaliWaveBuilder.UseDacAndIsPulsedIndex = False # should have an impact only for HEC
LArCaliWaveBuilder.RecAllCells      = RecAllCells
LArCaliWaveBuilder.isSC       = SuperCells
LArCaliWaveBuilder.UsePattern = usePatt
LArCaliWaveBuilder.NumPattern = numPatt

if StripsXtalkCorr:
   LArCaliWaveBuilder.ADCsaturation = 0
else:
   LArCaliWaveBuilder.ADCsaturation = ADCsaturation

if SuperCells:
   LArCaliWaveBuilder.CablingKey="LArOnOffIdMapSC"
   
topSequence+=LArCaliWaveBuilder

if (doCaliWaveSelector) :
   from LArCalibUtils.LArCalibUtilsConf import LArCaliWaveSelector
   LArCaliWaveSelector = LArCaliWaveSelector("LArCaliWaveSelector")
   LArCaliWaveSelector.KeyList         = [ KeyOutput+"multi" ]
   LArCaliWaveSelector.KeyOutput       = KeyOutput
   LArCaliWaveSelector.GroupingType     = GroupingType
   if (GainList[0]=="HIGH") :
      LArCaliWaveSelector.SelectionList = [ "HEC/0/0/460","HEC/1/0/460","HEC/2/0/230","HEC/3/0/230" ] 

   if (GainList[0]=="MEDIUM") :
      LArCaliWaveSelector.SelectionList = [ "HEC/0/1/3600","HEC/1/1/3600","HEC/2/1/1800","HEC/3/1/1800"]
   
   if (GainList[0]=="LOW") :   
      LArCaliWaveSelector.SelectionList = [ "HEC/0/2/24000","HEC/1/2/24000","HEC/2/2/18000","HEC/3/2/18000" ]
      
   topSequence+=LArCaliWaveSelector

######################################################################
#                                                                    #
#                          Correction                                #
#                                                                    #
######################################################################

if CorrectBadChannels:

   from LArCalibUtils.LArCalibUtilsConf import LArCalibPatchingAlg_LArCaliWaveContainer_
   theLArCaliWavePatcher=LArCalibPatchingAlg_LArCaliWaveContainer_("LArCaliWavePatcher")
   theLArCaliWavePatcher.ContainerKey=KeyOutput
   #theLArCaliWavePatcher.PatchMethod="PhiNeighbor" ##take the first neigbour
   theLArCaliWavePatcher.PatchMethod="PhiAverage" ##do an aveage in phi after removing bad and empty event
   theLArCaliWavePatcher.OutputLevel=INFO
   
   from LArBadChannelTool.LArBadChannelToolConf import LArBadChannelMasker
   theLArRCBMasker=LArBadChannelMasker("LArRCBMasker")
   theLArRCBMasker.DoMasking=True
   theLArRCBMasker.ProblemsToMask=[
      "deadCalib","deadReadout","deadPhys","almostDead","short",
      ]
   theLArCaliWavePatcher.UseCorrChannels=False  
   ToolSvc+=theLArRCBMasker
   theLArCaliWavePatcher.MaskingTool=theLArRCBMasker
   topSequence+=theLArCaliWavePatcher
 

##########################################################################
#                                                                        #
#                                 Cali OFC                               #
#                                                                        #
##########################################################################
if doOFC:
 if ( ReadAutoCorrFromCOOL ):
   if not 'InputAutoCorrSQLiteFile' in dir():
      DelayOFCLog.info( "Read AutoCorr from  Oracle DB" )
   else :
      DelayOFCLog.info( "Read AutoCorr from SQLite file" )
      
   AutoCorrFolder  = LArCalib_Flags.LArAutoCorrFolder
   AutoCorrTagSpec = LArCalibFolderTag(AutoCorrFolder,AutoCorrLArCalibFolderTag)
   conddb.addFolder("",AutoCorrFolder+"<tag>"+AutoCorrTagSpec+"</tag>"+"<dbConnection>"+InputDBConnectionAutoCorr+"</dbConnection>"+ ChannelSelection)
 else:
   if 'InputAutoCorrPoolFileName' in dir():
      DelayOFCLog.info( "Read AutoCorr from POOL file" )
      PoolFileList += [ InputAutoCorrPoolDir+"/"+InputAutoCorrPoolFileName ]
   else:
      DelayOFCLog.info( "No PoolFileList found! Please list the POOL files containing AutoCorrelation Matrix or read from COOL." )
      theApp.exit(-1)      


##########################################################################
#                                                                        #
#                                 Output                                 #
#                                                                        #
##########################################################################

#if not SuperCells:
#   from xAODEventInfoCnv.xAODEventInfoCreator import xAODMaker__EventInfoCnvAlg
#   topSequence+=xAODMaker__EventInfoCnvAlg()

if ( doLArCalibDataQuality  ) :
   from LArCalibDataQuality.LArCalibDataQualityConf import LArCaliWaveValidationAlg
   from LArBadChannelTool.LArBadChannelToolConf import LArBadChannelMasker
   theLArDelayValBCMask=LArBadChannelMasker("DelayValBCMask",
                                            DoMasking=True,
                                            ProblemsToMask=["deadReadout","deadCalib","deadPhys","almostDead",
                                                            "highNoiseHG","highNoiseMG","highNoiseLG"]
                                            )
   svcMgr.ToolSvc+=theLArDelayValBCMask
   from LArCalibDataQuality.Thresholds import cwFWHMThr, cwAmpThr,cwAmpThrFEB, cwFWHMThrFEB  
   theCaliWaveValidationAlg=LArCaliWaveValidationAlg("CaliWaveVal")
   theCaliWaveValidationAlg.BadChannelMaskingTool=theLArDelayValBCMask
   theCaliWaveValidationAlg.ValidationKey=KeyOutput
   theCaliWaveValidationAlg.ReferenceKey="LArCaliWaveRef"
   theCaliWaveValidationAlg.MsgLevelForDeviations=WARNING
   theCaliWaveValidationAlg.ListOfDevFEBs="caliWaveFebs.txt"
   theCaliWaveValidationAlg.AmplitudeTolerance=cwAmpThr
   theCaliWaveValidationAlg.CaliWaveFWHMTolerance=cwFWHMThr
   theCaliWaveValidationAlg.AmplitudeToleranceFEB=cwAmpThrFEB
   theCaliWaveValidationAlg.CaliWaveFWHMToleranceFEB=cwFWHMThrFEB
   theCaliWaveValidationAlg.TimeShiftDetection=True
   theCaliWaveValidationAlg.PatchMissingFEBs=True
   theCaliWaveValidationAlg.UseCorrChannels=False  
   topSequence+=theCaliWaveValidationAlg
   
   ## second instance of the validation tool to detect "bad" channel 
   theBadCaliWave=LArCaliWaveValidationAlg("CaliWaveFail")
   theBadCaliWave.BadChannelMaskingTool=theLArDelayValBCMask
   theBadCaliWave.ValidationKey=KeyOutput
   theBadCaliWave.ReferenceKey="LArCaliWaveRef"
   theBadCaliWave.MsgLevelForDeviations=WARNING
   theBadCaliWave.ListOfDevFEBs="Bad_caliWaveFebs.txt"
   theBadCaliWave.AmplitudeTolerance=["20,20,20"]
   theBadCaliWave.CaliWaveFWHMTolerance=["50,50,50"]
   theBadCaliWave.AmplitudeToleranceFEB=["10,10,10"]
   theBadCaliWave.CaliWaveFWHMToleranceFEB=["80,80,80"]
   theBadCaliWave.TimeShiftDetection=True
   theBadCaliWave.UseCorrChannels=False
   topSequence+=theBadCaliWave
   


if ( doMonitoring ) :

   from AthenaMonitoring.DQMonFlags import DQMonFlags
   DQMonFlags.enableLumiAccess.set_Value_and_Lock(False)

   from AthenaMonitoring.AthenaMonitoringConf import AthenaMonManager
   topSequence += AthenaMonManager( "LArMon" )
   LArMon = topSequence.LArMon

   ## old style
   #LArMon.FileKey = "AllMon"
   #LArMon.ManualDataTypeSetup = True
   #LArMon.Environment         = "user"
   #LArMon.ManualRunLBSetup    = True
   #LArMon.Run                 = 1
   #LArMon.LumiBlock           = 1
   #LArMon.CheckEveryNoEvents  = 999999 #to do the check only at the end of the run

   ## tier0 style
   LArMon.FileKey = "GLOBAL"
   LArMon.ManualDataTypeSetup = True
   LArMon.Environment         = "tier0"
   LArMon.ManualRunLBSetup    = True
   LArMon.Run                 = int(RunNumberList[0])
   LArMon.LumiBlock           = 1
   LArMon.CheckEveryNoEvents  = 999999 #to do the check only at the end of the run
   
   include("LArMonTools/LArFEBMon_jobOptions.py")
   
   RootHistOutputFileName='RootHistos_'+BaseFileName+".root"

   theApp.HistogramPersistency = "ROOT"
   from GaudiSvc.GaudiSvcConf import THistSvc
   if os.path.exists(OutputRootFileDir+ "/" +RootHistOutputFileName): 
      os.remove(OutputRootFileDir + "/" +RootHistOutputFileName)
   svcMgr += THistSvc()

   svcMgr.THistSvc.Output =  ["GLOBAL DATAFILE='"+OutputRootFileDir+ "/" +RootHistOutputFileName+"' OPT='New'"]


if (WriteNtuple):
   from LArCalibTools.LArCalibToolsConf import LArCaliWaves2Ntuple
   LArCaliWaves2Ntuple = LArCaliWaves2Ntuple( "LArCaliWaves2Ntuple" )
   LArCaliWaves2Ntuple.NtupleName  = "CALIWAVE"
   LArCaliWaves2Ntuple.SaveDerivedInfo = SaveDerivedInfo
   LArCaliWaves2Ntuple.AddFEBTempInfo = False
   LArCaliWaves2Ntuple.SaveJitter = SaveJitter
   LArCaliWaves2Ntuple.KeyList     = [ KeyOutput ]
   LArCaliWaves2Ntuple.isSC = SuperCells
   if SuperCells:
      LArCaliWaves2Ntuple.CalibMapKey = "LArCalibIdMapSC"
   
   topSequence+=LArCaliWaves2Ntuple
   
   theApp.HistogramPersistency = "ROOT"
   from GaudiSvc.GaudiSvcConf import NTupleSvc
   if os.path.exists(OutputRootFileDir+"/"+OutputCaliWaveRootFileName): 
      os.remove(OutputRootFileDir+"/"+OutputCaliWaveRootFileName)  
   svcMgr += NTupleSvc()
   svcMgr.NTupleSvc.Output = [ "FILE1 DATAFILE='"+OutputRootFileDir+"/"+OutputCaliWaveRootFileName+"' OPT='NEW'" ]


if ( WritePoolFile ) :
   
   from RegistrationServices.OutputConditionsAlg import OutputConditionsAlg

   if os.path.exists(OutputPoolFileDir+"/"+OutputPoolFileName): 
      os.remove(OutputPoolFileDir+"/"+OutputPoolFileName)
   OutputConditionsAlg=OutputConditionsAlg("OutputConditionsAlg",OutputPoolFileDir+"/"+OutputPoolFileName,
                                           [OutputObjectSpecCaliWave],[OutputTagSpecCaliWave],WriteIOV)
   OutputConditionsAlg.Run1 = IOVBegin
   if IOVEnd>0:
      OutputConditionsAlg.Run2 = IOVEnd
   
   #OutputConditionsAlg.ObjectList += [OutputObjectSpecOFC]
   #OutputConditionsAlg.IOVTagList += [OutputObjectSpecTagOFC]


   svcMgr.IOVDbSvc.dbConnection  = OutputDB
   
   from RegistrationServices.RegistrationServicesConf import IOVRegistrationSvc
   svcMgr += IOVRegistrationSvc()
   svcMgr.IOVRegistrationSvc.RecreateFolders = False




###########################################################################
#                            OFC computation
###########################################################################
if doOFC:
  from LArCalibUtils.LArCalibUtilsConf import LArAutoCorrDecoderTool
  theLArAutoCorrDecoderTool = LArAutoCorrDecoderTool()
  theLArAutoCorrDecoderTool.isSC = SuperCells
  ToolSvc += theLArAutoCorrDecoderTool

  from LArCalibUtils.LArCalibUtilsConf import LArOFCAlg
  LArCaliOFCAlg = LArOFCAlg("LArCaliOFCAlg")
  LArCaliOFCAlg.ReadCaliWave = True
  LArCaliOFCAlg.KeyList   = [ ContainerKey ]
  LArCaliOFCAlg.Nphase    = Nphases
  LArCaliOFCAlg.Dphase    = Dphases
  LArCaliOFCAlg.Ndelay    = Ndelays
  LArCaliOFCAlg.Nsample   = Nsamples
  LArCaliOFCAlg.Normalize = Normalize
  LArCaliOFCAlg.TimeShift = TimeShift
  LArCaliOFCAlg.TimeShiftByIndex = TimeShiftByIndex
  LArCaliOFCAlg.Verify    = True
  LArCaliOFCAlg.FillShape = False
  if ( DumpOFC ) :
     LArCaliOFCAlg.DumpOFCfile = "LArOFCCali.dat"
  LArCaliOFCAlg.GroupingType = GroupingType
  LArCaliOFCAlg.DecoderTool=theLArAutoCorrDecoderTool
  LArCaliOFCAlg.isSC = SuperCells
  topSequence+=LArCaliOFCAlg



  if ( WritePoolFile ) :
   from RegistrationServices.OutputConditionsAlg import OutputConditionsAlg
   OutputConditionsAlg2=OutputConditionsAlg("OutputConditionsAlg2",OutputPoolFileDir+"/"+OutputPoolFileName,
                                           [OutputObjectSpecOFC],[OutputObjectSpecTagOFC],WriteIOV)
   OutputConditionsAlg2.Run1 = IOVBegin
   if IOVEnd>0:
      OutputConditionsAlg2.Run2 = IOVEnd
   

  if (WriteNtuple):
   from LArCalibTools.LArCalibToolsConf import LArOFC2Ntuple
   LArOFC2Ntuple = LArOFC2Ntuple("LArOFC2Ntuple")
   LArOFC2Ntuple.ContainerKey = OFCKey 	   
   LArOFC2Ntuple.NtupleFile = "FILE2" 	   
   LArOFC2Ntuple.AddFEBTempInfo = False 	   
   LArOFC2Ntuple.isSC = SuperCells
   topSequence+=LArOFC2Ntuple

   if os.path.exists(OutputRootFileDir+"/"+OutputOFCRootFileName): 
      os.remove(OutputRootFileDir+"/"+OutputOFCRootFileName)  
   svcMgr += NTupleSvc()
   svcMgr.NTupleSvc.Output += [ "FILE2 DATAFILE='"+OutputRootFileDir+"/"+OutputOFCRootFileName+"' OPT='NEW'" ]
   
   
###########################################################################

svcMgr.MessageSvc.OutputLevel  = INFO
svcMgr.MessageSvc.defaultLimit = 10000
svcMgr.MessageSvc.Format       = "% F%20W%S%7W%R%T %0W%M"

svcMgr+=CfgMgr.AthenaEventLoopMgr(OutputLevel = WARNING)

from AthenaCommon.AppMgr import theAuditorSvc
from AthenaCommon.ConfigurableDb import getConfigurable
theAuditorSvc += getConfigurable("MemStatAuditor")(OutputLevel = WARNING)
theAuditorSvc += getConfigurable("ChronoAuditor")()
theAuditorSvc += getConfigurable("NameAuditor")()

###########################################################################

