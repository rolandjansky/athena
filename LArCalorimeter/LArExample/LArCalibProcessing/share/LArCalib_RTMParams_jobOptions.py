# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from future import standard_library
standard_library.install_aliases()
import subprocess

###########################################################################
#
# <Marco.Delmastro@cern.ch>
#
# Example jobOptions to extract RTM parameters from Cali/Master waveforms
#
# Last update: 09/12/2008 <Fabien.Tarrade@cern.ch>
#
###########################################################################

if not "SuperCells" in dir():
   SuperCells=False
   
if not SuperCells: include("LArCalibProcessing/LArCalib_Flags.py")
if SuperCells:     include("LArCalibProcessing/LArCalib_FlagsSC.py")

###########################################################################
# Input selection
###########################################################################

## RunNumber trigger IOV if reading from COOL is eneabled

if not 'SubDet' in dir():
   SubDet = "Barrel"  

if not 'Partition' in dir():
   if (SubDet=='EndCap'):
      Partition = "EB-EC*"
   else:
      Partition = "EB-EMB*"

from string import *
def DBConnectionFile(sqlitefile):  
   return "sqlite://;schema="+sqlitefile+";dbname=CONDBR2"

if not 'DBConnectionCOOL' in dir():
   DBConnectionCOOL = "oracle://ATLAS_COOLPROD;schema=ATLAS_COOLOFL_LAR;dbname=CONDBR2;"


if not 'StripsXtalkCorr' in dir():
   StripsXtalkCorr = True 

if not 'ChannelSelection' in dir():
   ChannelSelection = " "  # Read all
   ## Example to read only cool for Barrel C : Strips->Back
   #ChannelSelection = "<channelSelection>0,35:66</channelSelection>" 
   

## CaliWave
      
if not 'ReadCaliWaveFromCOOL' in dir():
   ReadCaliWaveFromCOOL = True

if not 'InputCaliWavePoolDir' in dir():
   InputCaliWavePoolDir = subprocess.getoutput("pwd")

if not 'InputCaliWavePoolFileName' in dir():
   InputCaliWavePoolFileName = "LArCaliWave.pool.root"

## CaliPulseParams
   
if not 'ReadCaliPulseParamsFromCOOL' in dir():
   ReadCaliPulseParamsFromCOOL = False

if not 'InputCaliPulseParamsFolder' in dir():
   if not SuperCells: InputCaliPulseParamsFolder = "/LAR/ElecCalibOfl/CaliPulseParams/RTM" 
   if SuperCells:     InputCaliPulseParamsFolder = "/LAR/ElecCalibOflSC/CaliPulseParams/RTM" 


if not 'InputCaliPulseParamsPoolDir' in dir():
   InputCaliPulseParamsPoolDir = subprocess.getoutput("pwd")

if not 'InputCaliPulseParamsPoolFileName' in dir():
   InputPulseParamsPoolFileName = "LArCaliPulseParamsVsCalib_AllBoards.pool.root"

## DetCellParams
   
if not 'ReadDetCellParamsFromCOOL' in dir():
   ReadDetCellParamsFromCOOL = False

if not 'InputDetCellParamsFolder' in dir():
   if not SuperCells: InputDetCellParamsFolder = "/LAR/ElecCalibOfl/DetCellParams/RTM"  
   if SuperCells:     InputDetCellParamsFolder = "/LAR/ElecCalibOflSC/DetCellParams/RTM"  

if not 'InputDetCellParamsPoolDir' in dir():
   InputDetCellParamsPoolDir = subprocess.getoutput("pwd")

if not 'InputDetCellParamsPoolFileName' in dir():
   InputDetCellParamsPoolFileName = "LArDetCellParams.pool.root"
   
###########################################################################
#  RTM algorithms steering option defaults
###########################################################################

###################################
#  Basic extraction defaults      #
###################################

if not 'ExtractTcal'  in dir():
   ExtractTcal  = True

if not 'ExtractFstep' in dir():
   ExtractFstep = True

if not 'ExtractOmega0' in dir():
   ExtractOmega0  = True

if not 'ExtractTaur'  in dir():
   ExtractTaur  = True

###################################
#     Extract All Parameters      #
###################################

if ( 'ExtractAllParameters' in dir() and ExtractAllParameters ) :

   ReadCaliPulseParamsFromCOOL = False
   ExtractTcal   = True
   ExtractFstep  = True

   ReadDetCellParamsFromCOOL   = False
   ExtractOmega0 = True
   ExtractTaur   = True
   
###################################
#      Extract only tauR          #
###################################

if ( 'ExtractTauROnly' in dir() and ExtractTauROnly ) :

   ReadCaliPulseParamsFromCOOL = True
   ExtractTcal   = False
   ExtractFstep  = False
   
   ReadDetCellParamsFromCOOL   = True
   ExtractOmega0 = False
   ExtractTaur   = True

## If needed, reads the LArCalibPulseParams from COOL (Oracle/SQLite)

if ( ReadCaliPulseParamsFromCOOL ):
   if 'InputCaliPulseParamsSQLiteFile' in dir():
      InputDBConnectionCaliPulseParams = DBConnectionFile(InputCaliPulseParamsSQLiteFile)
   else:
      InputDBConnectionCaliPulseParams = DBConnectionCOOL

## If needed, reads the LArDetCellParams (for Omega0 values) from COOL (Oracle/SQLite)

if ( ReadDetCellParamsFromCOOL ):
   if 'InputDetCellParamsSQLiteFile' in dir():
      InputDBConnectionDetCellParams = DBConnectionFile(InputDetCellParamsSQLiteFile)
   else:
      InputDBConnectionDetCellParams = DBConnectionCOOL

if ( ReadCaliWaveFromCOOL ):      
   if 'InputCaliWaveSQLiteFile' in dir():
      InputDBConnectionCaliWave = DBConnectionFile(InputCaliWaveSQLiteFile)
   else:
      InputDBConnectionCaliWave = DBConnectionCOOL

###########################################################################

if not 'doTest' in dir():
   doTest = False        

if not 'SuffixRetrievedCaliPulse' in dir():
   SuffixRetrievedCaliPulse = ""
        
if not 'SuffixRetrievedDetCell' in dir():
   SuffixRetrievedDetCell = ""

if not 'SuffixExtractedCaliPulse' in dir():
   SuffixExtractedCaliPulse = "_RTM"

if not 'SuffixExtractedDetCell' in dir():
   SuffixExtractedDetCell = "_RTM"

if not 'RecoverEmptyDB' in dir():
   RecoverEmptyDB = True

#
# Cosine response scan
#

if not 'UseOmegaScanHelper' in dir():
   UseOmegaScanHelper = True

if not 'CosRespScan' in dir():
   CosRespScan  = [ False ,  True , False , False ] # Strips only

if not 'Omega0Min' in dir():
   Omega0Min    = [ 0.100 , 0.220 ,  0.060 ,  0.100 ]

if not 'Omega0Max' in dir(): 
   Omega0Max    = [ 0.600 , 0.310 ,  0.215 ,  0.700 ]

if not 'NpointScan' in dir():
   NpointScan   = [   800 ,   900 ,    310 ,    120 ]

if not 'TShaper' in dir():
   TShaper = 15

if not 'CosRespTtailMin' in dir():
   CosRespTtailMin = 0 

if not 'CosRespTtailMax' in dir():
   CosRespTtailMax = 0

#
# Residual oscillation minimisation
#

if not 'DeltaTtailTaur' in dir():
   DeltaTtailTaur  = 100

#
# Special output
#

if not 'DumpOmegaScan' in dir():
   DumpOmegaScan = False

if not 'OmegaScanKey' in dir():
   OmegaScanKey = "OmegaScan"
	
if not 'DumpResOscill' in dir():
   DumpResOscill = False

if not 'ResOscillKeyBefore' in dir():
   ResOscillKeyBefore = "ResOscillBefore"

if not 'ResOscillKeyAfter' in dir():
   ResOscillKeyAfter  = "ResOscillAfter"

if not 'StoreResOscill' in dir():
   StoreResOscill = [ False , False , False , False ]

#
# FT/Slot selection
#

if not 'FTSelection' in dir():
   FTSelection   = False

if not 'PosNeg' in dir():
   PosNeg        = 0
	
if not 'FT' in dir():
   FT            = [ 0 ]

if not 'SlotSelection' in dir():
   SlotSelection = False

if not 'Slot' in dir():
   Slot          = [ 0 ]

#
# Input wave type
#

if not 'ContainerKey' in dir():
   ContainerKey = "LArCaliWave"

if not 'IgnoreDACSelection' in dir():
   IgnoreDACSelection = True

if not 'DAC' in dir():
   DAC = [ 500 , 4000, 40000 ] # High, Medium, Low


#######################################################
#                Monitoring properties
#######################################################
if not 'CheckBadEvents' in dir():
   CheckBadEvents = True


###########################################################################
#                     RTMParams output                                    #
###########################################################################

if not 'GroupingType' in dir():
   GroupingType = "ExtendedSubDetector"
   
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

if not 'OutputParamsRootFileDir' in dir():
   OutputParamsRootFileDir= subprocess.getoutput("pwd")
    
if not 'OutputPoolFileDir' in dir():
   OutputPoolFileDir= subprocess.getoutput("pwd")

if not 'RTMFileTag' in dir():
   RTMFileTag = "DefaultExtraction"
   for RunNumber in RunNumberList :
      RTMFileTag += "_"+str(RunNumber)
   RTMFileTag = RTMFileTag+"_"+Partition.replace("*","")

if not 'OutputParamsRootFileName' in dir():
   OutputParamsRootFileName = "LArParams_"+RTMFileTag + ".root"
   
if not 'OutputPoolFileName' in dir():
   OutputPoolFileName = "LArRTMParams_"+RTMFileTag + ".pool.root"
   
if not 'OutputCaliPulseParamsFolder' in dir():
   if not SuperCells: OutputCaliPulseParamsFolder = "/LAR/ElecCalibOfl/CaliPulseParams/RTM"
   if SuperCells:     OutputCaliPulseParamsFolder = "/LAR/ElecCalibOflSC/CaliPulseParams/RTM"
   
if not 'OutputDetCellParamsFolder' in dir():
   if not SuperCells: OutputDetCellParamsFolder = "/LAR/ElecCalibOfl/DetCellParams/RTM"
   if SuperCells:     OutputDetCellParamsFolder = "/LAR/ElecCalibOflSC/DetCellParams/RTM"

if not StripsXtalkCorr:
   CaliWaveFolder = LArCalib_Flags.LArCaliWaveFolder
else:
   CaliWaveFolder = LArCalib_Flags.LArCaliWaveFolderXtlk

rs=FolderTagResover()
if not "LArDetCellParamsTag" in dir():
   LArDetCellParamsTag=rs.getFolderTagSuffix(OutputDetCellParamsFolder)

if not "LArCaliPulseParamsTag" in dir():
   LArCaliPulseParamsTag=rs.getFolderTagSuffix(OutputCaliPulseParamsFolder)

if not "CaliWaveLArCalibFolderTag" in dir ():
   CaliWaveLArCalibFolderTag=rs.getFolderTagSuffix(CaliWaveFolder)

del rs #Close database

if not 'OutputDB' in dir():
   OutputDB = LArCalib_Flags.OutputDB

if 'OutputSQLiteFile' in dir():
   OutputDB = DBConnectionFile(OutputSQLiteFile)

if not 'OutputObjectSpecCaliPulseParams' in dir():
   OutputObjectSpecCaliPulseParams    = "LArCaliPulseParamsComplete#"+"LArCaliPulseParams_RTM#"+ OutputCaliPulseParamsFolder
   OutputObjectSpecTagCaliPulseParams = LArCalibFolderTag(OutputCaliPulseParamsFolder,LArCaliPulseParamsTag,)
						
if not 'OutputObjectSpecDetCellParams' in dir():
   OutputObjectSpecDetCellParams      = "LArDetCellParamsComplete#"+"LArDetCellParams_RTM#"+ OutputDetCellParamsFolder		
   OutputObjectSpecTagDetCellParams   = LArCalibFolderTag(OutputDetCellParamsFolder,LArDetCellParamsTag)

## Bad Channel   
   
if not 'ReadBadChannelFromCOOL' in dir():
   ReadBadChannelFromCOOL = True   

if ( ReadBadChannelFromCOOL ):      
   if 'InputBadChannelSQLiteFile' in dir():
      InputDBConnectionBadChannel = DBConnectionFile(InputBadChannelSQLiteFile)
   else:
      if 'InputDBConnectionBadChannel' not in dir():
         InputDBConnectionBadChannel = "COOLOFL_LAR/CONDBR2"      

###########################################################################
#                            Print summary
###########################################################################

RTMParamsLog = logging.getLogger( "RTMParamsLog" )
RTMParamsLog.info( " ========================================================= " )
RTMParamsLog.info( " ***               LAr RTMParams summary               *** " )
RTMParamsLog.info( " ========================================================= " )
for RunNumber in RunNumberList :
   RTMParamsLog.info( " RunNumber                                  = "+str(RunNumber) )

if ( ReadCaliWaveFromCOOL ):
   RTMParamsLog.info( " InputDBConnectionCaliWave                  = "+InputDBConnectionCaliWave )
   RTMParamsLog.info( " CaliWaveLArCalibFolderTag                  = "+CaliWaveLArCalibFolderTag )   
else :
   RTMParamsLog.info( " InputCaliWavePoolFileName                  = "+InputCaliWavePoolFileName )

if ( ReadCaliWaveFromCOOL and ChannelSelection != " " ):
   RTMParamsLog.info( " ChannelSelection                           = "+ChannelSelection )

if ( ReadCaliPulseParamsFromCOOL ):
   RTMParamsLog.info( " InputDBConnectionCaliPulseParams           = "+InputDBConnectionCaliPulseParams )
   RTMParamsLog.info( " InputCaliPulseParamsFolder                 = "+InputCaliPulseParamsFolder )
   if  "CaliPulseParamsLArCalibFolderTag" in dir() :    
      RTMParamsLog.info( " CaliPulseParamsLArCalibFolderTag           = "+CaliPulseParamsLArCalibFolderTag )

if ( ReadDetCellParamsFromCOOL ):
   RTMParamsLog.info( " InputDBConnectionDetCellParams             = "+InputDBConnectionDetCellParams )
   RTMParamsLog.info( " InputDetCellParamsFolder                   = "+InputDetCellParamsFolder )
   if "DetCellParamsLArCalibFolderTag" in dir() :
      RTMParamsLog.info( " DetCellParamsLArCalibFolderTag             = "+DetCellParamsLArCalibFolderTag )
   
if ( not ExtractTcal or not ExtractFstep and not ReadCaliPulseParamsFromCOOL ):
   RTMParamsLog.info( " InputCaliPulseParamsPoolFileName           = "+InputPulseParamsPoolFileName )

if ( not ExtractOmega0 or not ExtractTaur and not ReadDetCellParamsFromCOOL ):
   RTMParamsLog.info( " InputDetCellParamsPoolFileName             = "+InputDetCellParamsPoolFileName )

if ( WriteNtuple ):
   RTMParamsLog.info( " OutputParamsRootFullFileName               = "+OutputParamsRootFileDir+"/"+OutputParamsRootFileName )

if (WritePoolFile):
   RTMParamsLog.info( " OutputParamsPoolFullFileName          = "+OutputPoolFileDir+"/"+OutputPoolFileName )
   RTMParamsLog.info( " OutputObjectSpecCaliPulseParams            = "+str(OutputObjectSpecCaliPulseParams) )
   RTMParamsLog.info( " OutputObjectSpecTagCaliPulseParams         = "+str(OutputObjectSpecTagCaliPulseParams) )
   RTMParamsLog.info( " OutputObjectSpecDetCellParams              = "+str(OutputObjectSpecDetCellParams) )
   RTMParamsLog.info( " OutputObjectSpecTagDetCellParams           = "+str(OutputObjectSpecTagDetCellParams) )

if (WriteIOV):
   RTMParamsLog.info( " IOVBegin                                   = "+str(IOVBegin) )
   RTMParamsLog.info( " IOVEnd                                     = "+str(IOVEnd) )
   RTMParamsLog.info( " LArCalibOutputDB                           = "+OutputDB )
   RTMParamsLog.info( " OutputDetCellParamsFolder                  = "+OutputDetCellParamsFolder )
   RTMParamsLog.info( " OutputDetCellParamsFolder                  = "+OutputDetCellParamsFolder )
   
RTMParamsLog.info( " ======================================================== " )

###########################################################################
#
#                           Global settings
#
###########################################################################

include( "AthenaCommon/Atlas_Gen.UnixStandardJob.py" )

## get a handle to the default top-level algorithm sequence
from AthenaCommon.AlgSequence import AlgSequence 
topSequence = AlgSequence()

from AthenaCommon.AlgSequence import AthSequencer
condSeq = AthSequencer("AthCondSeq")


## get a handle to the ApplicationManager, to the ServiceManager and to the ToolSvc
from AthenaCommon.AppMgr import (theApp, ServiceMgr as svcMgr,ToolSvc)

include("LArCalibProcessing/LArCalib_MinimalSetup.py")
if SuperCells:
  from LArCabling.LArCablingAccess import LArOnOffIdMappingSC,LArCalibIdMappingSC
  LArOnOffIdMappingSC()
  LArCalibIdMappingSC()

###########################################################################
#                                                                         #
#    Input conditions data (CaliWave) for COOL and/or POOL                #
#                                                                         #
###########################################################################

include("AthenaPoolCnvSvc/AthenaPool_jobOptions.py")
include("LArCondAthenaPool/LArCondAthenaPool_joboptions.py")

from IOVDbSvc.CondDB import conddb
PoolFileList = []

## Bad Channel   

if 'BadChannelsFolder' not in dir():
   BadChannelsFolder="/LAR/BadChannels/BadChannels"
if 'MissingFEBsFolder' not in dir():
   MissingFEBsFolder="/LAR/BadChannels/MissingFEBs"

if not 'InputBadChannelSQLiteFile' in dir():
   RTMParamsLog.info( "Read Bad Channels from Oracle DB")
else :   
   RTMParamsLog.info( "Read Bad Channels from SQLite file") 

if 'BadChannelsLArCalibFolderTag' in dir() :
   BadChannelsTagSpec = LArCalibFolderTag (BadChannelsFolder,BadChannelsLArCalibFolderTag) 
   conddb.addFolder("",BadChannelsFolder+"<tag>"+BadChannelsTagSpec+"</tag>"+"<dbConnection>"+InputDBConnectionBadChannel+"</dbConnection>",
                    className="CondAttrListCollection")
else :
   conddb.addFolder("",BadChannelsFolder+"<dbConnection>"+InputDBConnectionBadChannel+"</dbConnection>",className="CondAttrListCollection")

if 'MissingFEBsLArCalibFolderTag' in dir() :
   MissingFEBsTagSpec = LArCalibFolderTag (MissingFEBsFolder,MissingFEBsLArCalibFolderTag)   
   conddb.addFolder("",MissingFEBsFolder+"<tag>"+MissingFEBsTagSpec+"</tag>"+"<dbConnection>"+InputDBConnectionBadChannel+"</dbConnection>",
                    className='AthenaAttributeList')
else :
   conddb.addFolder("",MissingFEBsFolder+"<dbConnection>"+InputDBConnectionBadChannel+"</dbConnection>",className='AthenaAttributeList')

from LArBadChannelTool.LArBadChannelToolConf import LArBadChannelCondAlg, LArBadFebCondAlg
theLArBadChannelCondAlg=LArBadChannelCondAlg(ReadKey=BadChannelsFolder)
condSeq+=theLArBadChannelCondAlg

theLArBadFebCondAlg=LArBadFebCondAlg(ReadKey=MissingFEBsFolder)
condSeq+=theLArBadFebCondAlg
 
# This algorithm verifies that no FEBs are dropping out of the run
# If it finds corrupt events, it breaks the event loop and terminates the job rapidly
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

if SuperCells:
   conddb.addFolder("","/LAR/IdentifierOfl/OnOffIdMap_SC<db>COOLOFL_LAR/OFLP200</db><tag>LARIdentifierOflOnOffIdMap_SC-000</tag>") 

## define the DB Gobal Tag :
svcMgr.IOVDbSvc.GlobalTag   = LArCalib_Flags.globalFlagDB
try:
   svcMgr.IOVDbSvc.DBInstance=""
except: 
   pass

from LArCalibProcessing.LArCalibCatalogs import larCalibCatalogs
svcMgr.PoolSvc.ReadCatalog += larCalibCatalogs

if (  ReadCaliWaveFromCOOL ) :
   if not 'InputCaliWaveSQLiteFile' in dir():
      RTMParamsLog.info( "Read CaliWave from Oracle DB" )
   else :
      RTMParamsLog.info( "Read CaliWave from SQLite file" )

if ( not ExtractTcal or not ExtractFstep ):
   if ( ReadCaliPulseParamsFromCOOL  ) :
      if not 'InputCaliPulseParamsSQLiteFile' in dir():
         RTMParamsLog.info( "Read CaliPulseParams from Oracle DB" )
      else :
         RTMParamsLog.info( "Read CaliPulseParams from SQLite file" )

if ( not ExtractOmega0 or not ExtractTaur):
    if ( ReadDetCellParamsFromCOOL  ) :
      if not 'InputDetCellParamsSQLiteFile' in dir():
         RTMParamsLog.info( "Read DetCellParams from Oracle DB" )
      else :
         RTMParamsLog.info( "Read DetCellParams from SQLite file" )     

## CaliWave
if ( ReadCaliWaveFromCOOL ):
   CaliWaveTagSpec = LArCalibFolderTag(CaliWaveFolder,CaliWaveLArCalibFolderTag)
   conddb.addFolder("",CaliWaveFolder+"<tag>"+CaliWaveTagSpec+"</tag>"+"<dbConnection>"+InputDBConnectionCaliWave+"</dbConnection>" + ChannelSelection)
else:
   if 'InputCaliWavePoolFileName' in dir():
      RTMParamsLog.info("Read CaliWave from POOL file")
      PoolFileList += [ InputCaliWavePoolFileDir+"/"+InputCaliWavePoolFileName ]
   else:
      RTMParamsLog.error("No PoolFileList found! Please list the POOL files containing CaliWave or read from COOL.")
      theApp.exit(-1)

## CaliPulseParams
if ( not ExtractTcal or not ExtractFstep ): # Load CaliPulseParams from COOL or POOL only if extraction is not selected
   if ( ReadCaliPulseParamsFromCOOL ):
      if 'CaliPulseParamsLArCalibFolderTag' in dir():
         CaliPulseParamsTagSpec = LArCalibFolderTag(InputCaliPulseParamsFolder,CaliPulseParamsLArCalibFolderTag)
         conddb.addFolder("",InputCaliPulseParamsFolder+"<tag>"+CaliPulseParamsTagSpec+"</tag>"+"<dbConnection>"+InputDBConnectionCaliPulseParams+"</dbConnection>" + ChannelSelection)
      else :
         conddb.addFolder("",InputCaliPulseParamsFolder+"<key>LArCaliPulseParams</key><dbConnection>"+InputDBConnectionCaliPulseParams+"</dbConnection>" + ChannelSelection)
   else:
      if 'InputCaliPulseParamsPoolFileName' in dir():
         RTMParamsLog.info("Read CaliPulseParams from POOL file")
         PoolFileList += [ InputCaliPulseParamsPoolDir+"/"+InputCaliPulseParamsPoolFileName ]
      else:
         RTMParamsLog.error("No PoolFileList found! Please list the POOL files containing CaliPulseParams or read from COOL.")
         theApp.exit(-1)

## DetCellParams
if ( not ExtractOmega0 or not ExtractTaur): # Load DetCellParams from COOL or POOL only if extraction is not selected
   if ( ReadDetCellParamsFromCOOL ):
      if 'DetCellParamsLArCalibFolderTag' in dir():
         DetCellParamsTagSpec = LArCalibFolderTag(InputDetCellParamsFolder,DetCellParamsLArCalibFolderTag)
         conddb.addFolder("",InputDetCellParamsFolder+"<tag>"+DetCellParamsTagSpec+"</tag>"+"<dbConnection>"+InputDBConnectionDetCellParams+"</dbConnection>" + ChannelSelection)
      else :
         conddb.addFolder("",InputDetCellParamsFolder+"<key>LArDetCellParams</key><dbConnection>"+InputDBConnectionDetCellParams+"</dbConnection>" + ChannelSelection)
   else:  
      if 'InputDetCellParamsPoolFileName' in dir():
         RTMParamsLog.info("Read DetCellParams from POOL file")
         PoolFileList += [ InputDetCellParamsPoolDir+"/"+InputDetCellParamsPoolFileName ]
      else:
         RTMParamsLog.error("No PoolFileList found! Please list the POOL files containing DetCellParams or read from COOL.")
         theApp.exit(-1)

if ( len(PoolFileList)>0 ):

   from AthenaCommon.ConfigurableDb import getConfigurable
   svcMgr += getConfigurable( "ProxyProviderSvc" )()
   svcMgr.ProxyProviderSvc.ProviderNames += [ "CondProxyProvider" ]
   
   svcMgr += getConfigurable( "CondProxyProvider" )()
   svcMgr.CondProxyProvider.InputCollections += PoolFileList
   
###########################################################################
#                                                                         #
#              Extract RTM parameters Cali/Master waveforms               #
#                                                                         #
###########################################################################

if not 'doTest' in dir():
   doTest = False

from LArCalibUtils.LArCalibUtilsConf import LArRTMParamExtractor
LArRTMParamExtractor = LArRTMParamExtractor("LArRTMParamExtractor")

LArRTMParamExtractor.KeyList        = [ ContainerKey ]
LArRTMParamExtractor.TestMode       = doTest
LArRTMParamExtractor.GroupingType   = GroupingType
LArRTMParamExtractor.DAC            = DAC
LArRTMParamExtractor.IgnoreDACSelection = IgnoreDACSelection
LArRTMParamExtractor.isSC           = SuperCells

LArRTMParamExtractor.ExtractTcal    = ExtractTcal
LArRTMParamExtractor.ExtractFstep   = ExtractFstep
LArRTMParamExtractor.ExtractOmega0  = ExtractOmega0
LArRTMParamExtractor.ExtractTaur    = ExtractTaur

LArRTMParamExtractor.RecoverEmptyDB = RecoverEmptyDB

LArRTMParamExtractor.SuffixRetrievedCaliPulse = SuffixRetrievedCaliPulse
LArRTMParamExtractor.SuffixRetrievedDetCell   = SuffixRetrievedDetCell
LArRTMParamExtractor.SuffixExtractedCaliPulse = SuffixExtractedCaliPulse
LArRTMParamExtractor.SuffixExtractedDetCell   = SuffixExtractedDetCell

LArRTMParamExtractor.FTSelection   = FTSelection
LArRTMParamExtractor.FT            = FT
LArRTMParamExtractor.PosNeg        = PosNeg

LArRTMParamExtractor.SlotSelection = SlotSelection
LArRTMParamExtractor.Slot          = Slot
	
LArRTMParamExtractor.DumpOmegaScan = DumpOmegaScan
LArRTMParamExtractor.OmegaScanKey  = OmegaScanKey 

LArRTMParamExtractor.DumpResOscill      = DumpResOscill
LArRTMParamExtractor.ResOscillKeyBefore = ResOscillKeyBefore
LArRTMParamExtractor.ResOscillKeyAfter  = ResOscillKeyAfter

from LArCalibUtils.LArCalibUtilsConf import LArWFParamTool
theLArWFParamTool = LArWFParamTool()
theLArWFParamTool.isSC = SuperCells
    
if 'NBaseline' in dir():
   theLArWFParamTool.NBaseline = NBaseline

if 'SubtractBaseline' in dir():
   theLArWFParamTool.SubtractBaseline = SubtractBaseline

if 'ShiftToStart' in dir():
   theLArWFParamTool.ShiftToStart = ShiftToStart

if 'TShaper' in dir():
   theLArWFParamTool.TShaper = TShaper

theLArWFParamTool.TtailMin = [ 0, 0, CosRespTtailMin, 0 ]
theLArWFParamTool.TtailMax = [ 0, 0, CosRespTtailMax, 0 ]

if 'UseOmegaScanHelper' in dir():
   theLArWFParamTool.UseOmegaScanHelper = UseOmegaScanHelper
if 'CosRespScan'in dir():
   theLArWFParamTool.CosRespScan = CosRespScan

if 'Omega0Min' in dir(): 
   theLArWFParamTool.Omega0Min   = Omega0Min  

if 'Omega0Max' in dir():
   theLArWFParamTool.Omega0Max   = Omega0Max

if 'NpointScan' in dir():
   theLArWFParamTool.NpointScan  = NpointScan 

if 'StoreResOscill' in dir():
   theLArWFParamTool.StoreResOscill = StoreResOscill

ToolSvc += theLArWFParamTool

topSequence += LArRTMParamExtractor

###########################################################################

if ( WriteNtuple ):

   from LArCalibTools.LArCalibToolsConf import LArWFParams2Ntuple
   LArWFParams2Ntuple = LArWFParams2Ntuple("LArWFParams2Ntuple")
   LArWFParams2Ntuple.DumpCaliPulseParams = True
   LArWFParams2Ntuple.DumpDetCellParams   = True
   LArWFParams2Ntuple.CaliPulseParamsKey="LArCaliPulseParams_RTM"
   LArWFParams2Ntuple.DetCellParamsKey="LArDetCellParams_RTM"
   LArWFParams2Ntuple.isSC = SuperCells
   topSequence += LArWFParams2Ntuple
   
   if ( DumpOmegaScan ):
      
      from LArCalibTools.LArCalibToolsConf import LArCaliWaves2Ntuple as LArOmegaScans2Ntuple
      LArOmegaScans2Ntuple = LArOmegaScans2Ntuple( "LArOmegaScans2Ntuple" )
      LArOmegaScans2Ntuple.NtupleName = "OMEGASCAN"
      LArOmegaScans2Ntuple.KeyList = [ OmegaScanKey ]
      
      topSequence += LArOmegaScans2Ntuple
		
   if ( DumpResOscill ):

      from LArCalibTools.LArCalibToolsConf import LArCaliWaves2Ntuple as LArResOscillsBefore2Ntuple
      LArResOscillsBefore2Ntuple = LArResOscillsBefore2Ntuple( "LArResOscillsBefore2Ntuple" )
      LArResOscillsBefore2Ntuple.NtupleName = "RESOSCILLBEFORE"
      LArResOscillsBefore2Ntuple.KeyList = [ ResOscillKeyBefore ]

      topSequence += LArResOscillsBefore2Ntuple
		
      from LArCalibTools.LArCalibToolsConf import LArCaliWaves2Ntuple as LArResOscillsAfter2Ntuple
      LArResOscillsAfter2Ntuple = LArResOscillsAfter2Ntuple( "LArResOscillsAfter2Ntuple" )
      LArResOscillsAfter2Ntuple.NtupleName = "RESOSCILLAFTER"
      LArResOscillsAfter2Ntuple.KeyList = [ ResOscillKeyAfter ]

      topSequence += LArResOscillsAfter2Ntuple

   theApp.HistogramPersistency = "ROOT"
   from GaudiSvc.GaudiSvcConf import NTupleSvc
   if os.path.exists(OutputParamsRootFileDir+"/"+OutputParamsRootFileName): 
      os.remove(OutputParamsRootFileDir+"/"+OutputParamsRootFileName)  
   svcMgr += NTupleSvc()
   svcMgr.NTupleSvc.Output = [ "FILE1 DATAFILE='"+OutputParamsRootFileDir+"/"+OutputParamsRootFileName+"' OPT='NEW'" ]

if (  WritePoolFile ) :
   
   from RegistrationServices.OutputConditionsAlg import OutputConditionsAlg
   
   # write CaliPulseParams and DetCellParams
   if os.path.exists(OutputPoolFileDir+"/"+OutputPoolFileName): 
      os.remove(OutputPoolFileDir+"/"+OutputPoolFileName)  
   OutputConditionsAlgParams = OutputConditionsAlg("OutputConditionsAlgParams",OutputPoolFileDir+"/"+OutputPoolFileName,
                                                        [OutputObjectSpecCaliPulseParams],[OutputObjectSpecTagCaliPulseParams],WriteIOV)
   OutputConditionsAlgParams.ObjectList += [OutputObjectSpecDetCellParams]
   OutputConditionsAlgParams.IOVTagList += [OutputObjectSpecTagDetCellParams]

   OutputConditionsAlgParams.Run1     = IOVBegin
   if IOVEnd>0:
      OutputConditionsAlgParams.Run2  = IOVEnd
		
   svcMgr.IOVDbSvc.dbConnection  = OutputDB

   from RegistrationServices.RegistrationServicesConf import IOVRegistrationSvc
   svcMgr += IOVRegistrationSvc()
   svcMgr.IOVRegistrationSvc.OutputLevel = VERBOSE
   svcMgr.IOVRegistrationSvc.RecreateFolders = False	

###########################################################################
#                Use EventSelector to select IOV                          #
###########################################################################

from McEventSelector.McEventSelectorConf import McEventSelector
svcMgr += McEventSelector("EventSelector")
svcMgr.EventSelector.RunNumber = int(RunNumberList[0])
svcMgr.EventSelector.EventsPerRun      = 1
svcMgr.EventSelector.FirstEvent	       = 0
svcMgr.EventSelector.InitialTimeStamp  = 0
svcMgr.EventSelector.TimeStampInterval = 1

##########################################################################
#          don't remove otherwise infinite loop                          #
##########################################################################

theApp.EvtMax = 1

###########################################################################

svcMgr.MessageSvc.OutputLevel  = WARNING
svcMgr.MessageSvc.defaultLimit = 10000
svcMgr.MessageSvc.Format       = "% F%20W%S%7W%R%T %0W%M"

svcMgr+=CfgMgr.AthenaEventLoopMgr(OutputLevel = INFO)

from AthenaCommon.AppMgr import theAuditorSvc
from AthenaCommon.ConfigurableDb import getConfigurable
theAuditorSvc += getConfigurable("MemStatAuditor")(OutputLevel = DEBUG)
theAuditorSvc += getConfigurable("ChronoAuditor")()
theAuditorSvc += getConfigurable("NameAuditor")()

###########################################################################
