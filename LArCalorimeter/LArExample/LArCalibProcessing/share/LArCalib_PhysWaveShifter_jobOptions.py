# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from future import standard_library
standard_library.install_aliases()
import subprocess

###########################################################################
#
# LArCalib_PhysWaveShifter_jobOptions.py
#
# Example jobOption to shift LArPhysWave in time before OFC computation
#
# Last update: 25/09/2008 <Caroline.Collard@cern.ch>
#
###########################################################################

include ("LArCalibProcessing/LArCalib_Flags.py")

###########################################################################
#               Input selection (PhysWave)
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

if not 'ReadPhysWaveFromCOOL' in dir():
   ReadPhysWaveFromCOOL = True
	
if not 'PhysWaveLArCalibFolderTag' in dir():
   PhysWaveLArCalibFolderTag = LArCalib_Flags.tagSuffix  

if not 'InputPhysWavePoolDir' in dir():
   InputPhysWavePoolDir = subprocess.getoutput("pwd")

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
#                PhysWaveShifter properties
###########################################################################

if not 'InputKey' in dir():
	InputKey = "LArPhysWave"

if not 'OutputKey' in dir():
	OutputKey = "LArPhysWave"

###########################################################################
#                          PhysWaveShifter output
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
   IOVBegin = RunNumber

if not 'IOVEnd' in dir():
   IOVEnd = LArCalib_Flags.IOVEnd   
   
if not 'OutputPhysWaveRootFileDir' in dir():
   OutputPhysWaveRootFileDir = subprocess.getoutput("pwd")

PhysWaveFileTag = "Shifted_"+str(RunNumber)+"_"+Partition.replace("*","")

if not 'OutputPhysWaveRootFileName' in dir():
   OutputPhysWaveRootFileName = "LArPhysWave_"+ PhysWaveFileTag + ".root"
   
if not 'OutputPhysWavePoolFileName' in dir():
   OutputPhysWavePoolFileName = "LArPhysWave_"+ PhysWaveFileTag + ".pool.root"

if not 'LArPhysFolderOutputTag' in dir():
   LArPhysFolderOutputTag = LArCalib_Flags.tagSuffix    

if not 'OutputDB' in dir():
   OutputDB = LArCalib_Flags.OutputDB

if 'OutputSQLiteFile' in dir():
   OutputDB = DBConnectionFile(OutputSQLiteFile)

if not 'OutputObjectSpecPhysWave' in dir():
   OutputObjectSpecPhysWave   = "LArPhysWaveContainer#"+"LArPhysWave"+"#"+ LArCalib_Flags.LArPhysWaveFolder
   OutputObjectSpecTagPhysWave    = LArCalibFolderTag(LArCalib_Flags.LArPhysWaveFolder,LArPhysFolderOutputTag) 

if not 'DBConnectionCOOL' in dir():
   DBConnectionCOOL = "impl=cool;techno=oracle;schema=ATLAS_COOLOFL_LAR;ATLAS_COOLPROD:CONDBR2:ATLAS_COOL_READER:"

if ( ReadPhysWaveFromCOOL ):      
   if 'InputPhysWaveSQLiteFile' in dir():
      InputDBConnectionPhysWave = DBConnectionFile(InputPhysWaveSQLiteFile)
   else:
      InputDBConnectionPhysWave = DBConnectionCOOL

###########################################################################
# Print summary
###########################################################################

PhysWaveLog = logging.getLogger( "PhysWaveLog" )
PhysWaveLog.info( " ======================================================== " )
PhysWaveLog.info( " ***           LAr PhysWave summary                  *** " )
PhysWaveLog.info( " ======================================================== " )
PhysWaveLog.info( " RunNumber                               = "+str(RunNumber) )
if ( ReadPhysWaveFromCOOL ):
   PhysWaveLog.info( " InputDBConnectionPhysWave               = "+InputDBConnectionPhysWave )
else :
   PhysWaveLog.info( " InputPhysWavePoolFileName               = "+InputPhysWavePoolFileName )
if ( (ReadPhysWaveFromCOOL) and ChannelSelection != " " ):
   PhysWaveLog.info( " ChannelSelection                        = "+ChannelSelection )
PhysWaveLog.info( " PhysWaveLArCalibFolderTag               = "+PhysWaveLArCalibFolderTag )
PhysWaveLog.info( " OutputPhysWaveRootFullFileName          = "+OutputPhysWaveRootFileDir+"/"+OutputPhysWaveRootFileName )
PhysWaveLog.info( " OutputPhysWavePoolFullFileName          = "+OutputPhysWavePoolFileDir+"/"+OutputPhysWavePoolFileName )
PhysWaveLog.info( " OutputObjectSpecPhysWave                = "+str(OutputObjectSpecPhysWave) )
PhysWaveLog.info( " OutputObjectSpecTagPhysWave             = "+str(OutputObjectSpecTagPhysWave) )
PhysWaveLog.info( " IOVBegin                                = "+str(IOVBegin) )
PhysWaveLog.info( " IOVEnd                                  = "+str(IOVEnd) )
PhysWaveLog.info( " LArCalibOutputDB                        = "+OutputDB )
PhysWaveLog.info( " ======================================================== " )
###########################################################################

###########################################################################
#
#                           Global settings
#
###########################################################################

include( "AthenaCommon/Atlas_Gen.UnixStandardJob.py" )

## get a handle to the default top-level algorithm sequence
from AthenaCommon.AlgSequence import AlgSequence 
topSequence = AlgSequence()

## get a handle to the ApplicationManager, to the ServiceManager and to the ToolSvc
from AthenaCommon.AppMgr import (theApp, ServiceMgr as svcMgr,ToolSvc)

include("LArCalibProcessing/LArCalib_MinimalSetup.py")

svcMgr.IOVDbSvc.GlobalTag   = LArCalib_Flags.globalFlagDB
try:
   svcMgr.IOVDbSvc. DBInstance=""
except: 
   pass

###########################################################################
#                                                                         #
#    Input conditions data (CaliWave) for COOL and/or POOL                #
#                                                                         #
###########################################################################

include("AthenaPoolCnvSvc/AthenaPool_jobOptions.py")
include("LArCondAthenaPool/LArCondAthenaPool_joboptions.py")

from IOVDbSvc.CondDB import conddb
PoolFileList     = []

from LArCalibProcessing.LArCalibCatalogs import larCalibCatalogs
svcMgr.PoolSvc.ReadCatalog += larCalibCatalogs

if ( ReadPhysWaveFromCOOL ):
   PhysWaveFolder  = "/LAR/ElecCalibOfl/PhysWaves/TempWillExistOnlyInTheSQLiteFile"
   PhysWaveTagSpec = LArCalibFolderTag("/LAR/ElecCalibOfl/PhysWaves/TempWillExistOnlyInTheSQLiteFile",PhysWaveLArCalibFolderTag)
   conddb.addFolder("",PhysWaveFolder+"<tag>"+PhysWaveTagSpec+"</tag>"+"<dbConnection>"+InputDBConnectionPhysWave+"</dbConnection>" + ChannelSelection)
   
else:
   if 'InputPhysWavePoolFileName' in dir():
      PoolFileList += [ InputPhysWavePoolDir+"/"+InputPhysWavePoolFileName ]
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
# Shift physics waveforms
#
###########################################################################

from LArCalibUtils.LArCalibUtilsConf import LArPhysWaveShifter
LArPhysWaveShifter = LArPhysWaveShifter("LArPhysWaveShifter")

LArPhysWaveShifter.KeyList      = [ InputKey ]
LArPhysWaveShifter.KeyOutput    = OutputKey
LArPhysWaveShifter.GroupingType = GroupingType

#
# Minimum Tstart per FEB computation
#
LArPhysWaveShifter.ComputeTimeShiftByFEB  = True
LArPhysWaveShifter.TimeShiftByFEBMode     = 2 # 1 = minimum Tstart from helper in FEB
                                              # 2 = minumum Tpeak-NindexFromPeak in FEB
LArPhysWaveShifter.TimeShiftByFEBDump     = True
LArPhysWaveShifter.TimeShiftByFEBDumpFile = "TimeShiftFEB.py"

#
# Possible time shifts
#
LArPhysWaveShifter.TimeShiftByHelper = False
LArPhysWaveShifter.TimeShiftByIndex  = 0
LArPhysWaveShifter.TimeShiftByLayer  = False
LArPhysWaveShifter.TshiftLayer       = [ 0 , 0 , 0 , 0 ]
LArPhysWaveShifter.TimeShiftFromPeak = False
LArPhysWaveShifter.NindexFromPeak    = 0 # if 0 will use Ndelays*Nsampling
LArPhysWaveShifter.Ndelays           = 24
LArPhysWaveShifter.Nsamplings        = 2
LArPhysWaveShifter.TimeShiftByFEB    = True
LArPhysWaveShifter.TimeShiftGuardRegion  = 5


topSequence += LArPhysWaveShifter

###########################################################################
if ( WriteNtuple ) :

   from LArCalibTools.LArCalibToolsConf import LArWaves2Ntuple
   LArPhysWaves2Ntuple = LArWaves2Ntuple("LArPhysWaves2Ntuple")
   LArPhysWaves2Ntuple.NtupleName = "PHYSWAVE" 
   LArPhysWaves2Ntuple.KeyList = [ OutputKey ] 
   LArPhysWaves2Ntuple.ReadCaliWave = False
   
   topSequence+=LArPhysWaves2Ntuple   
   
   theApp.HistogramPersistency = "ROOT"
   from GaudiSvc.GaudiSvcConf import NTupleSvc
   svcMgr += NTupleSvc()
   svcMgr.NTupleSvc.Output = [ "FILE1 DATAFILE='"+OutputPhysWaveRootFileDir+"/"+OutputPhysWaveRootFileName+"' OPT='NEW'" ]
   
if (  WritePoolFile ) :

   from RegistrationServices.OutputConditionsAlg import OutputConditionsAlg
   
   # writre PredPhysWave
   OutputConditionsAlgPhysWave = OutputConditionsAlg("OutputConditionsAlgPhysWave",OutputPhysWavePoolFileDir+"/"+OutputPhysWavePoolFileName,
                                                     [OutputObjectSpecPhysWave],[OutputObjectSpecTagPhysWave],WriteIOV)
   OutputConditionsAlgPhysWave.Run1     = IOVBegin
   if IOVEnd>0:
      OutputConditionsAlgPhysWave.Run2  = IOVEnd
		
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

svcMgr+=CfgMgr.AthenaEventLoopMgr(OutputLevel = DEBUG)

from AthenaCommon.AppMgr import theAuditorSvc
from AthenaCommon.ConfigurableDb import getConfigurable
theAuditorSvc += getConfigurable("MemStatAuditor")(OutputLevel = DEBUG)
theAuditorSvc += getConfigurable("ChronoAuditor")()
theAuditorSvc += getConfigurable("NameAuditor")()

###########################################################################


