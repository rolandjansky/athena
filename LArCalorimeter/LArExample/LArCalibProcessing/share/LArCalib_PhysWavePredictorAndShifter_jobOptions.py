# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from future import standard_library
standard_library.install_aliases()
import subprocess

###########################################################################
#
# <Marco.Delmastro@cern.ch>
#
# Example jobOptions to compute PhysWavePredicted and Shifted
#
# Last update: 09/12/2008 <Fabien.Tarrade@cern.ch>
#
###########################################################################

if not "SuperCells" in dir():
   SuperCells=False
   
if not SuperCells: include("LArCalibProcessing/LArCalib_Flags.py")
if SuperCells:     include("LArCalibProcessing/LArCalib_FlagsSC.py")

###########################################################################
#                   Input selection (CaliWave)
###########################################################################

# RunNumber trigger IOV if reading from COOL is eneabled
if not 'RunNumber' in dir():
   RunNumber = '0000000'

if not 'SubDet' in dir():
   SubDet = "Barrel"  

if not 'Partition' in dir():
   if (SubDet=='EndCap'):
      Partition      = "EB-EC*"
   else:
      Partition      = "EB-EMB*"

if not 'isHEC' in dir():
   isHEC= False

## HEC PhysWave
if not 'ReadHECPhysWaveFromCOOL' in dir():
   ReadHECPhysWaveFromCOOL = True
	
if not 'InputHECPhysWavePoolFileDir' in dir():
   InputHECMapPoolFileDir  = subprocess.getoutput("pwd")
   
if not 'InputHECPhysWavePoolFileName' in dir():
   InputHECPhysWavePoolFileName = "LArHECPhysWave.pool.root"   

## CaliWave      
if not 'ReadCaliWaveFromCOOL' in dir():
   ReadCaliWaveFromCOOL = True

if not 'InputCaliWavePoolFileDir' in dir():
   InputCaliWavePoolFileDir = subprocess.getoutput("pwd")

if not 'InputCaliWavePoolFileName' in dir():
   InputCaliWavePoolFileName = "LArCaliWave.pool.root"

## CaliPulseParams      
if not 'ReadCaliPulseParamsFromCOOL' in dir():
   ReadCaliPulseParamsFromCOOL = True

if not 'InputCaliPulseParamsPoolFileDir' in dir():
   InputCaliPulseParamsPoolFileDir = subprocess.getoutput("pwd")

if not 'InputCaliPulseParamsPoolFileName' in dir():
   InputCaliPulseParamsPoolFileName = "LArCaliPulseParamsVsCalib_AllBoards.pool.root"

## DetCellParams    
if not 'ReadDetCellParamsFromCOOL' in dir():
   ReadDetCellParamsFromCOOL = True

if not 'InputDetCellParamsPoolFileDir' in dir():
   InputDetCellParamsPoolFileDir = subprocess.getoutput("pwd")

if not 'InputDetCellParamsPoolFileName' in dir():
   InputDetCellParamsPoolFileName = "detector_EMECA_C_v1.pool.root"

## DTime      
if not 'ReadDTimeFromCOOL' in dir():
   ReadDTimeFromCOOL = True

if not 'InputDTimePoolFileDir' in dir():
   InputDTimePoolFileDir = subprocess.getoutput("pwd")

if not 'InputDTimePoolFileName' in dir():
   InputDTimePoolFileName = "tdrift_EMECA_C_v1.pool.root"

## Options
   
if not 'StripsXtalkCorr' in dir():
   StripsXtalkCorr = True
   
if (isHEC) :
    StripsXtalkCorr=False

if not 'ChannelSelection' in dir():
   # Read all
   ChannelSelection = " "
   ## Example to read only cool for Barrel C : Strips->Back
   #ChannelSelection = "<channelSelection>0,35:66</channelSelection>" 

from string import *
def DBConnectionFile(sqlitefile):  
   return "sqlite://;schema="+sqlitefile+";dbname=CONDBR2"

if not 'DBConnectionCOOL' in dir():
   DBConnectionCOOL = "oracle://ATLAS_COOLPROD;schema=ATLAS_COOLOFL_LAR;dbname=CONDBR2;"

if not 'InputCaliPulseParamsFolder' in dir():
   if not SuperCells: InputCaliPulseParamsFolder = "/LAR/ElecCalibOfl/CaliPulseParams/RTM"
   if SuperCells:     InputCaliPulseParamsFolder = "/LAR/ElecCalibOflSC/CaliPulseParams/RTM"
   
if not 'InputDetCellParamsFolder' in dir():
   if not SuperCells: InputDetCellParamsFolder = "/LAR/ElecCalibOfl/DetCellParams/RTM"   
   if SuperCells:     InputDetCellParamsFolder = "/LAR/ElecCalibOflSC/DetCellParams/RTM"   

## If needed, reads the LArCalibPulseParams from COOL (Oracle/SQLite)

if ( ReadCaliPulseParamsFromCOOL ):
   if 'InputCaliPulseParamsSQLiteFile' in dir():
      InputDBConnectionCaliPulseParams = DBConnectionFile(InputCaliPulseParamsSQLiteFile)
   else:
      InputDBConnectionCaliPulseParams = DBConnectionCOOL

## HEC PhysWave
if ( ReadHECPhysWaveFromCOOL ):
   if 'InputHECPhysWaveSQLiteFile' in dir():
      InputDBConnectionHECPhysWave = DBConnectionFile(InputHECPhysWaveSQLiteFile)
   else:
      InputDBConnectionHECPhysWave = DBConnectionCOOL 

## CaliWave
if ( ReadCaliWaveFromCOOL ):      
   if 'InputCaliWaveSQLiteFile' in dir():
      InputDBConnectionCaliWave = DBConnectionFile(InputCaliWaveSQLiteFile)
   else:
      InputDBConnectionCaliWave = DBConnectionCOOL


## DetCellParams      
if ( ReadDetCellParamsFromCOOL ):      
   if 'InputDetCellParamsSQLiteFile' in dir():
       InputDBConnectionDetCellParams= DBConnectionFile(InputDetCellParamsSQLiteFile)
   else:
       InputDBConnectionDetCellParams= DBConnectionCOOL

## DTime
if ( ReadDTimeFromCOOL ):      
   if 'InputDTimeSQLiteFile' in dir():
      InputDBConnectionDTime = DBConnectionFile(InputDTimeSQLiteFile)
   else:
      InputDBConnectionDTime = DBConnectionCOOL
      
###########################################################################
#                   PhysWavePredictor properties
###########################################################################

if not 'HECPhysWaveKey' in dir(): 
   HECPhysWaveKey="LArPhysWaveHECIdeal"  

if not 'KeyCaliList' in dir():
   KeyCaliList   = [ "LArCaliWave" ]
        
if not 'LArMphysMcaliKey' in dir ():
   LArMphysMcaliKey = "LArMphysOverMcal"

if not 'doTest' in dir():
   doTest = False

   
##############################################
   
# Tcali, Fstep
if not 'UseCaliPulseParamsFromJO' in dir():
   UseCaliPulseParamsFromJO = False # pb to read the poolfile

# Omega0, Taur
if not 'UseDetCellParamsFromJO' in dir():
   UseDetCellParamsFromJO = False

# Tdrift
if not 'UseTdriftFromJO' in dir():
   UseTdriftFromJO   = False 

# Tdiff
if not 'UseTimeShiftFromJO' in dir():
   UseTimeShiftFromJO = True

###############################################
# part of software + DB missing so use   
if not 'UseDoubleTriangle' in dir():
   if (isHEC) :
      UseDoubleTriangle = False
   else :
      UseDoubleTriangle = False ##normally True but software not ready   

if not 'HV' in dir(): 
   HV = 2000

if ( HV == 2000 ):
   TdriftVector  = [ 420  ,     469 ,     469 ,     469 ]
   TdriftVector2 = [ 420  ,     921 ,     921 ,     921 ]
   TdriftWeight2 = [   0. ,  0.0672 ,  0.0672 ,  0.0672 ]
elif ( HV == 1600 ):
   TdriftVector  = [ 451.92 , 504.644 , 504.644 , 504.644 ]
   TdriftVector2 = [ 451.92 , 990.996 , 990.996 , 990.996 ]
   TdriftWeight2 = [   0.   ,  0.0672 ,  0.0672 ,  0.0672 ]

if not 'InjPointCorrLayer' in dir():
   InjPointCorrLayer = [ 1, 1, 1, 1 ]

if not 'InjPointUseTauR' in dir():
   InjPointUseTauR   = [ 1, 1, 1, 1 ]

if not 'WritePhysWave2NTuple' in dir():
   WritePhysWave2NTuple = True  # write predicted LArPhysWave to ntuple

if not 'WriteCaliWave2NTuple' in dir():
   WriteCaliWave2NTuple = False # add LArCaliWave to predicted LArPhysWave ntuple

if not 'WriteMphyMcal2NTuple' in dir():
   WriteMphyMcal2NTuple = True  # add LArMphysOverMcalComplete to predicted LArPhysWave ntuple


###########################################################################
#                PhysWaveShifter properties                               #
###########################################################################

if not 'doPhysWaveShifter' in dir():
   doPhysWaveShifter = False 

if not 'ShiftMode' in dir():
   ShiftMode = 2 # 1 = minimum Tstart from helper in FEB  # 2 = minumum Tpeak-NindexFromPeak in FEB

if not 'InputKey' in dir():
   if (doPhysWaveShifter) :
      InputKey = "LArPhysWaveUnShifted"
   else :
      InputKey = "LArPhysWave"  # no shift applied to the PhysWave   

if not 'OutputKey' in dir():
   OutputKey = "LArPhysWave"
      
if not 'Nsamplings' in dir():
   Nsamplings = 2

if not 'TimeShiftGuardRegion' in dir():
   TimeShiftGuardRegion = 5
   
#######################################################
#                     PhysWavePredicted output
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
   IOVBegin = int(RunNumber)

if not 'IOVEnd' in dir():
   IOVEnd = LArCalib_Flags.IOVEnd

if not 'OutputPhysWaveRootFileDir' in dir():
    OutputPhysWaveRootFileDir= subprocess.getoutput("pwd")
    
if not 'OutputPhysWavePoolFileDir' in dir():
    OutputPhysWavePoolFileDir= subprocess.getoutput("pwd")

if not 'OutputMphysOverMcalPoolFileDir' in dir():
    OutputMphysOverMcalPoolFileDir= subprocess.getoutput("pwd")    

PhysWaveFileTag = str(RunNumber)+"_"+Partition.replace("*","")

if not 'OutputPhysWaveRootFileName' in dir():
   OutputPhysWaveRootFileName = "LArPhysWave_RTM_"+PhysWaveFileTag + ".root"
   
if not 'OutputPhysWavePoolFileName' in dir():
   OutputPhysWavePoolFileName = "LArPhysWave_RTM_"+PhysWaveFileTag + ".pool.root"

if not 'OutputMphysOverMcalPoolFileName' in dir():
   OutputMphysOverMcalPoolFileName = "LArMphysOverMcal_RTM_"+ PhysWaveFileTag+ ".pool.root"


if not StripsXtalkCorr:
   CaliWaveFolder = LArCalib_Flags.LArCaliWaveFolder
else:
   CaliWaveFolder = LArCalib_Flags.LArCaliWaveFolderXtlk


HECPhysWaveFolder  = "/LAR/ElecCalibOfl/PhysWaves/HECIdeal"


rs=FolderTagResover()
if not "LArDetCellParamsTag" in dir():
   LArDetCellParamsTag=rs.getFolderTagSuffix(InputDetCellParamsFolder)

if not "LArCaliPulseParamsTag" in dir():
   LArCaliPulseParamsTag=rs.getFolderTagSuffix(InputCaliPulseParamsFolder)

if not "CaliWaveLArCalibFolderTag" in dir ():
   CaliWaveLArCalibFolderTag=rs.getFolderTagSuffix(CaliWaveFolder)

if not 'LArPhysFolderOutputTag' in dir():
   LArPhysFolderOutputTag=rs.getFolderTagSuffix(LArCalib_Flags.LArPhysWaveFolder)

if not 'LArPhysFolderOutputTag' in dir():
   LArPhysFolderOutputTag=rs.getFolderTagSuffix(LArCalib_Flags.LArMPhysOverMCalFolder)

if not 'DTimeLArCalibFolderTag' in dir():
   DTimeLArCalibFolderTag=rs.getFolderTagSuffix(LArCalib_Flags.LArDTimeFolder)

if not 'HECPhysWaveTagSpec' in dir():
   HECPhysWaveTagSpec=rs.getFolderTagSuffix(HECPhysWaveFolder)
del rs #Close database




if not 'OutputDB' in dir():
   OutputDB = LArCalib_Flags.OutputDB

if 'OutputSQLiteFile' in dir():
   OutputDB = DBConnectionFile(OutputSQLiteFile)

if not 'OutputObjectSpecPhysWave' in dir():
   OutputObjectSpecPhysWave   = "LArPhysWaveContainer#"+"LArPhysWave"+"#"+ LArCalib_Flags.LArPhysWaveFolder
   OutputObjectSpecTagPhysWave    = LArCalibFolderTag(LArCalib_Flags.LArPhysWaveFolder,LArPhysFolderOutputTag)
						
if not 'OutputObjectSpecMphysOverMcal' in dir():
   OutputObjectSpecMphysOverMcal = "LArMphysOverMcalComplete#"+"LArMphysOverMcal"+"#"+ LArCalib_Flags.LArMPhysOverMCalFolder		
   OutputObjectSpecTagMphysOverMcal  = LArCalibFolderTag(LArCalib_Flags.LArMPhysOverMCalFolder,LArPhysFolderOutputTag)

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

PhysWaveLog = logging.getLogger( "PhysWaveLog" )
PhysWaveLog.info( " ======================================================== " )
PhysWaveLog.info( " ***            LAr PredPhysWave summary              *** " )
PhysWaveLog.info( " ======================================================== " )
PhysWaveLog.info( " RunNumber                                  = "+str(RunNumber) )

if ( ReadCaliPulseParamsFromCOOL ):
   PhysWaveLog.info( " InputDBConnectionCaliPulseParams           = "+InputDBConnectionCaliPulseParams )
   PhysWaveLog.info( " InputCaliPulseParamsFolder                 = "+InputCaliPulseParamsFolder )
   if  "CaliPulseParamsLArCalibFolderTag" in dir() :    
      PhysWaveLog.info( " CaliPulseParamsLArCalibFolderTag           = "+CaliPulseParamsLArCalibFolderTag )


if ( ReadCaliWaveFromCOOL ):
   PhysWaveLog.info( " InputDBConnectionCaliWave                  = "+InputDBConnectionCaliWave )
else :
   PhysWaveLog.info( " InputCaliWavePoolFileName                  = "+InputCaliWavePoolFileName )
if ( (ReadCaliWaveFromCOOL) and ChannelSelection != " " ):
   PhysWaveLog.info( " ChannelSelection                           = "+ChannelSelection )

PhysWaveLog.info( " CaliWaveLArCalibFolderTag                  = "+CaliWaveLArCalibFolderTag )
PhysWaveLog.info( " OutputPhysWaveRootFullFileName             = "+OutputPhysWaveRootFileDir+"/"+OutputPhysWaveRootFileName )
PhysWaveLog.info( " OutputPhysWavePoolFullFileName             = "+OutputPhysWavePoolFileDir+"/"+OutputPhysWavePoolFileName )
PhysWaveLog.info( " OutputMphysOverMcalPoolFullFileName        = "+OutputMphysOverMcalPoolFileDir+"/"+OutputMphysOverMcalPoolFileName )
PhysWaveLog.info( " OutputObjectSpecPhysWave                   = "+str(OutputObjectSpecPhysWave) )
PhysWaveLog.info( " OutputObjectSpecTagPhysWave                = "+str(OutputObjectSpecTagPhysWave) )
PhysWaveLog.info( " OutputObjectSpecMphusOverMcal              = "+str(OutputObjectSpecMphysOverMcal) )
PhysWaveLog.info( " OutputObjectSpecTagMphysOverMcal           = "+str(OutputObjectSpecTagMphysOverMcal) )
PhysWaveLog.info( " IOVBegin                                   = "+str(IOVBegin) )
PhysWaveLog.info( " IOVEnd                                     = "+str(IOVEnd) )
PhysWaveLog.info( " LArCalibOutputDB                           = "+OutputDB )
PhysWaveLog.info( " ======================================================== " )

###########################################################################
#
#                           Global settings
#
###########################################################################
include ("LArConditionsCommon/LArMinimalSetup.py")
from LArCabling.LArCablingAccess import LArOnOffIdMapping
LArOnOffIdMapping()
if SuperCells:
  from LArCabling.LArCablingAccess import LArCalibIdMappingSC,LArOnOffIdMappingSC
  LArOnOffIdMappingSC()
  LArCalibIdMappingSC()


# Provides ByteStreamInputSvc name of the data file to process in the offline context
#

## get a handle to the default top-level algorithm sequence
from AthenaCommon.AlgSequence import AlgSequence 
topSequence = AlgSequence()
from AthenaCommon.AlgSequence import AthSequencer
condSeq = AthSequencer("AthCondSeq")

## get a handle to the ApplicationManager, to the ServiceManager and to the ToolSvc
from AthenaCommon.AppMgr import (theApp, ServiceMgr as svcMgr,ToolSvc)

include("LArCalibProcessing/LArCalib_MinimalSetup.py")

###########################################################################
#                                                                         #
#    Input conditions data (CaliWave) for COOL and/or POOL                #
#                                                                         #
###########################################################################

include("AthenaPoolCnvSvc/AthenaPool_jobOptions.py")
include("LArCondAthenaPool/LArCondAthenaPool_joboptions.py")

from IOVDbSvc.CondDB import conddb
PoolFileList     = []


if not 'InputBadChannelSQLiteFile' in dir():
   PhysWaveLog.info( "Read Bad Channels from Oracle DB")
else :   
   PhysWaveLog.info( "Read Bad Channels from SQLite file") 

if 'BadChannelsFolder' not in dir():
   BadChannelsFolder="/LAR/BadChannels/BadChannels"
if 'MissingFEBsFolder' not in dir():
   MissingFEBsFolder="/LAR/BadChannels/MissingFEBs"


if 'BadChannelsLArCalibFolderTag' in dir() :
   BadChannelsTagSpec = LArCalibFolderTag (BadChannelsFolder,BadChannelsLArCalibFolderTag) 
   conddb.addFolder("",BadChannelsFolder+"<tag>"+BadChannelsTagSpec+"</tag>"+"<dbConnection>"+InputDBConnectionBadChannel+"</dbConnection>",className="CondAttrListCollection")
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
      PhysWaveLog.info( "Read CaliWave from Oracle DB" )
   else :
      PhysWaveLog.info( "Read CaliWave from SQLite file" )

if ( ReadCaliPulseParamsFromCOOL ) :       
   if not 'InputCaliPulseParamsSQLiteFile' in dir():
      PhysWaveLog.info( "Read PulseParams from Oracle DB" )
   else :
      PhysWaveLog.info( "Read PulseParams from SQLite file" )

if ( ReadDetCellParamsFromCOOL  ) :      
   if not 'InputDetCellParamsSQLiteFile' in dir():
      PhysWaveLog.info( "Read DetCellParams from Oracle DB" )
   else :
      PhysWaveLog.info( "Read DetCellParams from SQLite file" )

if ( ReadDTimeFromCOOL ) :      
   if not 'InputDTimeSQLiteFile' in dir():
      PhysWaveLog.info( "Read DTime from Oracle DB" )
   else :
      PhysWaveLog.info( "Read DTime from SQLite file" )
      
if (isHEC):
   if (ReadHECPhysWaveFromCOOL ): 
      if not 'InputHECPhysWaveSQLiteFile' in dir():
         PhysWaveLog.info( "Read HEC PhysWave from Oracle DB" )
      else :
         PhysWaveLog.info( "Read HEC PhysWave form SQLite file" )
         
      
   if ( ReadHECPhysWaveFromCOOL ):
      conddb.addFolder("",HECPhysWaveFolder+"<tag>"+HECPhysWaveTagSpec+"</tag>"+"<dbConnection>COOLOFL_LAR/COMP200</dbConnection>" + ChannelSelection)

   else:
      if 'InputHECPhysWavePoolFileName' in dir():
         PhysWaveLog.info( "Read HECPhysWave form POOL file" )
         PoolFileList += [ InputHECPhysWavePoolFileDir +"/"+ InputHECPhysWavePoolFileName ]   
      else:
         PhysWaveLog.info( "No PoolFileList found! Please list the POOL files containing HECPhysWave or read from COOL." )
         theApp.exit(-1)      

## CaliWave
if ( ReadCaliWaveFromCOOL ):
      CaliWaveTagSpec = LArCalibFolderTag(CaliWaveFolder,CaliWaveLArCalibFolderTag)
      conddb.addFolder("",CaliWaveFolder+"<tag>"+CaliWaveTagSpec+"</tag>"+"<dbConnection>"+InputDBConnectionCaliWave+"</dbConnection>" + ChannelSelection)
else:
   if 'InputCaliWavePoolFileName' in dir():
      PhysWaveLog.info( "Read CaliWave from POOL file" )
      PoolFileList += [ InputCaliWavePoolFileDir+"/"+InputCaliWavePoolFileName ]
   else:
      PhysWaveLog.info( "No PoolFileList found! Please list the POOL files containing CaliWave or read from COOL." )
      theApp.exit(-1)     
      

## PulseParams      
if ( ReadCaliPulseParamsFromCOOL ):
   PulseParamsFolder = InputCaliPulseParamsFolder
   PulseParamsTagSpec = LArCalibFolderTag(PulseParamsFolder,LArCaliPulseParamsTag)
   conddb.addFolder("",PulseParamsFolder+"<tag>"+PulseParamsTagSpec+"</tag>"+"<dbConnection>"+InputDBConnectionCaliPulseParams+"</dbConnection>" + ChannelSelection)
   
else:
   if 'InputCaliPulseParamsPoolFileName' in dir():
      PhysWaveLog.info( "Read PulseParams from POOL file" )
      PoolFileList += [ InputCaliPulseParamsPoolFileDir+"/"+InputCaliPulseParamsPoolFileName ]
   else:
      PhysWaveLog.info( "No PoolFileList found! Please list the POOL files containing PulseParams or read from COOL." )
      theApp.exit(-1)
         
if not (isHEC) :
   ## DetCellParams
   if (  ReadDetCellParamsFromCOOL ):
      DetCellParamsFolder = InputDetCellParamsFolder
      DetCellParamsTagSpec = LArCalibFolderTag(DetCellParamsFolder,LArCaliPulseParamsTag)
      conddb.addFolder("",DetCellParamsFolder+"<tag>"+DetCellParamsTagSpec+"</tag>"+"<dbConnection>"+InputDBConnectionDetCellParams+"</dbConnection>" + ChannelSelection)
   else:
      if 'InputDetCellParamsPoolFileName' in dir():
         PhysWaveLog.info( "Read DetCellParams from POOL file" )
         PoolFileList += [ InputDetCellParamsPoolFileDir+"/"+InputDetCellParamsPoolFileName ]
      else:
         PhysWaveLog.info( "No PoolFileList found! Please list the POOL files containing DetCellParams or read from COOL." )
         theApp.exit(-1)

## DTime
if ( ReadDTimeFromCOOL ):
   DTimeTagSpec = LArCalibFolderTag(LArCalib_Flags.LArDTimeFolder,DTimeLArCalibFolderTag)
   DTimeTagSpec = "LARElecCalibOflSCTdriftComputed-000"
   # here TODO - replace this again later
   conddb.addFolder("",LArCalib_Flags.LArDTimeFolder+"<tag>"+DTimeTagSpec+"</tag>"+"<dbConnection>"+InputDBConnectionDTime+"</dbConnection>")#,className='LArTdriftComplete')
else:
   if 'InputDTimePoolFileName' in dir():
      PhysWaveLog.info( "Read DTime from POOL file" )
      PoolFileList += [ InputDTimePoolFileDir+"/"+InputDTimePoolFileName ]
   else:
      PhysWaveLog.info( "No PoolFileList found! Please list the POOL files containing DTrif or read from COOL." )
      theApp.exit(-1)     

if ( len(PoolFileList)>0 ):

   from AthenaCommon.ConfigurableDb import getConfigurable
   svcMgr += getConfigurable( "ProxyProviderSvc" )()
   svcMgr.ProxyProviderSvc.ProviderNames += [ "CondProxyProvider" ]
   
   svcMgr += getConfigurable( "CondProxyProvider" )()
   svcMgr.CondProxyProvider.InputCollections += PoolFileList
   

###########################################################################
#
#       Predict physics waveforms from Cali/Master waveforms
#
###########################################################################

from LArCalibUtils.LArCalibUtilsConf import LArPhysWavePredictor
from LArBadChannelTool.LArBadChannelToolConf import LArBadChannelMasker
theMask=LArBadChannelMasker("BadChannelMask",
                            DoMasking=True,
                            ProblemsToMask=["deadCalib","deadReadout","deadPhys","almostDead","short"]
                            )
svcMgr.ToolSvc+=theMask

LArPhysWavePredictor = LArPhysWavePredictor( "LArPhysWavePredictor" )
LArPhysWavePredictor.MaskingTool              = theMask
LArPhysWavePredictor.TestMode	              = doTest
LArPhysWavePredictor.isSC                     = SuperCells
LArPhysWavePredictor.KeyCaliList              = KeyCaliList
LArPhysWavePredictor.UseCaliPulseParamsFromJO = UseCaliPulseParamsFromJO
LArPhysWavePredictor.UseDetCellParamsFromJO   = UseDetCellParamsFromJO
LArPhysWavePredictor.UseTdriftFromJO          = UseTdriftFromJO
LArPhysWavePredictor.Tdrift	              = TdriftVector
LArPhysWavePredictor.UseDoubleTriangle        = UseDoubleTriangle
LArPhysWavePredictor.Tdrift2	              = TdriftVector2
LArPhysWavePredictor.WeightTriangle2          = TdriftWeight2
LArPhysWavePredictor.UseTimeShiftFromJO       = UseTimeShiftFromJO
LArPhysWavePredictor.GroupingType             = GroupingType
LArPhysWavePredictor.NormalizeCali	      = True
if (isHEC) :
   LArPhysWavePredictor.NormalizeCali	      = False
LArPhysWavePredictor.KeyMphysMcali	      = LArMphysMcaliKey
LArPhysWavePredictor.DumpMphysMcali           = False # set to True to dump on a ASCII file
LArPhysWavePredictor.KeyPhys                  = InputKey
LArPhysWavePredictor.isHEC                    = isHEC
#
# Avoid using these settings, use LArPhysWaveShifter in a separate job
#
LArPhysWavePredictor.TimeShiftByHelper        = False
LArPhysWavePredictor.TimeShiftByIndex         = 0
LArPhysWavePredictor.TimeShiftByLayer         = False
LArPhysWavePredictor.Tshift	              = [ 0 , 0 , 0 , 0 ]
LArPhysWavePredictor.TimeShiftByFEB           = False
LArPhysWavePredictor.TimeShiftGuardRegion     = 0

#################### PHYSWAVE Prediction FOR EM #################### 
if not (isHEC) :
   from LArCalibUtils.LArCalibUtilsConf import LArPhysWaveTool
   LArPhysWaveTool=LArPhysWaveTool()
   LArPhysWaveTool.NormalizeCali     = False # this is taken care by LArPhysWavePredictor
   LArPhysWaveTool.TimeOriginShift   = False
   LArPhysWaveTool.SubtractBaseline  = False
   LArPhysWaveTool.InjPointCorrLayer = InjPointCorrLayer
   LArPhysWaveTool.InjPointUseTauR   = InjPointUseTauR 
   svcMgr.ToolSvc +=LArPhysWaveTool
   
#################### PHYSWAVE Prediction FOR HEC ####################
else :
   from LArCalibUtils.LArCalibUtilsConf import LArPhysWaveHECTool
   LArPhysWaveHECTool=LArPhysWaveHECTool()
   LArPhysWaveHECTool.NormalizeCali     = False  # this is taken care by LArPhysWavePredictor changed by FT to True was False
   LArPhysWaveHECTool.TimeOriginShift   = False
   LArPhysWaveHECTool.SubtractBaseline  = False
   svcMgr.ToolSvc +=LArPhysWaveHECTool
   
topSequence += LArPhysWavePredictor

###########################################################################
#
#       Shift physics waveforms will be done before OFC computation
#
###########################################################################
if (doPhysWaveShifter) :
   
   from LArCalibUtils.LArCalibUtilsConf import LArPhysWaveShifter
   LArPhysWaveShifter                        = LArPhysWaveShifter("LArPhysWaveShifter")

   LArPhysWaveShifter.KeyList                = [ InputKey ]
   LArPhysWaveShifter.KeyOutput              = OutputKey
   LArPhysWaveShifter.GroupingType           = GroupingType

   #
   # Minimum Tstart per FEB computation
   #
   LArPhysWaveShifter.ComputeTimeShiftByFEB  = True
   LArPhysWaveShifter.TimeShiftByFEBMode     = ShiftMode
   LArPhysWaveShifter.TimeShiftByFEBDump     = True
   LArPhysWaveShifter.TimeShiftByFEBDumpFile = "TimeShiftFEB.py"
   
   #
   # Possible time shifts
   #
   LArPhysWaveShifter.TimeShiftByHelper      = False
   LArPhysWaveShifter.TimeShiftByIndex       = 0
   LArPhysWaveShifter.TimeShiftByLayer       = False
   LArPhysWaveShifter.TshiftLayer            = [ 0 , 0 , 0 , 0 ]
   LArPhysWaveShifter.TimeShiftFromPeak      = False
   LArPhysWaveShifter.NindexFromPeak         = 0 # if 0 will use Ndelays*Nsampling
   LArPhysWaveShifter.Ndelays                = 24
   LArPhysWaveShifter.Nsamplings             = Nsamplings 
   LArPhysWaveShifter.TimeShiftByFEB         = True
   LArPhysWaveShifter.TimeShiftGuardRegion   = TimeShiftGuardRegion
   
   topSequence += LArPhysWaveShifter

##########################################################################

if ( WriteNtuple ) :

   if ( WritePhysWave2NTuple ) :
      from LArCalibTools.LArCalibToolsConf import LArPhysWaves2Ntuple
      LArPhysWaves2Ntuple              = LArPhysWaves2Ntuple("LArPhysWaves2Ntuple")
      LArPhysWaves2Ntuple.NtupleName   = "PHYSWAVE" 
      LArPhysWaves2Ntuple.AddFEBTempInfo   = False  
      LArPhysWaves2Ntuple.KeyList      = [ OutputKey  ]
      LArPhysWaves2Ntuple.isSC = SuperCells
      LArPhysWaves2Ntuple.OutputLevel = DEBUG  # here here added this
      topSequence += LArPhysWaves2Ntuple
      
   if ( WriteCaliWave2NTuple ) :
      from LArCalibTools.LArCalibToolsConf import LArCaliWaves2Ntuple
      LArCaliWaves2Ntuple = LArCaliWaves2Ntuple( "LArCaliWaves2Ntuple" )	
      LArCaliWaves2Ntuple.NtupleName   = "CALIWAVE"
      LArCaliWaves2Ntuple.AddFEBTempInfo   = False
      LArCaliWaves2Ntuple.KeyList      = KeyCaliList
      LArCaliWaves2Ntuple.isSC = SuperCells
      
      topSequence += LArCaliWaves2Ntuple
      
   if ( WriteMphyMcal2NTuple ) :
      from LArCalibTools.LArCalibToolsConf import LArMphysOverMcal2Ntuple
      LArMphysOverMcal2Ntuple                = LArMphysOverMcal2Ntuple( "LArMphysOverMcal2Ntuple" )
      LArMphysOverMcal2Ntuple.ContainerKey   = "LArMphysOverMcal"
      LArMphysOverMcal2Ntuple.AddFEBTempInfo   = False
      LArMphysOverMcal2Ntuple.isSC = SuperCells
   
      topSequence += LArMphysOverMcal2Ntuple
   
   theApp.HistogramPersistency = "ROOT"
   from GaudiSvc.GaudiSvcConf import NTupleSvc
   if os.path.exists(OutputPhysWaveRootFileDir+"/"+OutputPhysWaveRootFileName): 
      os.remove(OutputPhysWaveRootFileDir+"/"+OutputPhysWaveRootFileName)  
   svcMgr += NTupleSvc()
   svcMgr.NTupleSvc.Output = [ "FILE1 DATAFILE='"+OutputPhysWaveRootFileDir+"/"+OutputPhysWaveRootFileName+"' OPT='NEW'" ]
   
if (  WritePoolFile ) :
   
   from RegistrationServices.OutputConditionsAlg import OutputConditionsAlg
   
   # writre PredPhysWave
   if os.path.exists(OutputPhysWavePoolFileDir+"/"+OutputPhysWavePoolFileName): 
      os.remove(OutputPhysWavePoolFileDir+"/"+OutputPhysWavePoolFileName)
   OutputConditionsAlgPhysWave = OutputConditionsAlg("OutputConditionsAlgPhysWave",OutputPhysWavePoolFileDir+"/"+OutputPhysWavePoolFileName,
                                                     [OutputObjectSpecPhysWave],[OutputObjectSpecTagPhysWave],WriteIOV)
   OutputConditionsAlgPhysWave.Run1     = IOVBegin
   if IOVEnd>0:
      OutputConditionsAlgPhysWave.Run2  = IOVEnd
		
   # writre MphysOverMcal
   if os.path.exists(OutputMphysOverMcalPoolFileDir+"/"+OutputMphysOverMcalPoolFileName): 
      os.remove(OutputMphysOverMcalPoolFileDir+"/"+OutputMphysOverMcalPoolFileName)
   OutputConditionsAlgMphysOverMcal = OutputConditionsAlg("OutputConditionsAlgMphysOverMcal",OutputMphysOverMcalPoolFileDir+"/"+OutputMphysOverMcalPoolFileName,
                                                     [OutputObjectSpecMphysOverMcal],[OutputObjectSpecTagMphysOverMcal],WriteIOV)
   OutputConditionsAlgMphysOverMcal.Run1     = IOVBegin
   if IOVEnd>0:
      OutputConditionsAlgMphysOverMcal.Run2  = IOVEnd

   svcMgr.IOVDbSvc.dbConnection  = OutputDB

   from RegistrationServices.RegistrationServicesConf import IOVRegistrationSvc
   svcMgr += IOVRegistrationSvc()
   svcMgr.IOVRegistrationSvc.OutputLevel = VERBOSE
   svcMgr.IOVRegistrationSvc.RecreateFolders = False

###########################################################################
#                Use EventSelector to select IOV                          #
###########################################################################

theByteStreamInputSvc=svcMgr.ByteStreamInputSvc
if not 'FullFileName' in dir():
   RampLog.info( "No FullFileName! Please give a FullFileName list." )
   theApp.exit(-1)

else :   
   svcMgr.EventSelector.Input=FullFileName
   
svcMgr.EventSelector.MaxBadEvents = 0

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
