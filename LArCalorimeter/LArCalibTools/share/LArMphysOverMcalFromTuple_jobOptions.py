# Last update: Caroline Collard 2009-03-31

IOVBegin=80000

import commands
###########################################################################
include("LArCalibProcessing/LArCalib_Flags.py")


# output POOL file:
if not 'writePOOLfile' in dir():
    writePOOLfile = True
    
if not 'MphysOverMcalPOOLFileOUT' in dir():
    MphysOverMcalPOOLFileOUT = "/tmp/collard/MphysOverMcal_88240_EMBC.pool.root"
    
###########################################################################
#
# Global settings
#
###########################################################################

include( "AthenaCommon/Atlas_Gen.UnixStandardJob.py" )

## get a handle to the default top-level algorithm sequence
from AthenaCommon.AlgSequence import AlgSequence 
topSequence = AlgSequence()

## get a handle to the ApplicationManager, to the ServiceManager and to the ToolSvc
from AthenaCommon.AppMgr import (theApp, ServiceMgr as svcMgr,ToolSvc)

include( "LArCalibProcessing/LArCalib_MinimalSetup.py")

svcMgr.IOVDbSvc.GlobalTag   = LArCalib_Flags.globalFlagDB

#########################################################################################################
#
# Reading Ntuple
#########################################################################################################
theApp.Dlls += ["LArRawUtils"]
theApp.Dlls += ["LArTools"]
theApp.Dlls += ["LArCalibUtils"]
theApp.Dlls += ["LArCalibTools"]

theApp.TopAlg += [ "LArMphysOverMcalFromTuple" ]
LArMphysOverMcalFromTuple = Algorithm( "LArMphysOverMcalFromTuple" )
LArMphysOverMcalFromTuple.FileNames = [
 "/tmp/collard/fpm_pulse_mini88240.root"
]
FPMKey = "LArMphysOverMcal"
LArMphysOverMcalFromTuple.StoreKey = FPMKey


###########################################################################
#
# writing to POOL file...
#
###########################################################################

if ( writePOOLfile ):
    
    FPMMphysFolder = "/LAR/ElecCalib/MphysOverMcal/FPM"
    OutputObjectSpec    = "LArMphysOverMcalComplete#"+FPMKey+"#"+FPMMphysFolder
    LArMphysFolderOutputTag = LArCalib_Flags.tagSuffix
    OutputObjectSpecTag = LArCalibFolderTag(FPMMphysFolder,LArMphysFolderOutputTag)
    
    from RegistrationServices.OutputConditionsAlg import OutputConditionsAlg
    theOutputConditionsAlg = OutputConditionsAlg("OutputConditionsAlg", MphysOverMcalPOOLFileOUT,
                                                 [OutputObjectSpec],[OutputObjectSpecTag],True)
    theOutputConditionsAlg.Run1     = IOVBegin


##########################################################################

theApp.EvtMax = 1

###########################################################################

svcMgr.MessageSvc.OutputLevel  = INFO
svcMgr.MessageSvc.defaultLimit = 10000
svcMgr.MessageSvc.Format       = "% F%20W%S%7W%R%T %0W%M"

svcMgr+=CfgMgr.AthenaEventLoopMgr(OutputLevel = INFO)

from AthenaCommon.AppMgr import theAuditorSvc
from AthenaCommon.ConfigurableDb import getConfigurable
theAuditorSvc += getConfigurable("MemStatAuditor")(OutputLevel = DEBUG)
theAuditorSvc += getConfigurable("ChronoAuditor")()
theAuditorSvc += getConfigurable("NameAuditor")()

###########################################################################

svcMgr.IOVDbSvc.dbConnection="sqlite://;schema=myDB200.db;dbname=COMP200"

from RegistrationServices.RegistrationServicesConf import IOVRegistrationSvc
svcMgr += IOVRegistrationSvc()
svcMgr.IOVRegistrationSvc.OutputLevel = DEBUG
svcMgr.IOVRegistrationSvc.RecreateFolders = True

###########################################################################

