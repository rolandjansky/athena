# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from future import standard_library
standard_library.install_aliases()
import subprocess

###########################################################################
#
# <Marco.Delmastro@cern.ch>
#
# jobOptions to compute OFC phys in all gains for two different DeltaPhase
# settings, ad store the objects in parallel folders.
#
# Last update: 04/12/2009 <Marco.Delmastro@cern.ch>
#
###########################################################################

if not "SuperCells" in dir():
   SuperCells=False
   
if not SuperCells: include("LArCalibProcessing/LArCalib_Flags.py")
if SuperCells:     include("LArCalibProcessing/LArCalib_FlagsSC.py")

###########################################################################
#             PhysWaveShifter properties                                  #
###########################################################################

if not "UseDelta" in dir():
   UseDelta=0

if not "NColl" in dir():
   NColl=0

if not 'doPhysWaveShifter' in dir():
   doPhysWaveShifter = True 

if not 'InputKey' in dir():   
   if (doPhysWaveShifter) :
      if not SuperCells: InputKey = "LArPhysWaveUnShifted"
      if SuperCells:     InputKey = "LArPhysWave"
   else :
      InputKey = "LArPhysWave"  # no shift applied to the PhysWave   

if not 'isFCAL' in dir() :
   isFCAL = False

if not 'InputKey' in dir():
   InputKey = "LArPhysWaveUnShifted"

if not 'ShiftMode' in dir():
   ShiftMode = 3  # 1 = minimum Tstart from helper in FEB
                  # 2 = minumum Tpeak-NindexFromPeak in FEB
                  # 3 = average Tpeak-NindexFromPeak in FEB
                  
if not 'Nsamplings' in dir():
   Nsamplings = 2

if not 'TimeShiftGuardRegion' in dir():
   TimeShiftGuardRegion = 12

if not 'UsePhysCaliTdiff' in dir():
   UsePhysCaliTdiff = True

if not 'PhysCaliTdiffKey' in dir():
   PhysCaliTdiffKey = "LArPhysCaliTdiff"

###########################################################################
#               Input selection (Autocorr, PhysWave)
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

## AutoCorr      

if not 'ReadAutoCorrFromCOOL' in dir():
   ReadAutoCorrFromCOOL = True

if not 'InputAutoCorrPoolDir' in dir():
   InputAutoCorrPoolDir = subprocess.getoutput("pwd")

if not 'InputAutoCorrPoolFileName' in dir():
   InputAutoCorrPoolFileName = "LArAutoCorr.pool.root"

## PhysWave

if not 'ReadPhysWaveFromCOOL' in dir():
   ReadPhysWaveFromCOOL = True

if not 'InputPhysWavePoolDir' in dir():
   InputPhysWavePoolDir = subprocess.getoutput("pwd")

if not 'InputPhysWavePoolFileName' in dir():
   InputPhysWavePoolFileName = "LArPhysWave.pool.root"  

## PhysCaliTdiff

if not 'ReadPhysCaliTdiffFromCOOL' in dir():
   ReadPhysCaliTdiffFromCOOL = True

if not 'InputPhysCaliTdiffPoolDir' in dir():
   InputPhysCaliTdiffPoolDir = subprocess.getoutput("pwd")

if not 'InputPhysCaliTdiffPoolFileName' in dir():
   InputPhysCaliTdiffPoolFileName = "LArPhysCaliTdiff.pool.root"

if not 'ChannelSelection' in dir():
   # Read all
   ChannelSelection = " "
   ## Example to read only cool for Barrel C : Strips->Back
   #ChannelSelection = "<channelSelection>0,35:66</channelSelection>" 

from string import *
def DBConnectionFile(sqlitefile):
   return "sqlite://;schema="+sqlitefile+";dbname=CONDBR2"

###########################################################################
# OFC properties (2 instances)
###########################################################################

if not 'ContainerKey' in dir():
   ContainerKey = "LArPhysWave"

if not 'Normalize' in dir():
   Normalize = True

#
# First instance
#

if not 'TimeShift' in dir() :
   TimeShift = False

if not 'TimeShiftByIndex' in dir() :
   TimeShiftByIndex = -1

if not 'Nsamples' in dir():
   Nsamples = 5

if Nsamples == 4:
   if not 'ReadDSPCOnfig' in dir():
      ReadDSPConfig=True
   if not 'DSPConfigFolder' in dir():
      DSPConfigFolder="/LAR/Configuration/DSPConfiguration"

if not 'Nphases' in dir():
   Nphases = 8

if not 'Dphases' in dir():
   Dphases = 3
   
if not 'Ndelays' in dir():
   Ndelays = 24
   
if not 'OFCKey' in dir():
   OFCKey = "LArOFC"

if not 'ShapeKey' in dir():
   ShapeKey = "LArShape"


if not 'ShiftKey' in dir():
   ShiftKey="LArPhysWaveShift"

if not 'FillShape' in dir():
   FillShape = True

if not 'OFCTreename' in dir():
   OFCTreename = "OFC"

if not 'ShapeTreename' in dir():
   ShapeTreename = "Shape"

#
# Second instance
#

if not 'TimeShift2' in dir() :
   TimeShift2 = False

if not 'TimeShiftByIndex2' in dir() :
   TimeShiftByIndex2 = -1

if not 'Nsamples2' in dir():
   Nsamples2 = 5

if Nsamples2 == 4:
   if not 'ReadDSPCOnfig2' in dir():
      ReadDSPConfig2=True
   if not 'DSPConfigFolder2' in dir():
      DSPConfigFolder2="/LAR/Configuration/DSPConfiguration"

if not 'Nphases2' in dir():
   Nphases2 = 24

if not 'Dphases2' in dir():
   Dphases2 = 1
   
if not 'Ndelays2' in dir():
   Ndelays2 = 24
   
if not 'OFCKey2' in dir():
   OFCKey2 = "LArOFC_2"

if not 'ShapeKey2' in dir():
   ShapeKey2 = "LArShape_2"

if not 'FillShape2' in dir():
   FillShape2 = True

if not 'OFCTreename2' in dir():
   OFCTreename2 = "OFC2"

if not 'ShapeTreename2' in dir():
   ShapeTreename2 = "Shape2"

###########################################################################
# OFC output
###########################################################################

if not 'GroupingType' in dir():
   if not SuperCells: GroupingType = "ExtendedSubDetector"
   if SuperCells:     GroupingType = "SuperCells"
   
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

if not 'OutputOFCRootFileDir' in dir():
   OutputOFCRootFileDir = subprocess.getoutput("pwd")
   
if not 'OutputOFCPoolFileDir' in dir():
   OutputOFCPoolFileDir = subprocess.getoutput("pwd")

OFCFileTag = str(RunNumber)+"_"+Partition.replace("*","")
OFCFileTag += "_"+str(Nsamples)+"samples"

if not 'OutputOFCRootFileName' in dir():
   OutputOFCRootFileName = "LArOFCPhys_"+OFCFileTag + ".root"
   
if not 'OutputPoolFileName' in dir():
   OutputPoolFileName = "LArOFCShapePhys_"+OFCFileTag + ".pool.root"


#input folders:
if "AutoCorrFolder" not in dir():
   AutoCorrFolder  = LArCalib_Flags.LArAutoCorrFolder
if "PhysAutoCorrFolder" not in dir():
   PhysAutoCorrFolder  = LArCalib_Flags.LArPhysAutoCorrFolder
if "PhysCaliTdiffFolder" not in dir():
   PhysCaliTdiffFolder  = LArCalib_Flags.LArPhysCaliTdiffFolder

if isFCAL:
   PhysWaveFolderFCAL="/LAR/ElecCalibOfl/PhysWaves/FCALFromTB"
else:
   PhysWaveFolder  = LArCalib_Flags.LArPhysWaveFolder


#Shift (can be output or input)
if "OFCBinFolder" not in dir():
   if not SuperCells: OFCBinFolder="/LAR/ElecCalibOfl/OFCBin/PhysWaveShifts"
   if SuperCells:     OFCBinFolder="/LAR/ElecCalibOflSC/OFCBin/PhysWaveShifts"


#output Folders:
if "FolderOFC" not in dir():
   if not SuperCells: FolderOFC   = "/LAR/ElecCalibOfl/OFC/PhysWave/RTM/5samples3bins17phases"
   if SuperCells:     FolderOFC   = "/LAR/ElecCalibOflSC/OFC/PhysWave/RTM/5samples3bins17phases"

if "FolderShape" not in dir():
   if not SuperCells: FolderShape = "/LAR/ElecCalibOfl/Shape/RTM/5samples3bins17phases"
   if SuperCells:     FolderShape = "/LAR/ElecCalibOflSC/Shape/RTM/5samples3bins17phases"

if "FolderOFC2" not in dir():
   if not SuperCells: FolderOFC2   = "/LAR/ElecCalibOfl/OFC/PhysWave/RTM/5samples"
   if SuperCells:     FolderOFC2   = "/LAR/ElecCalibOflSC/OFC/PhysWave/RTM/5samples"

if "FolderShape2" not in dir():
   if not SuperCells: FolderShape2 = "/LAR/ElecCalibOfl/Shape/RTM/5samples"
   if SuperCells:     FolderShape2 = "/LAR/ElecCalibOflSC/Shape/RTM/5samples"


rs=FolderTagResover()
if not 'AutoCorrLArCalibFolderTag' in dir():
   AutoCorrLArCalibFolderTag=rs.getFolderTagSuffix(AutoCorrFolder)


if not 'PhysAutoCorrLArCalibFolderTag' in dir():
   PhysAutoCorrLArCalibFolderTag="_mu_"+str(NColl)+rs.getFolderTagSuffix(PhysAutoCorrFolder)

if not 'PhysWaveLArCalibFolderTag' in dir():
   if isFCAL:
      PhysWaveLArCalibFolderTag=rs.getFolderTagSuffix(PhysWaveFolderFCAL)
   else:
      PhysWaveLArCalibFolderTag=rs.getFolderTagSuffix(PhysWaveFolder)

if not 'PhysCaliTdiffLArCalibFolderTag' in dir():
   PhysCaliTdiffLArCalibFolderTag=rs.getFolderTagSuffix(PhysCaliTdiffFolder)

if not 'OFCBinFolderTag' in dir():
   OFCBinFolderTag=rs.getFolderTagSuffix(OFCBinFolder)


#output Folders:
if "FolderTagOFC" not in dir():
   FolderTagOFC   = rs.getFolderTagSuffix(FolderOFC)
if "FolderTagOFCmu" not in dir():
   FolderTagOFCmu   = "-mu-"+str(NColl)+rs.getFolderTagSuffix(FolderOFC)
if "FolderTagShape" not in dir():
   FolderTagShape = rs.getFolderTagSuffix(FolderShape)

if "FolderTagOFC2" not in dir():
      FolderTagOFC2   = rs.getFolderTagSuffix(FolderOFC2)
if "FolderTagOFC2mu" not in dir():
      FolderTagOFC2mu  = "-mu-"+str(NColl)+rs.getFolderTagSuffix(FolderOFC)

if "FolderTagShape2" not in dir():
   FolderTagShape2 = rs.getFolderTagSuffix(FolderShape2)


del rs #close Database

if not 'OutputDB' in dir():
   OutputDB = LArCalib_Flags.OutputDB

if 'OutputSQLiteFile' in dir():
   OutputDB = DBConnectionFile(OutputSQLiteFile)


OutputObjectSpecOFC    = "LArOFCComplete#"+OFCKey+"#"+ FolderOFC
OutputObjectSpecTagOFC = LArCalibFolderTag(FolderOFC,FolderTagOFC)
						
OutputObjectSpecOFCmu    = "LArOFCComplete#"+OFCKey+"_mu"+"#"+ FolderOFC
OutputObjectSpecTagOFCmu = LArCalibFolderTag(FolderOFC,FolderTagOFCmu)
						
OutputObjectSpecShape = "LArShapeComplete#"+ShapeKey+"#"+ FolderShape
OutputObjectSpecTagShape  = LArCalibFolderTag(FolderShape,FolderTagShape)

OutputObjectSpecOFC2    = "LArOFCComplete#"+OFCKey2+"#"+ FolderOFC2
OutputObjectSpecTagOFC2 = LArCalibFolderTag(FolderOFC2,FolderTagOFC2)
						
OutputObjectSpecOFC2mu   = "LArOFCComplete#"+OFCKey2+"_mu"+"#"+ FolderOFC2
OutputObjectSpecTagOFC2mu = LArCalibFolderTag(FolderOFC2,FolderTagOFC2mu)
						
OutputObjectSpecShape2 = "LArShapeComplete#"+ShapeKey2+"#"+ FolderShape2
OutputObjectSpecTagShape2  = LArCalibFolderTag(FolderShape2,FolderTagShape2)

OutputObjectSpecOFCBin="LArOFCBinComplete#"+ShiftKey+"#"+OFCBinFolder
OutputObjectSpecTagOFCBin = LArCalibFolderTag(OFCBinFolder,OFCBinFolderTag)


if not 'DBConnectionCOOL' in dir():
   DBConnectionCOOL = "oracle://ATLAS_COOLPROD;schema=ATLAS_COOLOFL_LAR;dbname=CONDBR2;"

if ( ReadAutoCorrFromCOOL ):      
   if 'InputAutoCorrSQLiteFile' in dir():
      InputDBConnectionAutoCorr = DBConnectionFile(InputAutoCorrSQLiteFile)
   else:
      InputDBConnectionAutoCorr = DBConnectionCOOL
   if 'InputPhysAutoCorrSQLiteFile' in dir():
      InputDBConnectionPhysAutoCorr = DBConnectionFile(InputAutoCorrSQLiteFile)
   else:
      InputDBConnectionPhysAutoCorr = InputDBConnectionAutoCorr

if ( ReadPhysWaveFromCOOL ):      
   if 'InputPhysWaveSQLiteFile' in dir():
      InputDBConnectionPhysWave = DBConnectionFile(InputPhysWaveSQLiteFile)
   else:
      InputDBConnectionPhysWave = DBConnectionCOOL

if ( ReadPhysCaliTdiffFromCOOL ):
   if 'InputPhysCaliTdiffSQLiteFile' in dir():
      InputDBConnectionPhysCaliTdiff = DBConnectionFile(InputPhysCaliTdiffSQLiteFile)
   else:
      InputDBConnectionPhysCaliTdiff = "COOLOFL_LAR/COMP200"

## Bad Channel   
   
if not 'ReadBadChannelFromCOOL' in dir():
   ReadBadChannelFromCOOL = True   

if ( ReadBadChannelFromCOOL ):      
   if 'InputBadChannelSQLiteFile' in dir():
      InputDBConnectionBadChannel = DBConnectionFile(InputBadChannelSQLiteFile)
   else:
      InputDBConnectionBadChannel = "COOLOFL_LAR/CONDBR2"        
      
###########################################################################
# Print summary
###########################################################################

OFCLog = logging.getLogger( "OFCLog" )
OFCLog.info( " ======================================================== " )
OFCLog.info( " ***                 LAr OFC summary                  *** " )
OFCLog.info( " ======================================================== " )
OFCLog.info( " RunNumber                          = "+str(RunNumber) )
if ( ReadAutoCorrFromCOOL ):
   OFCLog.info( " InputDBConnectionAutoCorr          = "+InputDBConnectionAutoCorr )
   OFCLog.info( " InputDBConnectionPhysAutoCorr      = "+InputDBConnectionPhysAutoCorr )
   OFCLog.info( " AutoCorrLArCalibFolderTag             = "+AutoCorrLArCalibFolderTag)
else :
   OFCLog.info( " InputAutoCorrPoolFileName          = "+InputAutoCorrPoolDir+"/"+InputAutoCorrPoolFileName )
   OFCLog.info( " InputPhysAutoCorrPoolFileName          = "+InputPhysAutoCorrPoolDir+"/"+InputPhysAutoCorrPoolFileName )
   

if ( ReadPhysWaveFromCOOL ):
   OFCLog.info( " InputDBConnectionPhysWave          = "+InputDBConnectionPhysWave )
   OFCLog.info( " PhysWaveLArCalibFolderTag          =" +PhysWaveLArCalibFolderTag )
else :
   OFCLog.info( " InputPhysWavePoolFileName          = "+InputPhysWavePoolDir+"/"+InputPhysWavePoolFileName )
   
if ( (ReadAutoCorrFromCOOL or ReadPhysWaveFromCOOL) and ChannelSelection != " " ):
   OFCLog.info( " ChannelSelection                   = "+ChannelSelection )
   
if ( ReadPhysCaliTdiffFromCOOL ):
   OFCLog.info( " InputDBConnectionPhysCaliTdiff     = "+InputDBConnectionPhysCaliTdiff )
   OFCLog.info( " PhysCaliTdiffLArCalibFolderTag     = "+PhysCaliTdiffLArCalibFolderTag )
else :
   OFCLog.info( " InputPhysCaliTdiffPoolFileName     = "+InputPhysCaliTdiffPoolDir+"/"+InputPhysCaliTdiffPoolFileName )                                                      
OFCLog.info( " OutputOFCRootFullFileName          = "+OutputOFCRootFileDir+"/"+OutputOFCRootFileName )
OFCLog.info( " OutputPoolFullFileName             = "+OutputOFCPoolFileDir+"/"+OutputPoolFileName )
OFCLog.info( " OutputObjectSpecOFC                = "+str(OutputObjectSpecOFC) )
OFCLog.info( " OutputObjectSpecTagOFC             = "+str(OutputObjectSpecTagOFC) )
OFCLog.info( " OutputObjectSpecOFCmu              = "+str(OutputObjectSpecOFCmu) )
OFCLog.info( " OutputObjectSpecTagOFCmu           = "+str(OutputObjectSpecTagOFCmu) )
OFCLog.info( " OutputObjectSpecShape              = "+str(OutputObjectSpecShape) )
OFCLog.info( " OutputObjectSpecTagShape           = "+str(OutputObjectSpecTagShape) )
OFCLog.info( " OutputObjectSpecOFC2               = "+str(OutputObjectSpecOFC2) )
OFCLog.info( " OutputObjectSpecTagOFC2            = "+str(OutputObjectSpecTagOFC2) )
OFCLog.info( " OutputObjectSpecOFC2mu             = "+str(OutputObjectSpecOFC2mu) )
OFCLog.info( " OutputObjectSpecTagOFC2mu          = "+str(OutputObjectSpecTagOFC2mu) )
OFCLog.info( " OutputObjectSpecShape2             = "+str(OutputObjectSpecShape2) )
OFCLog.info( " OutputObjectSpecTagShape2          = "+str(OutputObjectSpecTagShape2) )
OFCLog.info( " OutputObjectSpecOFCBin             = "+str(OutputObjectSpecOFCBin) )
OFCLog.info( " OutputObjectSpecTagOFCBin          = "+str(OutputObjectSpecTagOFCBin) )
OFCLog.info( " IOVBegin                           = "+str(IOVBegin) )
OFCLog.info( " IOVEnd                             = "+str(IOVEnd) )
OFCLog.info( " LArCalibOutputDB                   = "+OutputDB )
OFCLog.info( " ======================================================== " )

###########################################################################
#
# Global settings
#
###########################################################################

include("AthenaCommon/Atlas_Gen.UnixStandardJob.py")

#
# Provides ByteStreamInputSvc name of the data file to process in the offline context
#

## get a handle to the default top-level algorithm sequence
from AthenaCommon.AlgSequence import AlgSequence 
topSequence = AlgSequence()

## get a handle to the ApplicationManager, to the ServiceManager and to the ToolSvc
from AthenaCommon.AppMgr import (theApp, ServiceMgr as svcMgr,ToolSvc)

include("LArCalibProcessing/LArCalib_MinimalSetup.py")
#Get cabling
if not SuperCells:
  from LArCabling.LArCablingAccess import LArCalibIdMapping,LArOnOffIdMapping
  LArCalibIdMapping()
  LArOnOffIdMapping() 
else:   
  from LArCabling.LArCablingAccess import LArOnOffIdMappingSC,LArCalibIdMappingSC
  LArOnOffIdMappingSC()
  LArCalibIdMappingSC() 

###########################################################################
#                                                                         #
#    Input conditions data (AutoCorr, CaliWave) for COOL and/or POOL      #
#                                                                         #
###########################################################################

include("AthenaPoolCnvSvc/AthenaPool_jobOptions.py")
include("LArCondAthenaPool/LArCondAthenaPool_joboptions.py")

from IOVDbSvc.CondDB import conddb
PoolFileList     = []

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

if ( ReadAutoCorrFromCOOL or ReadPhysWaveFromCOOL ):
   if not ('InputAutoCorrSQLiteFile')  in dir():
      OFCLog.info( "Read AutoCorr from Oracle" )
   else :
      OFCLog.info( "Read AutoCorr from SQLite file" )
      
   if not ('InputPhysWaveSQLiteFile')  in dir():
      OFCLog.info( "Read PhysWave from Oracle DB" )
   else :
      OFCLog.info( "Read PhysWave from SQLite file" )
      
if ( ReadAutoCorrFromCOOL ):
   PhysAutoCorrTagSpec= LArCalibFolderTag(PhysAutoCorrFolder,PhysAutoCorrLArCalibFolderTag)
   AutoCorrTagSpec= LArCalibFolderTag(AutoCorrFolder,AutoCorrLArCalibFolderTag)
   conddb.addFolder("",AutoCorrFolder+"<tag>"+AutoCorrTagSpec+"</tag>"+"<dbConnection>"+InputDBConnectionAutoCorr+"</dbConnection>"+ ChannelSelection)
   conddb.addFolder("",PhysAutoCorrFolder+"<tag>"+PhysAutoCorrTagSpec+"</tag><key>LArPhysAutoCorr</key>"+"<dbConnection>"+InputDBConnectionAutoCorr+"</dbConnection>"+ ChannelSelection)
else:
   if 'InputAutoCorrPoolFileName' in dir():
      OFCLog.info( "Read AutoCorr from POOL file")
      PoolFileList += [ InputAutoCorrPoolDir+"/"+InputAutoCorrPoolFileName ]
   else:
      OFCLog.info( "No PoolFileList found! Please list the POOL files containing AutoCorrelation Matrix or read from COOL." )
      theApp.exit(-1)      
   if 'InputPhysAutoCorrPoolFileName' in dir():
      OFCLog.info( "Read PhysAutoCorr from POOL file")
      PoolFileList += [ InputPhysAutoCorrPoolDir+"/"+InputPhysAutoCorrPoolFileName ]
   else:
      OFCLog.info( "No PoolFileList found! Please list the POOL files containing PhysAutoCorrelation Matrix or read from COOL." )
      theApp.exit(-1)   
      
if ( ReadPhysWaveFromCOOL ):
   if (doPhysWaveShifter or isFCAL) :
      if isFCAL:
         PhysWaveTagSpec = LArCalibFolderTag(PhysWaveFolderFCAL,PhysWaveLArCalibFolderTag)
      else:
         PhysWaveTagSpec = LArCalibFolderTag(PhysWaveFolder,PhysWaveLArCalibFolderTag)
      if isFCAL:   
         conddb.addFolder("",PhysWaveFolderFCAL+"<key>"+InputKey+"</key><tag>"+PhysWaveTagSpec+"</tag>"+"<dbConnection>"+InputDBConnectionPhysWave+"</dbConnection>" + ChannelSelection)
      else:   
         conddb.addFolder("",PhysWaveFolder+"<key>"+InputKey+"</key><tag>"+PhysWaveTagSpec+"</tag>"+"<dbConnection>"+InputDBConnectionPhysWave+"</dbConnection>" + ChannelSelection)
   else :
      PhysWaveTagSpec = LArCalibFolderTag(PhysWaveFolder,PhysWaveLArCalibFolderTag)
      conddb.addFolder("",PhysWaveFolder+"<tag>"+PhysWaveTagSpec+"</tag>"+"<dbConnection>"+InputDBConnectionPhysWave+"</dbConnection>" + ChannelSelection) 
else:
   if 'InputPhysWavePoolFileName' in dir():
      OFCLog.info( "Read PhysWave from POOL file" )
      PoolFileList += [ InputPhysWavePoolDir+"/"+InputPhysWavePoolFileName ]
   else:
      OFCLog.info( "No PoolFileList found! Please list the POOL files containing PhysWave or read from COOL." )
      theApp.exit(-1)

if (UsePhysCaliTdiff):
   if ( ReadPhysCaliTdiffFromCOOL ):
         PhysCaliTdiffTagSpec = LArCalibFolderTag(PhysCaliTdiffFolder,PhysCaliTdiffLArCalibFolderTag)
         conddb.addFolder("",PhysCaliTdiffFolder+"<key>"+PhysCaliTdiffKey+"</key><tag>"+PhysCaliTdiffTagSpec+"</tag>"+"<dbConnection>"+InputDBConnectionPhysCaliTdiff+"</dbConnection>" + ChannelSelection)
   else: 
      if 'InputPhysCaliTdiffPoolFileName' in dir():
         OFCLog.info( "Read PhysCaliTdiff from POOL file" )
         PoolFileList += [ InputPhysCaliTdiffPoolDir+"/"+InputPhysCaliTdiffPoolFileName ]
      else:
         OFCLog.info( "No PoolFileList found! Please list the POOL files containing PhysCaliTdiff or read from COOL." )
         theApp.exit(-1)

if ( len(PoolFileList)>0 ):

   from AthenaCommon.ConfigurableDb import getConfigurable
   svcMgr += getConfigurable( "ProxyProviderSvc" )()
   svcMgr.ProxyProviderSvc.ProviderNames += [ "CondProxyProvider" ]

   svcMgr += getConfigurable( "CondProxyProvider" )()
   svcMgr.CondProxyProvider.InputCollections += PoolFileList

if (doPhysWaveShifter) :

   ###########################################################################
   #                                                                         #
   #                 Shift physics waveforms only for FCAL                   #
   #                                                                         # 
   ###########################################################################
   from LArCalibUtils.LArCalibUtilsConf import LArPhysWaveShifter
   LArPhysWaveShifter                        = LArPhysWaveShifter("LArPhysWaveShifter")
   
   LArPhysWaveShifter.KeyList                = [ InputKey ]
   LArPhysWaveShifter.KeyOutput              = ContainerKey
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
   LArPhysWaveShifter.TimeShiftFromPeak      = False
   LArPhysWaveShifter.NindexFromPeak         = 0 # if 0 will use Ndelays*Nsampling
   LArPhysWaveShifter.Ndelays                = 24
   LArPhysWaveShifter.Nsamplings             = Nsamplings 
   LArPhysWaveShifter.TimeShiftByFEB         = True
   LArPhysWaveShifter.TimeShiftGuardRegion   = TimeShiftGuardRegion
   LArPhysWaveShifter.UsePhysCaliTdiff       = UsePhysCaliTdiff
   LArPhysWaveShifter.CellByCellShiftsKey    = "LArPhysCaliTdiff"
   LArPhysWaveShifter.OutputShiftsKey        = ShiftKey
   LArPhysWaveShifter.isSC                   = SuperCells
   topSequence += LArPhysWaveShifter   

else:
   #Load Shifts from DB:
   if not SuperCells: # FIXME when shift for SC available
      conddb.addFolder("LAR_OFL",OFCBinFolder+"<tag>"+OutputObjectSpecTagOFCBin+"</tag><key>"+ShiftKey+"</key>");


###########################################################################
#                      OFC computation
###########################################################################

from LArCalibUtils.LArCalibUtilsConf import LArAutoCorrDecoderTool
theLArAutoCorrDecoderTool = LArAutoCorrDecoderTool()
theLArAutoCorrDecoderTool.UseAlwaysHighGain=True
theLArAutoCorrDecoderTool.isSC = SuperCells

if NColl > 0:
   theLArPhysAutoCorrDecoderTool = LArAutoCorrDecoderTool("LArPhysAutoCorrDecoderTool")
   theLArPhysAutoCorrDecoderTool.DecodeMode=1
   theLArPhysAutoCorrDecoderTool.UseAlwaysHighGain=True
   theLArPhysAutoCorrDecoderTool.isSC = SuperCells
   theLArPhysAutoCorrDecoderTool.KeyAutoCorr="LArPhysAutoCorr"

from LArCalibUtils.LArCalibUtilsConf import LArOFCAlg

LArPhysOFCAlg = LArOFCAlg("LArPhysOFCAlg")

LArPhysOFCAlg.ReadCaliWave = False
LArPhysOFCAlg.KeyList      = [ InputKey ]
LArPhysOFCAlg.Normalize    = Normalize
LArPhysOFCAlg.Verify       = True
LArPhysOFCAlg.GroupingType = GroupingType

LArPhysOFCAlg.Nphase    = Nphases
LArPhysOFCAlg.Dphase    = Dphases
LArPhysOFCAlg.Ndelay    = Ndelays
LArPhysOFCAlg.Nsample   = Nsamples
LArPhysOFCAlg.FillShape = FillShape
LArPhysOFCAlg.TimeShift = TimeShift
LArPhysOFCAlg.TimeShiftByIndex = TimeShiftByIndex
LArPhysOFCAlg.AddTimeOffset = -1.0*TimeShiftGuardRegion
if not SuperCells: # FIXME when shift for SC available
   LArPhysOFCAlg.LArPhysWaveBinKey = ShiftKey

LArPhysOFCAlg.UseDelta = 0 #Only for high-mu OFCs
LArPhysOFCAlg.KeyOFC   = OFCKey
LArPhysOFCAlg.KeyShape = ShapeKey
LArPhysOFCAlg.DecoderTool = theLArAutoCorrDecoderTool
LArPhysOFCAlg.isSC = SuperCells
if Nsamples==4:
   LArPhysOFCAlg.ReadDSPConfig   = ReadDSPConfig
   LArPhysOFCAlg.DSPConfigFolder = DSPConfigFolder


topSequence+=LArPhysOFCAlg

LArPhysOFCAlg2 = LArOFCAlg("LArPhysOFCAlg2")

LArPhysOFCAlg2.ReadCaliWave = False
LArPhysOFCAlg2.KeyList      = [ InputKey ]
LArPhysOFCAlg2.Normalize    = Normalize
LArPhysOFCAlg2.Verify       = True
LArPhysOFCAlg2.GroupingType = GroupingType

LArPhysOFCAlg2.Nphase    = Nphases2
LArPhysOFCAlg2.Dphase    = Dphases2
LArPhysOFCAlg2.Ndelay    = Ndelays2
LArPhysOFCAlg2.Nsample   = Nsamples2
LArPhysOFCAlg2.FillShape = FillShape2
LArPhysOFCAlg2.TimeShift = TimeShift2
LArPhysOFCAlg2.TimeShiftByIndex = TimeShiftByIndex2
LArPhysOFCAlg2.AddTimeOffset = -1.0*TimeShiftGuardRegion
if not SuperCells: # FIXME when shift for SC available
   LArPhysOFCAlg2.LArPhysWaveBinKey = ShiftKey
LArPhysOFCAlg2.UseDelta = 0 #Only for high-mu OFCs

LArPhysOFCAlg2.KeyOFC   = OFCKey2
LArPhysOFCAlg2.KeyShape = ShapeKey2
LArPhysOFCAlg2.DecoderTool = theLArAutoCorrDecoderTool
LArPhysOFCAlg2.isSC = SuperCells

if Nsamples2==4:
   LArPhysOFCAlg2.ReadDSPConfig   = ReadDSPConfig2
   LArPhysOFCAlg2.DSPConfigFolder = DSPConfigFolder2

topSequence+=LArPhysOFCAlg2

if NColl > 0:
   LArPhysOFCAlgmu = LArOFCAlg("LArPhysOFCAlgmu")
   
   LArPhysOFCAlgmu.ReadCaliWave = False
   LArPhysOFCAlgmu.KeyList      = [ InputKey ]
   LArPhysOFCAlgmu.Normalize    = Normalize
   LArPhysOFCAlgmu.Verify       = True
   LArPhysOFCAlgmu.GroupingType = GroupingType
   
   LArPhysOFCAlgmu.Nphase    = Nphases
   LArPhysOFCAlgmu.Dphase    = Dphases
   LArPhysOFCAlgmu.Ndelay    = Ndelays
   LArPhysOFCAlgmu.Nsample   = Nsamples
   LArPhysOFCAlgmu.FillShape = False
   LArPhysOFCAlgmu.TimeShift = TimeShift
   LArPhysOFCAlgmu.TimeShiftByIndex = TimeShiftByIndex
   LArPhysOFCAlgmu.AddTimeOffset = -1.0*TimeShiftGuardRegion
   if not SuperCells: # FIXME when shift for SC available
      LArPhysOFCAlgmu.LArPhysWaveBinKey = ShiftKey
      
   LArPhysOFCAlgmu.UseDelta = UseDelta
   LArPhysOFCAlgmu.KeyOFC   = OFCKey+"_mu"
   LArPhysOFCAlgmu.DecoderTool = theLArPhysAutoCorrDecoderTool
   LArPhysOFCAlgmu.isSC = SuperCells

   if Nsamples==4:
      LArPhysOFCAlgmu.ReadDSPConfig   = ReadDSPConfig
      LArPhysOFCAlgmu.DSPConfigFolder = DSPConfigFolder
   
   
   topSequence+=LArPhysOFCAlgmu
   
   LArPhysOFCAlgmu2 = LArOFCAlg("LArPhysOFCAlgmu2")
   
   LArPhysOFCAlgmu2.ReadCaliWave = False
   LArPhysOFCAlgmu2.KeyList      = [ InputKey ]
   LArPhysOFCAlgmu2.Normalize    = Normalize
   LArPhysOFCAlgmu2.Verify       = True
   LArPhysOFCAlgmu2.GroupingType = GroupingType
   
   LArPhysOFCAlgmu2.Nphase    = Nphases2
   LArPhysOFCAlgmu2.Dphase    = Dphases2
   LArPhysOFCAlgmu2.Ndelay    = Ndelays2
   LArPhysOFCAlgmu2.Nsample   = Nsamples2
   LArPhysOFCAlgmu2.FillShape = False
   LArPhysOFCAlgmu2.TimeShift = TimeShift2
   LArPhysOFCAlgmu2.TimeShiftByIndex = TimeShiftByIndex2
   LArPhysOFCAlgmu2.AddTimeOffset = -1.0*TimeShiftGuardRegion
   if not SuperCells: # FIXME when shift for SC available
      LArPhysOFCAlgmu2.LArPhysWaveBinKey = ShiftKey
   LArPhysOFCAlgmu2.UseDelta = UseDelta
   
   LArPhysOFCAlgmu2.KeyOFC   = OFCKey2+"_mu"
   LArPhysOFCAlgmu2.DecoderTool = theLArPhysAutoCorrDecoderTool
   LArPhysOFCAlgmu2.isSC = SuperCells
   
   if Nsamples2==4:
      LArPhysOFCAlgmu2.ReadDSPConfig   = ReadDSPConfig2
      LArPhysOFCAlgmu2.DSPConfigFolder = DSPConfigFolder2

   topSequence+=LArPhysOFCAlgmu2
###########################################################################

if ( WriteNtuple ) :

   if not 'InputBadChannelSQLiteFile' in dir():
      OFCLog.info( "Read Bad Channels from Oracle DB")
   else :   
      OFCLog.info( "Read Bad Channels from SQLite file") 
   
   if 'BadChannelsLArCalibFolderTag' in dir() :
      BadChannelsTagSpec = LArCalibFolderTag (BadChannelsFolder,BadChannelsLArCalibFolderTag) 
      conddb.addFolder("",BadChannelsFolder+"<tag>"+BadChannelsTagSpec+"</tag>"+"<dbConnection>"+InputDBConnectionBadChannel+"</dbConnection>",className='CondAttrListCollection')
   else :
      conddb.addFolder("",BadChannelsFolder+"<dbConnection>"+InputDBConnectionBadChannel+"</dbConnection>",className='CondAttrListCollection')
   
   from LArBadChannelTool.LArBadChannelToolConf import LArBadChannelCondAlg, LArBadFebCondAlg
   theLArBadChannelCondAlg=LArBadChannelCondAlg(ReadKey=BadChannelsFolder)
   from AthenaCommon.AlgSequence import AthSequencer
   condSeq = AthSequencer("AthCondSeq")
   condSeq+=theLArBadChannelCondAlg

   from LArCalibTools.LArCalibToolsConf import LArOFC2Ntuple 
   
   LArOFC2Ntuple1 = LArOFC2Ntuple("LArOFC2Ntuple1")
   LArOFC2Ntuple1.ContainerKey = OFCKey
   LArOFC2Ntuple1.NtupleName   = OFCTreeName	   
   LArOFC2Ntuple1.AddFEBTempInfo   = False   
   LArOFC2Ntuple1.isSC = SuperCells
   topSequence+=LArOFC2Ntuple1

   LArOFC2Ntuple2 = LArOFC2Ntuple("LArOFC2Ntuple2")
   LArOFC2Ntuple2.ContainerKey = OFCKey2
   LArOFC2Ntuple2.NtupleName   = OFCTreeName2 	   
   LArOFC2Ntuple2.AddFEBTempInfo   = False 	   
   LArOFC2Ntuple2.isSC = SuperCells
   topSequence+=LArOFC2Ntuple2

   if NColl > 0:
      LArOFC2Ntuple1mu = LArOFC2Ntuple("LArOFC2Ntuple1mu")
      LArOFC2Ntuple1mu.ContainerKey = OFCKey+"_mu"
      LArOFC2Ntuple1mu.NtupleName   = OFCTreeName+"_mu"
      LArOFC2Ntuple1mu.AddFEBTempInfo   = False   
      LArOFC2Ntuple1mu.isSC = SuperCells
      topSequence+=LArOFC2Ntuple1mu
 
      LArOFC2Ntuple2mu = LArOFC2Ntuple("LArOFC2Ntuple2mu")
      LArOFC2Ntuple2mu.ContainerKey = OFCKey2+"_mu"
      LArOFC2Ntuple2mu.NtupleName   = OFCTreeName2+"_mu"
      LArOFC2Ntuple2mu.AddFEBTempInfo   = False 	
      LArOFC2Ntuple2mu.isSC = SuperCells   
      topSequence+=LArOFC2Ntuple2mu

   if ( FillShape ):

      from LArCalibTools.LArCalibToolsConf import LArShape2Ntuple

      LArShape2Ntuple1 = LArShape2Ntuple("LArShape2Ntuple1")
      LArShape2Ntuple1.ContainerKey = ShapeKey
      LArShape2Ntuple1.NtupleName   = ShapeTreeName
      LArShape2Ntuple1.AddFEBTempInfo   = False
      LArShape2Ntuple1.isSC = SuperCells
      topSequence+=LArShape2Ntuple1

      LArShape2Ntuple2 = LArShape2Ntuple("LArShape2Ntuple2")
      LArShape2Ntuple2.ContainerKey = ShapeKey2 	   
      LArShape2Ntuple2.NtupleName   = ShapeTreeName2
      LArShape2Ntuple2.AddFEBTempInfo   = False
      LArShape2Ntuple2.isSC = SuperCells
      topSequence+=LArShape2Ntuple2


   if ( doPhysWaveShifter ):
      from LArCalibTools.LArCalibToolsConf import LArFebTimeOffset2Ntuple
      LArFebTimeOffset2Ntuple                = LArFebTimeOffset2Ntuple( "LArFebTimeOffset2Ntuple" )
      LArFebTimeOffset2Ntuple.ContainerKey   = "FebTimeOffset"
      LArFebTimeOffset2Ntuple.AddFEBTempInfo   = False
      LArFebTimeOffset2Ntuple.isSC = SuperCells

      topSequence+=LArFebTimeOffset2Ntuple

      from LArCalibTools.LArCalibToolsConf import LArWFParams2Ntuple
      LArWFParams2Ntuple=LArWFParams2Ntuple("LArWFParams2Ntuple")
      LArWFParams2Ntuple.DumpOFCBin=True
      LArWFParams2Ntuple.AddFEBTempInfo   = False
      LArWFParams2Ntuple.isSC = SuperCells
      topSequence+=LArWFParams2Ntuple
      
   theApp.HistogramPersistency = "ROOT"
   from GaudiSvc.GaudiSvcConf import NTupleSvc
   if os.path.exists(OutputOFCRootFileDir+"/"+OutputOFCRootFileName): 
      os.remove(OutputOFCRootFileDir+"/"+OutputOFCRootFileName)
   svcMgr += NTupleSvc()
   svcMgr.NTupleSvc.Output = [ "FILE1 DATAFILE='"+OutputOFCRootFileDir+"/"+OutputOFCRootFileName+"' OPT='NEW'" ]
   
if (  WritePoolFile ) :
   
   from RegistrationServices.OutputConditionsAlg import OutputConditionsAlg
   if os.path.exists(OutputOFCPoolFileDir+"/"+OutputPoolFileName): 
      os.remove(OutputOFCPoolFileDir+"/"+OutputPoolFileName)
      
   OutputConditionsAlg = OutputConditionsAlg("OutputConditionsAlg",OutputOFCPoolFileDir+"/"+OutputPoolFileName,
                                             [OutputObjectSpecOFC],[OutputObjectSpecTagOFC],WriteIOV)
   OutputConditionsAlg.Run1     = IOVBegin
   if IOVEnd>0:
      OutputConditionsAlg.Run2  = IOVEnd
      
   OutputConditionsAlg.ObjectList += [OutputObjectSpecOFC2]
   OutputConditionsAlg.IOVTagList += [OutputObjectSpecTagOFC2]

   if NColl > 0:
      OutputConditionsAlg.ObjectList += [OutputObjectSpecOFCmu]
      OutputConditionsAlg.IOVTagList += [OutputObjectSpecTagOFCmu]
      OutputConditionsAlg.ObjectList += [OutputObjectSpecOFC2mu]
      OutputConditionsAlg.IOVTagList += [OutputObjectSpecTagOFC2mu]

   if ( FillShape ):
      OutputConditionsAlg.ObjectList += [OutputObjectSpecShape]
      OutputConditionsAlg.IOVTagList += [OutputObjectSpecTagShape]
      OutputConditionsAlg.ObjectList += [OutputObjectSpecShape2]
      OutputConditionsAlg.IOVTagList += [OutputObjectSpecTagShape2]

   if ( doPhysWaveShifter):
      OutputConditionsAlg.ObjectList += [OutputObjectSpecOFCBin]
      OutputConditionsAlg.IOVTagList += [OutputObjectSpecTagOFCBin]

   svcMgr.IOVDbSvc.dbConnection  = OutputDB
   
   from RegistrationServices.RegistrationServicesConf import IOVRegistrationSvc
   svcMgr += IOVRegistrationSvc()
   svcMgr.IOVRegistrationSvc.OutputLevel = DEBUG
   svcMgr.IOVRegistrationSvc.RecreateFolders = False
  		
###########################################################################
#               Use EventSelector to select IOV                           #
###########################################################################	

from McEventSelector.McEventSelectorConf import McEventSelector
svcMgr += McEventSelector("EventSelector")
svcMgr.EventSelector.RunNumber	= int(RunNumber)
svcMgr.EventSelector.EventsPerRun      = 1
svcMgr.EventSelector.FirstEvent	= 1
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

svcMgr+=CfgMgr.AthenaEventLoopMgr(OutputLevel = VERBOSE)

from AthenaCommon.AppMgr import theAuditorSvc
from AthenaCommon.ConfigurableDb import getConfigurable
theAuditorSvc += getConfigurable("MemStatAuditor")(OutputLevel = DEBUG)
theAuditorSvc += getConfigurable("ChronoAuditor")()
theAuditorSvc += getConfigurable("NameAuditor")()

###########################################################################
