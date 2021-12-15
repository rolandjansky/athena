from future import standard_library
standard_library.install_aliases()
import subprocess

###########################################################################
#
# <Marco.Delmastro@cern.ch>
#
# Example jobOptions to compute PredictedPhysWave
#
# Last update: 25/01/2008 <Fabien.Tarrade@cern.ch>
#
###########################################################################

include("LArCalibProcessing/LArCalib_Flags.py")

###########################################################################
#                   Input selection (CaliWave)
###########################################################################

# RunNumber trigger IOV if reading from COOL is eneabled
if not 'RunNumber' in dir():
   RunNumber = 0

if not 'SubDet' in dir():
   SubDet = "Barrel"  

if not 'Partition' in dir():
   if (SubDet=='EndCap'):
      Partition      = "EB-EC*"
   else:
      Partition      = "EB-EMB*"
      
if not 'ReadCaliWaveFromCOOL' in dir():
   ReadCaliWaveFromCOOL = True

if not 'CaliWaveLArCalibFolderTag' in dir():
   CaliWaveLArCalibFolderTag = LArCalib_Flags.tagSuffix   

if not 'InputCaliWavePoolDir' in dir():
   InputCaliWavePoolDir = subprocess.getoutput("pwd")

if not 'InputCaliWavePoolFileName' in dir():
   InputCaliWavePoolFileName = "LArCaliWave.pool.root"

if not 'ChannelSelection' in dir():
   # Read all
   ChannelSelection = " "
   ## Example to read only cool for Barrel C : Strips->Back
   #ChannelSelection = "<channelSelection>0,35:66</channelSelection>" 

from string import *
def DBConnectionFile(sqlitefile):
   return "sqlite://;schema="+sqlitefile+";dbname=CONDBR2"

###########################################################################
#                   PhysWavePredictor properties
###########################################################################

if not 'StripsXtalkCorr' in dir():
   StripsXtalkCorr = True # Xtalk correction for strips
   
if not 'KeyCaliList' in dir():
   KeyCaliList   = [ "LArCaliWave" ]

if not 'LArMphysMcaliKey' in dir ():
   LArMphysMcaliKey = "LArMphysOverMcal"

if not 'doTest' in dir():
   doTest = False
	
# Tcali, Fstep
if not 'UseCaliPulseParamsFromJO' in dir():
   UseCaliPulseParamsFromJO = False

# Omega0, Taur
if not 'UseDetCellParamsFromJO' in dir():
   UseDetCellParamsFromJO = False

# Tdrift
if not 'UseTdriftFromJO' in dir():
   UseTdriftFromJO   = True

if not 'UseDoubleTriangle' in dir():
   UseDoubleTriangle = True

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

# Tdiff
if not 'UseTimeShiftFromJO' in dir():
   UseTimeShiftFromJO = True

if not 'InjPointCorrLayer' in dir():
   InjPointCorrLayer = [ 0, 0, 1, 0 ]
        
if not 'InjPointUseTauR' in dir():
   InjPointUseTauR   = [ 0, 0, 1, 0 ]

if not 'WritePhysWave2NTuple' in dir():
   WritePhysWave2NTuple = True  # write predicted LArPhysWave to ntuple

if not 'WriteCaliWave2NTuple' in dir():
   WriteCaliWave2NTuple = False # add LArCaliWave to predicted LArPhysWave ntuple

if not 'WriteMphyMcal2NTuple' in dir():
   WriteMphyMcal2NTuple = True  # add LArMphysOverMcalComplete to predicted LArPhysWave ntuple

###########################################################################
#                     PhysWavePredicted output
###########################################################################

if not 'GroupingType' in dir():
   if SuperCells:
      GroupingType = "SuperCells"
   else:
      GroupingType = "ExtendedSubDetector"
if not 'WriteNtuple' in dir():
   WriteNtuple = LArCalib_Flags.WriteNtuple

if not 'WritePoolFile' in dir():
   WritePoolFile = LArCalib_Flags.WritePoolFile

if not 'WriteIOV' in dir():
   WriteIOV = LArCalib_Flags.WriteIOV

if not 'IOVBegin' in dir():
   IOVBegin = RunNumber

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

if not 'LArPhysFolderOutputTag' in dir():
   LArPhysFolderOutputTag = LArCalib_Flags.tagSuffix  

if not 'OutputDB' in dir():
   OutputDB = LArCalib_Flags.OutputDB

if 'OutputSQLiteFile' in dir():
   OutputDB = DBConnectionFile(OutputSQLiteFile)

if not 'OutputObjectSpecPhysWave' in dir():
   OutputObjectSpecPhysWave   = "LArPhysWaveContainer#"+"LArPhysWave"+"#"+ "/LAR/ElecCalibOfl/PhysWaves/TempWillExistOnlyInTheSQLiteFile"
   OutputObjectSpecTagPhysWave    = LArCalibFolderTag("/LAR/ElecCalibOfl/PhysWaves/TempWillExistOnlyInTheSQLiteFile",LArPhysFolderOutputTag)
						
if not 'OutputObjectSpecMphysOverMcal' in dir():
   OutputObjectSpecMphysOverMcal = "LArMphysOverMcalComplete#"+"LArMphysOverMcal"+"#"+ LArCalib_Flags.LArMphysOverMcalFolder		
   OutputObjectSpecTagMphysOverMcal  = LArCalibFolderTag(LArCalib_Flags.LArMphysOverMcalFolder,LArPhysFolderOutputTag)

if not 'DBConnectionCOOL' in dir():
   DBConnectionCOOL = "impl=cool;techno=oracle;schema=ATLAS_COOLOFL_LAR;ATLAS_COOLPROD:CONDBR2:ATLAS_COOL_READER:"

if ( ReadCaliWaveFromCOOL ):      
   if 'InputCaliWaveSQLiteFile' in dir():
      InputDBConnectionCaliWave = DBConnectionFile(InputCaliWaveSQLiteFile)
   else:
      InputDBConnectionCaliWave = DBConnectionCOOL

###########################################################################
#                            Print summary
###########################################################################

PhysWaveLog = logging.getLogger( "PhysWaveLog" )
PhysWaveLog.info( " ======================================================== " )
PhysWaveLog.info( " ***            LAr PredPhysWave summary              *** " )
PhysWaveLog.info( " ======================================================== " )
PhysWaveLog.info( " RunNumber                                  = "+str(RunNumber) )
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
PhysWaveLog.info( " OutputObjectSpecMphysOverMcal              = "+str(OutputObjectSpecMphysOverMcal) )
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

include( "AthenaCommon/Atlas_Gen.UnixStandardJob.py" )

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
#        Input conditions data (CaliWave) for COOL and/or POOL            #
#                                                                         #
###########################################################################

include("AthenaPoolCnvSvc/AthenaPool_jobOptions.py")
include("LArCondAthenaPool/LArCondAthenaPool_joboptions.py")

from IOVDbSvc.CondDB import conddb
PoolFileList     = []

from LArCalibProcessing.LArCalibCatalogs import larCalibCatalogs
svcMgr.PoolSvc.ReadCatalog += larCalibCatalogs

if ( ReadCaliWaveFromCOOL ):
   if ( not StripsXtalkCorr ):
      CaliWaveFolder = LArCalib_Flags.LArCaliWaveFolder
   else:
      CaliWaveFolder = LArCalib_Flags.LArCaliWaveFolderXtlk
   CaliWaveTagSpec = LArCalibFolderTag(CaliWaveFolder,CaliWaveLArCalibFolderTag)
   conddb.addFolder("",CaliWaveFolder+"<tag>"+CaliWaveTagSpec+"</tag>"+"<dbConnection>"+InputDBConnectionCaliWave+"</dbConnection>" + ChannelSelection)
else:
   if 'InputCaliWavePoolFileName' in dir():
      PoolFileList += [ InputCaliWavePoolDir+"/"+InputCaliWavePoolFileName ]
   else:
      printfunc ("No PoolFileList found! Please list the POOL files containing CaliWave or read from COOL.")
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

LArPhysWavePredictor = LArPhysWavePredictor( "LArPhysWavePredictor" )

LArPhysWavePredictor.TestMode	              = doTest
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
LArPhysWavePredictor.KeyMphysMcali	      = LArMphysMcaliKey
LArPhysWavePredictor.DumpMphysMcali           = False # set to True to dump on a ASCII file

#
# Avoid using these settings, use LArPhysWaveShifter in a separate job
#

LArPhysWavePredictor.TimeShiftByHelper    = False
LArPhysWavePredictor.TimeShiftByIndex     = 0
LArPhysWavePredictor.TimeShiftByLayer     = False
LArPhysWavePredictor.Tshift	          = [ 0 , 0 , 0 , 0 ]
LArPhysWavePredictor.TimeShiftByFEB       = False
LArPhysWavePredictor.TimeShiftGuardRegion = 0

from LArCalibUtils.LArCalibUtilsConf import LArPhysWaveTool
LArPhysWaveTool.NormalizeCali     = False # this is taken care by LArPhysWavePredictor
LArPhysWaveTool.TimeOriginShift   = False
LArPhysWaveTool.SubtractBaseline  = False
LArPhysWaveTool.InjPointCorrLayer = InjPointCorrLayer
LArPhysWaveTool.InjPointUseTauR   = InjPointUseTauR  

topSequence += LArPhysWavePredictor
###########################################################################

if ( WriteNtuple ) :

   if ( WritePhysWave2NTuple ) :
      from LArCalibTools.LArCalibToolsConf import LArWaves2Ntuple
      LArPhysWaves2Ntuple = LArWaves2Ntuple("LArPhysWaves2Ntuple")
      LArPhysWaves2Ntuple.NtupleName   = "PHYSWAVE" 
      LArPhysWaves2Ntuple.KeyList = [ "LArPhysWave" ]
      LArPhysWaves2Ntuple.ReadCaliWave = False
      
      topSequence += LArPhysWaves2Ntuple
      
   if ( WriteCaliWave2NTuple ) :
      from LArCalibTools.LArCalibToolsConf import LArWaves2Ntuple
      LArCaliWaves2Ntuple = LArWaves2Ntuple("LArCaliWaves2Ntuple")		
      LArCaliWaves2Ntuple.NtupleName = "CALIWAVE"
      LArCaliWaves2Ntuple.KeyList = KeyCaliList
      
      topSequence += LArCaliWaves2Ntuple
      
   if ( WriteMphyMcal2NTuple ) :
      from LArCalibTools.LArCalibToolsConf import LArParams2Ntuple
      LArParams2Ntuple = LArParams2Ntuple( "LArParams2Ntuple" )
      LArParams2Ntuple.KeyList += [ "LArMphysOverMcalComplete" ]
      LArParams2Ntuple.DetStoreSuffix = ""
      
      topSequence += LArParams2Ntuple	
   
   theApp.HistogramPersistency = "ROOT"
   from GaudiSvc.GaudiSvcConf import NTupleSvc
   svcMgr += NTupleSvc()
   svcMgr.NTupleSvc.Output = [ "FILE1 DATAFILE='"+OutputPhysWaveRootFileDir+"/"+OutputPhysWaveRootFileName+"' OPT='NEW'" ]
   
if (  WritePoolFile ) :

   from RegistrationServices.OutputConditionsAlg import OutputConditionsAlg
   # writre PredPhysWave
   OutputConditionsAlgPhysWave = OutputConditionsAlg("OutputConditionsAlgPhysWave",OutputPhysWavePoolFileDir+"/"+OutputPhysWavePoolFileName,
                                                     [OutputObjectSpecPhysWave],[OutputObjectSpecTagPhysWave],WriteIOV)
   #OutputConditionsAlgPhysWave.WriteIOV = WriteIOV
   OutputConditionsAlgPhysWave.Run1     = IOVBegin

   if IOVEnd>0:
      OutputConditionsAlgPhysWave.Run2  = IOVEnd
	
   # writre MphysOverMcal
   OutputConditionsAlgMphysOverMcal = OutputConditionsAlg("OutputConditionsAlgMphysOverMcal",OutputMphysOverMcalPoolFileDir+"/"+OutputMphysOverMcalPoolFileName,
                                                     [OutputObjectSpecMphysOverMcal],[OutputObjectSpecTagMphysOverMcal],WriteIOV)
   #OutputConditionsAlgMphysOverMcal.WriteIOV = WriteIOV
   OutputConditionsAlgMphysOverMcal.Run1     = IOVBegin
   if IOVEnd>0:
      OutputConditionsAlg.MphysOverMcal.Run2  = IOVEnd

   svcMgr.IOVDbSvc.dbConnection  = OutputDB

   from AthenaCommon.AppMgr import ServiceMgr
   from RegistrationServices.RegistrationServicesConf import IOVRegistrationSvc
   svcMgr += IOVRegistrationSvc()
   svcMgr.IOVRegistrationSvc.OutputLevel = VERBOSE
   svcMgr.IOVRegistrationSvc.RecreateFolders = False

###########################################################################
#                Use EventSelector to select IOV                          #
###########################################################################
from McEventSelector.McEventSelectorConf import McEventSelector
svcMgr += McEventSelector("EventSelector")
svcMgr.EventSelector.RunNumber	= RunNumber
svcMgr.EventSelector.EventsPerRun      = 1
svcMgr.EventSelector.FirstEvent	       = 1
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

