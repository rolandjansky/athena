# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

import commands

###########################################################################
#
# <Marco.Delmastro@cern.ch>
# <Nicolas.Berger@cern.ch>
#
# Store LArShapeComplete object for residuals in a POOL file
#
###########################################################################

include("RecJobTransforms/UseOracle.py")
include("LArCalibProcessing/LArCalib_Flags.py")
#include("LArConditionsCommon/LArIdMap_comm_jobOptions.py")

###########################################################################
#               Input selection (Autocorr, PhysWave)
###########################################################################

shapeErrorFileName = os.path.join(os.getenv('TMPDIR'), 'Shape/corr_ALL.root')
outputTemplateFileName   = os.path.join(os.getenv('TMPDIR'), 'Shape/samples_template.root')
outputFileName     = os.path.join(os.getenv('TMPDIR'), 'Shape/larShape.root')

# RunNumber trigger IOV if reading from COOL is eneabled
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

print dir(LArCalib_Flags)

#njpb LArPhysFolderOutputTag = LArCalib_Flags.tagSuffix  
LArPhysFolderOutputTag = '-UPD3-01'

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

include("AthenaCommon/Atlas_Gen.UnixStandardJob.py")

## get a handle to the default top-level algorithm sequence
from AthenaCommon.AlgSequence import AlgSequence 
topSequence = AlgSequence()

## get a handle to the ApplicationManager, to the ServiceManager and to the ToolSvc
from AthenaCommon.AppMgr import (theApp, ServiceMgr as svcMgr,ToolSvc)

include("LArCalibProcessing/LArCalib_MinimalSetup.py")

#from AthenaCommon.DetFlags import DetFlags
#DetFlags.Calo_setOn()

from LArSamplesMon.LArSamplesMonConfig import DefaultLArShapeCompleteMaker
lscm = DefaultLArShapeCompleteMaker('LArShapeCompleteMaker', shapeErrorFileName, outputFileName, outputTemplateFileName)
lscm.OutputLevel = DEBUG
topSequence += lscm

###########################################################################
#                                                                         #
#    Input conditions data (AutoCorr, CaliWave) for COOL and/or POOL      #
#                                                                         #
###########################################################################

include("AthenaPoolCnvSvc/AthenaPool_jobOptions.py")
include("LArCondAthenaPool/LArCondAthenaPool_joboptions.py")

from IOVDbSvc.CondDB import conddb

## define the DB Gobal Tag :
#svcMgr.IOVDbSvc.GlobalTag = LArCalib_Flags.globalFlagDB   
svcMgr.IOVDbSvc.GlobalTag = 'COMCOND-REPPT-008-00' 
#svcMgr.IOVDbSvc.GlobalTag = 'COMCOND-ES1PT-005-04'
#svcMgr.IOVDbSvc.GlobalTag = 'COMCOND-BLKPST-005-07'

###########################################################################
from RegistrationServices.OutputConditionsAlg import OutputConditionsAlg

if os.path.exists(OutputShapePoolFileDir+"/"+OutputShapePoolFileName): 
  os.remove(OutputShapePoolFileDir+"/"+OutputShapePoolFileName)
OutputConditionsAlgShape = OutputConditionsAlg("OutputConditionsAlgShape",OutputShapePoolFileDir+"/"+OutputShapePoolFileName,
                                               [OutputObjectSpecShape],[OutputObjectSpecTagShape],WriteIOV)
OutputConditionsAlgShape.Run1 = IOVBegin
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

svcMgr.MessageSvc.enableSuppression = False

###########################################################################
