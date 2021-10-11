# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

include("LArCalibProcessing/LArCalib_Flags.py")

shapeErrorFileName = os.path.join(os.getenv('TMPDIR'), 'Shape/corr_ALL.root')
outputTemplateFileName   = os.path.join(os.getenv('TMPDIR'), 'Shape/samples_template.root')
outputFileName     = os.path.join(os.getenv('TMPDIR'), 'Shape/larShape.root')

# RunNumber trigger IOV if reading from COOL is enabled
if not 'RunNumber' in dir():
   RunNumber = '0000000'

if not 'ChannelSelection' in dir():
   # Read all
   ChannelSelection = " "
   ## Example to read only cool for Barrel C : Strips->Back
   #ChannelSelection = "<channelSelection>0,35:66</channelSelection>" 

from string import *
def DBConnectionFile(sqlitefile):
   return "sqlite://;schema="+sqlitefile+";dbname=COMP200"

LArPhysFolderOutputTag = LArCalib_Flags.tagSuffix  

if not 'OutputDB' in dir():
   OutputDB = LArCalib_Flags.OutputDB

if 'OutputSQLiteFile' in dir():
   OutputDB = DBConnectionFile(OutputSQLiteFile)

if not 'OutputObjectSpecShape' in dir():
   OutputObjectSpecShape = "LArShapeComplete#LArResiduals#"+ LArCalib_Flags.LArShapeFolder+"5samples"		
   OutputObjectSpecTagShape  = LArCalibFolderTag(LArCalib_Flags.LArShapeFolder+"/Residuals/5samples",LArPhysFolderOutputTag)

WriteIOV = LArCalib_Flags.WriteIOV
IOVBegin = int(RunNumber)
IOVEnd = LArCalib_Flags.IOVEnd   

OutputShapePoolFileDir = os.getenv('TMPDIR')
OutputShapePoolFileName = "Shape/LArShapePhys.pool.root"


###########################################################################
# Print summary
###########################################################################

OFCLog = logging.getLogger( "OFCLog" )
OFCLog.info( " ======================================================== " )
OFCLog.info( " ***                 LAr OFC summary                  *** " )
OFCLog.info( " ======================================================== " )
OFCLog.info( " RunNumber                          = "+str(RunNumber) )
OFCLog.info( " OutputShapePoolFullFileName        = "+OutputShapePoolFileDir+"/"+OutputShapePoolFileName )
OFCLog.info( " OutputObjectSpecShape              = "+str(OutputObjectSpecShape) )
OFCLog.info( " IOVBegin                           = "+str(IOVBegin) )
OFCLog.info( " IOVEnd                             = "+str(IOVEnd) )
OFCLog.info( " LArCalibOutputDB                   = "+OutputDB )
OFCLog.info( " ======================================================== " )

###########################################################################
#
# Global settings
#
###########################################################################

## get a handle to the default top-level algorithm sequence
from AthenaCommon.AlgSequence import AlgSequence 
topSequence = AlgSequence()

## get a handle to the ApplicationManager, to the ServiceManager and to the ToolSvc
from AthenaCommon.AppMgr import (theApp, ServiceMgr as svcMgr,ToolSvc)

#include("LArCalibProcessing/LArCalib_MinimalSetup.py")

from LArSamplesMon.LArSamplesMonConfig import DefaultLArShapeCompleteMaker
lscm = DefaultLArShapeCompleteMaker('LArShapeCompleteMaker', shapeErrorFileName, outputFileName, outputTemplateFileName)
lscm.OutputLevel = DEBUG
topSequence += lscm


###########################################################################
from RegistrationServices.OutputConditionsAlg import OutputConditionsAlg

if os.path.exists(OutputShapePoolFileDir+"/"+OutputShapePoolFileName): 
  os.remove(OutputShapePoolFileDir+"/"+OutputShapePoolFileName)
OutputConditionsAlgShape = OutputConditionsAlg("OutputConditionsAlgShape",OutputShapePoolFileDir+"/"+OutputShapePoolFileName,
                                               [OutputObjectSpecShape],[OutputObjectSpecTagShape],WriteIOV)
OutputConditionsAlgShape.Run1     = IOVBegin
if IOVEnd>0:
  OutputConditionsAlgShape.Run2  = IOVEnd

svcMgr.IOVDbSvc.dbConnection  = OutputDB

svcMgr.IOVDbSvc.DBInstance=''
   
from RegistrationServices.RegistrationServicesConf import IOVRegistrationSvc
svcMgr += IOVRegistrationSvc()
svcMgr.IOVRegistrationSvc.OutputLevel = DEBUG
svcMgr.IOVRegistrationSvc.RecreateFolders = False
  		
###########################################################################
#               Use EventSelector to select IOV                           #
###########################################################################	

from McEventSelector.McEventSelectorConf import McEventSelector
svcMgr += McEventSelector("LArEventSelector")
svcMgr.LArEventSelector.RunNumber	= int(RunNumber)
svcMgr.LArEventSelector.EventsPerRun      = 1
svcMgr.LArEventSelector.FirstEvent	= 1
svcMgr.LArEventSelector.InitialTimeStamp  = 0
svcMgr.LArEventSelector.TimeStampInterval = 1

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

svcMgr.MessageSvc.enableSuppression = False

###########################################################################
