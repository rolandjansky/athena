import commands

###########################################################################
#
# <Marco.Delmastro@cern.ch>
#
# Example jobOptions to compute OFC phys in all 3 gains
#
# Last update: 09/12/2008 <Marco.Delmastro@cern.ch>
#
###########################################################################

include("LArCalibProcessing/LArCalib_Flags.py")

###########################################################################
#             PhysWaveShifter properties                                  #
###########################################################################

if not 'doPhysWaveShifter' in dir():
   doPhysWaveShifter = True 

if not 'doOFCPicker' in dir():
   doOFCPicker=False

if not 'InputKey' in dir():   
   if (doPhysWaveShifter) :
      InputKey = "LArPhysWaveUnShifted"
   else :
      InputKey = "LArPhysWave"  # no shift applied to the PhysWave   

if not 'isFCAL' in dir() :
   isFCAL = False

if not 'InputKey' in dir():
   InputKey = "LArPhysWaveUnShifted"

if not 'ShiftMode' in dir():
   ShiftMode = 2  # 1 = minimum Tstart from helper in FEB
                  # 2 = minumum Tpeak-NindexFromPeak in FEB
                  # 3 = average Tpeak-NindexFromPeak in FEB
                  
if not 'Nsamplings' in dir():
   Nsamplings = 2

if not 'TimeShiftGuardRegion' in dir():
   TimeShiftGuardRegion = 5

if not 'UsePhysCaliTdiff' in dir():
   UsePhysCaliTdiff = False

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
   InputAutoCorrPoolDir = commands.getoutput("pwd")

if not 'InputAutoCorrPoolFileName' in dir():
   InputAutoCorrPoolFileName = "LArAutoCorr.pool.root"

## PhysWave

if not 'ReadPhysWaveFromCOOL' in dir():
   ReadPhysWaveFromCOOL = True

if not 'InputPhysWavePoolDir' in dir():
   InputPhysWavePoolDir = commands.getoutput("pwd")

if not 'InputPhysWavePoolFileName' in dir():
   InputPhysWavePoolFileName = "LArPhysWave.pool.root"  

if not 'ChannelSelection' in dir():
   # Read all
   ChannelSelection = " "
   ## Example to read only cool for Barrel C : Strips->Back
   #ChannelSelection = "<channelSelection>0,35:66</channelSelection>" 

from string import *
def DBConnectionFile(sqlitefile):
   return "sqlite://;schema="+sqlitefile+";dbname=CONDBR2"

###########################################################################
# OFC properties
###########################################################################

if not 'Nsamples' in dir():
   Nsamples = 5

if not 'Nphases' in dir():
   Nphases = 50

if not 'Dphases' in dir(): # not yest implemeented!
   Dphases = 1
   
if not 'Ndelays' in dir():
   Ndelays = 24
	
if not 'ContainerKey' in dir():
   ContainerKey = "LArPhysWave"
   
if not 'OFCKey' in dir():
   OFCKey = "LArOFC"

if not 'ShapeKey' in dir():
   ShapeKey = "LArShape"
	
if not 'Normalize' in dir():
   Normalize = True

if not 'TimeShift' in dir() :
   TimeShift = False

if not 'TimeShiftByIndex' in dir() :
   TimeShiftByIndex = -1

if not 'FillShape' in dir():
   FillShape = True # Do not fill a LArShapeComplete object for calibration OFC!

if not 'DumpOFC' in dir():
   DumpOFC = False

###########################################################################
# OFC output
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

if not 'OutputOFCRootFileDir' in dir():
   OutputOFCRootFileDir = commands.getoutput("pwd")
   
if not 'OutputPoolFileDir' in dir():
   OutputPoolFileDir = commands.getoutput("pwd")

OFCFileTag = str(RunNumber)+"_"+Partition.replace("*","")

OFCFileTag += "_"+str(Nsamples)+"samples"

if (Dphases>1): # not yet implemented!
   OFCFileTag += "_"+str(Dphases)+"Dphase"

if not 'OutputOFCRootFileName' in dir():
   OutputOFCRootFileName = "LArOFCPhys_"+OFCFileTag + ".root"
   
if not 'OutputPoolFileName' in dir():
   OutputPoolFileName = "LArOFCShapePhys_"+OFCFileTag + ".pool.root"

if not 'LArPhysFolderOutputTag' in dir():
   LArPhysFolderOutputTag = LArCalib_Flags.tagSuffix  

if not 'OutputDB' in dir():
   OutputDB = LArCalib_Flags.OutputDB

if 'OutputSQLiteFile' in dir():
   OutputDB = DBConnectionFile(OutputSQLiteFile)

if not 'OutputObjectSpecOFC' in dir():
   if doOFCPicker:
       FolderOFC = "/LAR/ElecCalibOfl/OFC/PhysWave/RTM/5samples1phase"
       OutputObjectSpecOFC       = "LArOFCComplete#LArOFC#"+FolderOFC 
       OutputObjectSpecTagOFC    = LArCalibFolderTag(FolderOFC,LArPhysFolderOutputTag)
   else:
      OutputObjectSpecOFC   = "LArOFCComplete#"  +OFCKey  +"#"+ LArCalib_Flags.LArOFCPhysFolder+str(Nsamples)+"samples"	
      OutputObjectSpecTagOFC    = LArCalibFolderTag(LArCalib_Flags.LArOFCPhysFolder+str(Nsamples)+"samples",LArPhysFolderOutputTag)
      
if not 'OutputObjectSpecShape' in dir():
   if doOFCPicker:
      FolderShape = "/LAR/ElecCalibOfl/Shape/RTM/5samples1phase"
      OutputObjectSpecShape       = "LArShapeComplete#LArShape#"+FolderShape 
      OutputObjectSpecTagShape    =  LArCalibFolderTag(FolderShape,LArPhysFolderOutputTag)
   else:
      OutputObjectSpecShape = "LArShapeComplete#"+ShapeKey+"#"+ LArCalib_Flags.LArShapeFolder+str(Nsamples)+"samples"		
      OutputObjectSpecTagShape  = LArCalibFolderTag(LArCalib_Flags.LArShapeFolder+str(Nsamples)+"samples",LArPhysFolderOutputTag)
     
if not 'DBConnectionCOOL' in dir():
   DBConnectionCOOL = "oracle://ATLAS_COOLPROD;schema=ATLAS_COOLOFL_LAR;dbname=CONDBR2;"

if ( ReadAutoCorrFromCOOL ):      
   if 'InputAutoCorrSQLiteFile' in dir():
      InputDBConnectionAutoCorr = DBConnectionFile(InputAutoCorrSQLiteFile)
   else:
      InputDBConnectionAutoCorr = DBConnectionCOOL

if ( ReadPhysWaveFromCOOL ):      
   if 'InputPhysWaveSQLiteFile' in dir():
      InputDBConnectionPhysWave = DBConnectionFile(InputPhysWaveSQLiteFile)
   else:
      InputDBConnectionPhysWave = DBConnectionCOOL

if ( ReadPhysCaliTdiffFromCOOL ):
   if 'InputPhysCaliTdiffSQLiteFile' in dir():
      InputDBConnectionPhysCaliTdiff = DBConnectionFile(InputPhysCaliTdiffSQLiteFile)
   else:
      InputDBConnectionPhysCaliTdiff = DBConnectionCOOL

## Bad Channel   
   
if not 'ReadBadChannelFromCOOL' in dir():
   ReadBadChannelFromCOOL = True   

if ( ReadBadChannelFromCOOL ):      
   if 'InputBadChannelSQLiteFile' in dir():
      InputDBConnectionBadChannel = DBConnectionFile(InputBadChannelSQLiteFile)
   else:
      #InputDBConnectionBadChannel = "oracle://ATLAS_COOLPROD;schema=ATLAS_COOLONL_LAR;dbname=CONDBR2;"
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
else :
   OFCLog.info( " InputAutoCorrPoolFileName          = "+InputAutoCorrPoolFileName )
if 'AutoCorrLArCalibFolderTag' in dir() :   
   OFCLog.info( " AutoCorrLArCalibFolderTag          = "+AutoCorrLArCalibFolderTag )
if ( ReadPhysWaveFromCOOL ):
   OFCLog.info( " InputDBConnectionPhysWave          = "+InputDBConnectionPhysWave )
else :
   OFCLog.info( " InputPhysWavePoolFileName          = "+InputPhysWavePoolFileName )
if ( (ReadAutoCorrFromCOOL or ReadPhysWaveFromCOOL) and ChannelSelection != " " ):
   OFCLog.info( " ChannelSelection                   = "+ChannelSelection )
if 'PhysWaveLArCalibFolderTag' in dir() :
   OFCLog.info( " PhysWaveLArCalibFolderTag          = "+PhysWaveLArCalibFolderTag )
OFCLog.info( " OutputOFCRootFullFileName          = "+OutputOFCRootFileDir+"/"+OutputOFCRootFileName )
OFCLog.info( " OutputPoolFullFileName             = "+OutputPoolFileDir+"/"+OutputPoolFileName )
OFCLog.info( " OutputObjectSpecOFC                = "+str(OutputObjectSpecOFC) )
OFCLog.info( " OutputObjectSpecTagOFC             = "+str(OutputObjectSpecTagOFC) )
OFCLog.info( " OutputObjectSpecShape              = "+str(OutputObjectSpecShape) )
OFCLog.info( " OutputObjectSpecTagShape           = "+str(OutputObjectSpecTagShape) )
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

###########################################################################
#                                                                         #
#    Input conditions data (AutoCorr, CaliWave) for COOL and/or POOL      #
#                                                                         #
###########################################################################

include("AthenaPoolCnvSvc/AthenaPool_jobOptions.py")
include("LArCondAthenaPool/LArCondAthenaPool_joboptions.py")

from IOVDbSvc.CondDB import conddb
PoolFileList     = []

include ("LArCalibProcessing/LArCalib_BadChanTool.py")

if not 'InputBadChannelSQLiteFile' in dir():
   OFCLog.info( "Read Bad Channels from Oracle DB")
else :   
   OFCLog.info( "Read Bad Channels from SQLite file") 

if 'BadChannelsLArCalibFolderTag' in dir() :
   BadChannelsTagSpec = LArCalibFolderTag (BadChannelsFolder,BadChannelsLArCalibFolderTag) 
   conddb.addFolder("",BadChannelsFolder+"<tag>"+BadChannelsTagSpec+"</tag>"+"<dbConnection>"+InputDBConnectionBadChannel+"</dbConnection>")
else :
   conddb.addFolder("",BadChannelsFolder+"<dbConnection>"+InputDBConnectionBadChannel+"</dbConnection>")

if 'MissingFEBsLArCalibFolderTag' in dir() :
   MissingFEBsTagSpec = LArCalibFolderTag (MissingFEBsFolder,MissingFEBsLArCalibFolderTag)   
   conddb.addFolder("",MissingFEBsFolder+"<tag>"+MissingFEBsTagSpec+"</tag>"+"<dbConnection>"+InputDBConnectionBadChannel+"</dbConnection>")
else :
   conddb.addFolder("",MissingFEBsFolder+"<dbConnection>"+InputDBConnectionBadChannel+"</dbConnection>")

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
   AutoCorrFolder  = LArCalib_Flags.LArAutoCorrFolder
   if 'AutoCorrLArCalibFolderTag' in dir():
      
      AutoCorrTagSpec = LArCalibFolderTag(AutoCorrFolder,AutoCorrLArCalibFolderTag)
      conddb.addFolder("",AutoCorrFolder+"<tag>"+AutoCorrTagSpec+"</tag>"+"<dbConnection>"+InputDBConnectionAutoCorr+"</dbConnection>"+ ChannelSelection)
   else :
      conddb.addFolder("",AutoCorrFolder+"<dbConnection>"+InputDBConnectionAutoCorr+"</dbConnection>"+ ChannelSelection)
else:
   if 'InputAutoCorrPoolFileName' in dir():
      OFCLog.info( "Read AutoCorr from POOL file")
      PoolFileList += [ InputAutoCorrPoolDir+"/"+InputAutoCorrPoolFileName ]
   else:
      OFCLog.info( "No PoolFileList found! Please list the POOL files containing AutoCorrelation Matrix or read from COOL." )
      theApp.exit(-1)      
      
if ( ReadPhysWaveFromCOOL ):
   PhysWaveFolder  = LArCalib_Flags.LArPhysWaveFolder
   if not (isFCAL) :
      if (doPhysWaveShifter) :
         if 'PhysWaveLArCalibFolderTag' in dir() :
            PhysWaveTagSpec = LArCalibFolderTag(PhysWaveFolder,PhysWaveLArCalibFolderTag)
            conddb.addFolder("",PhysWaveFolder+"<key>"+InputKey+"</key><tag>"+PhysWaveTagSpec+"</tag>"+"<dbConnection>"+InputDBConnectionPhysWave+"</dbConnection>" + ChannelSelection)
         else :
            conddb.addFolder("",PhysWaveFolder+"<key>"+InputKey+"</key><dbConnection>"+InputDBConnectionPhysWave+"</dbConnection>" + ChannelSelection)
      else :
         if 'PhysWaveLArCalibFolderTag' in dir() :
            PhysWaveTagSpec = LArCalibFolderTag(PhysWaveFolder,PhysWaveLArCalibFolderTag)
            conddb.addFolder("",PhysWaveFolder+"<tag>"+PhysWaveTagSpec+"</tag>"+"<dbConnection>"+InputDBConnectionPhysWave+"</dbConnection>" + ChannelSelection)
         else :
            conddb.addFolder("",PhysWaveFolder+"<dbConnection>"+InputDBConnectionPhysWave+"</dbConnection>" + ChannelSelection)
   else :
      if 'PhysWaveLArCalibFolderTag' in dir() :
         PhysWaveTagSpec = LArCalibFolderTag(PhysWaveFolder,PhysWaveLArCalibFolderTag)
         conddb.addFolder("",PhysWaveFolder+"<key>"+InputKey+"</key><tag>"+PhysWaveTagSpec+"</tag><dbConnection>"+InputDBConnectionPhysWave+"</dbConnection>" + ChannelSelection)
      else :
         PhysWaveFolderFCAL="/LAR/ElecCalibOfl/PhysWaves/FCALFromTB"
         conddb.addFolder("",PhysWaveFolderFCAL+"<key>"+InputKey+"</key><dbConnection>"+InputDBConnectionPhysWave+"</dbConnection>" + ChannelSelection)
         
else:
   if 'InputPhysWavePoolFileName' in dir():
      OFCLog.info( "Read PhysWave from POOL file" )
      PoolFileList += [ InputPhysWavePoolDir+"/"+InputPhysWavePoolFileName ]
   else:
      OFCLog.info( "No PoolFileList found! Please list the POOL files containing CaliWave or read from COOL." )
      theApp.exit(-1)




if (UsePhysCaliTdiff):
   if ( ReadPhysCaliTdiffFromCOOL ):
      PhysCaliTdiffFolder  = LArCalib_Flags.LArPhysCaliTdiffFolder
      if 'PhysCaliTdiffLArCalibFolderTag' in dir() :
         PhysCaliTdiffTagSpec = LArCalibFolderTag(PhysCaliTdiffFolder,PhysCaliTdiffLArCalibFolderTag)
         conddb.addFolder("",PhysCaliTdiffFolder+"<key>"+PhysCaliTdiffKey+"</key><tag>"+PhysCaliTdiffTagSpec+"</tag>"+"<dbConnection>"+InputDBConnectionPhysCaliTdiff+"</dbConnection>" + ChannelSelection)
      else :
         conddb.addFolder("",PhysCaliTdiffFolder+"<key>"+PhysCaliTdiffKey+"</key><dbConnection>"+InputDBConnectionPhysCaliTdiff+"</dbConnection>" + ChannelSelection)
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
   LArPhysWaveShifter.TimeShiftByLayer       = False
   LArPhysWaveShifter.TshiftLayer            = [ 0 , 0 , 0 , 0 ]
   LArPhysWaveShifter.TimeShiftFromPeak      = False
   LArPhysWaveShifter.NindexFromPeak         = 0 # if 0 will use Ndelays*Nsampling
   LArPhysWaveShifter.Ndelays                = 24
   LArPhysWaveShifter.Nsamplings             = Nsamplings 
   LArPhysWaveShifter.TimeShiftByFEB         = True
   LArPhysWaveShifter.TimeShiftGuardRegion   = TimeShiftGuardRegion
   LArPhysWaveShifter.UsePhysCaliTdiff       = UsePhysCaliTdiff

   topSequence += LArPhysWaveShifter   

###########################################################################
#                      OFC computation
###########################################################################

from LArCalibUtils.LArCalibUtilsConf import LArOFCAlg
LArPhysOFCAlg = LArOFCAlg("LArPhysOFCAlg")
LArPhysOFCAlg.ReadCaliWave = False
LArPhysOFCAlg.KeyList   = [ ContainerKey ]
LArPhysOFCAlg.Nphase    = Nphases
LArPhysOFCAlg.Dphase    = Dphases
LArPhysOFCAlg.Ndelay    = Ndelays
LArPhysOFCAlg.Nsample   = Nsamples
LArPhysOFCAlg.Normalize = Normalize
LArPhysOFCAlg.TimeShift = TimeShift
LArPhysOFCAlg.TimeShiftByIndex = TimeShiftByIndex
LArPhysOFCAlg.Verify    = True
LArPhysOFCAlg.FillShape = FillShape
if ( doOFCPicker):
   LArPhysOFCAlg.KeyOFC    = "tempOFC"
   LArPhysOFCAlg.KeyShape  = "tempShape"
else:
    LArPhysOFCAlg.KeyOFC   = OFCKey
    LArPhysOFCAlg.KeyShape = ShapeKey
    
if ( DumpOFC ) :
	LArPhysOFCAlg.DumpOFCfile = "LArOFCPhys.dat"
LArPhysOFCAlg.GroupingType = GroupingType
topSequence+=LArPhysOFCAlg

from LArCalibUtils.LArCalibUtilsConf import LArAutoCorrDecoderTool
theLArAutoCorrDecoderTool = LArAutoCorrDecoderTool()
ToolSvc += theLArAutoCorrDecoderTool


if ( doOFCPicker):
   conddb.addFolder("","/LAR/ElecCalibOfl/OFCBin/Dummy<dbConnection>"+DBConnectionCOOL+"</dbConnection>"+ ChannelSelection)
   from LArCalibUtils.LArCalibUtilsConf import LArOFPhasePicker
   LArOFPhasePicker = LArOFPhasePicker("LArOFPhasePicker")
   LArOFPhasePicker.KeyPhase = "LArOFCPhase"
   LArOFPhasePicker.KeyOFC_new = OFCKey
   LArOFPhasePicker.KeyOFC = "tempOFC"

   LArOFPhasePicker.KeyShape_new = ShapeKey
   LArOFPhasePicker.KeyShape_newcorr = ShapeKeyCorr
   LArOFPhasePicker.KeyShape = "tempShape"
   LArOFPhasePicker.KeyShapeResidual = "LArResiduals"
   LArOFPhasePicker.doShapeCorrection = FillShapeCorr

   LArOFPhasePicker.GroupingType = GroupingType
   LArOFPhasePicker.DefaultPhase = DefaultPhase
   LArOFPhasePicker.TimeOffsetCorrection = TimeOffsetCorrection
   topSequence += LArOFPhasePicker




###########################################################################

if ( WriteNtuple ) :

   from LArCalibTools.LArCalibToolsConf import LArOFC2Ntuple
   LArOFC2Ntuple = LArOFC2Ntuple("LArOFC2Ntuple")
   #LArOFC2Ntuple.ContainerKey = "tempOFC"
   LArOFC2Ntuple.ContainerKey = OFCKey
   LArOFC2Ntuple.AddFEBTempInfo   = False   
   topSequence+=LArOFC2Ntuple


   if ( doPhysWaveShifter ):
      from LArCalibTools.LArCalibToolsConf import LArFebTimeOffset2Ntuple
      LArFebTimeOffset2Ntuple                = LArFebTimeOffset2Ntuple( "LArFebTimeOffset2Ntuple" )
      LArFebTimeOffset2Ntuple.ContainerKey   = "FebTimeOffset"
      LArFebTimeOffset2Ntuple.AddFEBTempInfo   = False

      topSequence+=LArFebTimeOffset2Ntuple
   

   if ( FillShape ):
      from LArCalibTools.LArCalibToolsConf import LArShape2Ntuple
      LArShape2Ntuple = LArShape2Ntuple("LArShape2Ntuple")
      LArShape2Ntuple.ContainerKey = ShapeKey
      #LArShape2Ntuple.ContainerKey =  "tempShape"
      LArShape2Ntuple.AddFEBTempInfo   = False   
      topSequence+=LArShape2Ntuple


   theApp.HistogramPersistency = "ROOT"
   from GaudiSvc.GaudiSvcConf import NTupleSvc
   if os.path.exists(OutputOFCRootFileDir+"/"+OutputOFCRootFileName): 
      os.remove(OutputOFCRootFileDir+"/"+OutputOFCRootFileName)
   svcMgr += NTupleSvc()
   svcMgr.NTupleSvc.Output = [ "FILE1 DATAFILE='"+OutputOFCRootFileDir+"/"+OutputOFCRootFileName+"' OPT='NEW'" ]
   
if (  WritePoolFile ) :
   
   from RegistrationServices.OutputConditionsAlg import OutputConditionsAlg
   if os.path.exists(OutputPoolFileDir+"/"+OutputPoolFileName): 
      os.remove(OutputPoolFileDir+"/"+OutputPoolFileName)
   OutputConditionsAlg = OutputConditionsAlg("OutputConditionsAlg",OutputPoolFileDir+"/"+OutputPoolFileName,
                                                [OutputObjectSpecOFC],[OutputObjectSpecTagOFC],WriteIOV)
   OutputConditionsAlg.Run1     = IOVBegin
   if IOVEnd>0:
      OutputConditionsAlg.Run2  = IOVEnd
	
   if ( FillShape ):
      OutputConditionsAlg.ObjectList += [OutputObjectSpecShape]
      OutputConditionsAlg.IOVTagList += [OutputObjectSpecTagShape]

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
