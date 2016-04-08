#-------------------------------------------------------------
# Job options to read toy conditions
# Create the conditions first with AthExBeginRunWriteCond.py
#-------------------------------------------------------------

#--------------------------------------------------------------
# General job setup
#--------------------------------------------------------------
MessageSvc.Format = '% F%30W%S%7W%R%T %0W%M'

# Basic job configuration (for generator, provides McEventSelector)
import AthenaCommon.AtlasUnixGeneratorJob

# Max number of events
theApp.EvtMax = 30

# Here is the main trick to create "fake" run numbers
# McEventSelector will increase run number after EventsPerRun
svcMgr.EventSelector.RunNumber      = 1
svcMgr.EventSelector.EventsPerRun   = 2
svcMgr.EventSelector.FirstEvent     = 1

#--------------------------------------------------------------
# Private Application Configuration options
#--------------------------------------------------------------

# Full job is a list of algorithms
from AthenaCommon.AlgSequence import AlgSequence
job = AlgSequence()

# Add top algorithms to be run
from AthExBeginRun.AthExBeginRunConf import BeginRunAlg
job += BeginRunAlg( "BeginRunAlg",
                    Key = "/AthExBeginRun/AthExSimpleCond" )

# Import configurable for using our BeginRunTool
from AthExBeginRun.AthExBeginRunConf import BeginRunTool

# Setup a public tool and tell the algorithm to use it
ToolSvc += BeginRunTool( "BeginRunTool" )
job.BeginRunAlg.MyAlgTool = ToolSvc.BeginRunTool


#--------------------------------------------------------------
# Load POOL, IOVSvc and IOVDbSvc
#--------------------------------------------------------------
import AthenaPoolCnvSvc.AthenaPool

# Configure the DB connection and the folder
import IOVDbSvc.IOVDb
svcMgr.IOVDbSvc.dbConnection  = "sqlite://;schema=mytest.db;dbname=TESTCOOL"

# Use same folder tag as in AthExBeginRunWriteCond.py
tag = "COOL-TEST-1"   
svcMgr.IOVDbSvc.Folders = [ job.BeginRunAlg.Key+"<tag>" + tag + "</tag>" ]


# Print job configuration
print job
