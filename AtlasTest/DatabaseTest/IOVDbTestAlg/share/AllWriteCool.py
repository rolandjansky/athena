import AthenaCommon.AtlasUnixGeneratorJob
gbl.AthenaServices.SetFatalHandler(438)
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()
from AthenaCommon.AppMgr import ServiceMgr as svcMgr
from AthenaCommon.AppMgr import theApp

#import password for devdb10 from devdb10pwd.py
from devdb10_pwd import devdb10pwd

import IOVDbSvc.IOVDb

from IOVDbTestAlg.IOVDbTestAlgConf import IOVDbTestAlg
topSequence += IOVDbTestAlg( "IOVDbTestAlg" )

#--------------------------------------------------------------
# Set output level threshold (1=VERBOSE, 2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
#--------------------------------------------------------------
svcMgr.MessageSvc.OutputLevel = INFO
svcMgr.MessageSvc.Format      = "% F%30W%S%7W%R%T %0W%M"
IOVDbTestAlg.OutputLevel      = DEBUG
#svcMgr.IOVDbSvc.OutputLevel   = DEBUG
#svcMgr.IOVSvc.OutputLevel     = DEBUG


# IOVDbTestAlg.CreateExtraChanns=True

#--------------------------------------------------------------
IOVDbTestAlg.WriteCondObjs = True
IOVDbTestAlg.RegisterIOV   = True
IOVDbTestAlg.ReadWriteCool = True
IOVDbTestAlg.TagID         = "COOL-TEST-001"
IOVDbTestAlg.PrintLB       = True
IOVDbTestAlg.run           = 3
# in seconds - used for IOVDbTestAmdbCorrection
IOVDbTestAlg.RegTime       = 55

IOVDbTestAlg.run           = exRun
IOVDbTestAlg.RegTime       = exTime

try:
    svcMgr.IOVDbSvc.dbConnection  = "impl=cool;techno=oracle;devdb10:"
    svcMgr.IOVDbSvc.dbConnection  += TESTCOOL
    svcMgr.IOVDbSvc.dbConnection  += ":ATLAS_COOLTEST:%s" %devdb10pwd
except:
        svcMgr.IOVDbSvc.dbConnection  = "oracle://devdb10;dbname=TESTCOOL;schema=ATLAS_COOLTEST;user=ATLAS_COOLTEST;password=%s" %devdb10pwd

# uncomment this to use a local SQLite file instead
svcMgr.IOVDbSvc.dbConnection  = "sqlite://;schema=mytest.db;dbname=TESTCOOL"

#--------------------------------------------------------------
# REGISTRATION SERVICE - WITH THINGS COMMENTED CALCULATES IOV from RUN,EVENT NUMBERS.
import RegistrationServices.IOVRegistrationSvc
regSvc = svcMgr.IOVRegistrationSvc

# regSvc.IOVDbTag      = "DC1"
regSvc.RecreateFolders = False  # Select the following to delete and recreate the folders. Default is NOT to recreate
# The following set the interval for each of the IOVDbTest folders
# regSvc.BeginRun      = 4
# Max EndRun: 0x7FFFFFFF
# regSvc.EndRun        = 2147483647 
# regSvc.EndRun        = 3 
# regSvc.BeginLB       = 0
# Max EndLB: 0xFFFFFFFF
# regSvc.EndLB         = 4294967295



#--------------------------------------------------------------
# The following turns off the ability to set EventSelector parameters
# below
#ApplicationMgr.EvtSel = "NONE"
#--------------------------------------------------------------
# Set the following when reading back to adjust the run/event numbers
#   (don't forget ApplicationMgr.EvtMax)
svcMgr.EventSelector.RunNumber         = 1
svcMgr.EventSelector.EventsPerRun      = 10
svcMgr.EventSelector.FirstEvent        = 1
svcMgr.EventSelector.EventsPerLB       = 5
svcMgr.EventSelector.FirstLB           = 1
svcMgr.EventSelector.InitialTimeStamp  = 0
svcMgr.EventSelector.TimeStampInterval = 5
theApp.EvtMax                          = 100


#--------------------------------------------------------------
# Load POOL support
#--------------------------------------------------------------
import AthenaPoolCnvSvc.WriteAthenaPool

#--------------------------------------------------------------
# AthenaPool details
#--------------------------------------------------------------

# Check the dictionary in memory for completeness
svcMgr.AthenaSealSvc.CheckDictionary = TRUE

#--------------------------------------------------------------
#  Use AthenaOutputStreamTool to write
#    Must "turn off" standard AthenaOutputStream
#--------------------------------------------------------------

theApp.OutStream = []

from AthenaCommon.AppMgr import ToolSvc
IOVDbTestAlg.StreamName = "CondStream2"
from OutputStreamAthenaPool.OutputStreamAthenaPoolConf import AthenaPoolOutputStreamTool
ToolSvc += AthenaPoolOutputStreamTool("CondStream2",OutputFile = str(IOVDbTestAlg.run) + "-inf.root")
