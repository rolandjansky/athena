#-------------------------------------------------------------
# Job options to create toy conditions
# Use AthExBeginRunOptions.py to read them back
#-------------------------------------------------------------
MessageSvc.Format = '% F%30W%S%7W%R%T %0W%M'

#--------------------------------------------------------------
# Private Application Configuration options
#--------------------------------------------------------------

# Full job is a list of algorithms
from AthenaCommon.AlgSequence import AlgSequence
job = AlgSequence()

# Add top algorithms to be run
from AthExBeginRun.AthExBeginRunConf import AthExCreateCondAlg
job += AthExCreateCondAlg( "AthExCreateCondAlg",
                           Key = "/AthExBeginRun/AthExSimpleCond",
                           Tag = "COOL-TEST-1" )                           

# This defines the IOVs for the conditions objects. For example:
# [1, 5, 11] means 3 objects are created with the following IOV run ranges:
#   1) IOV = [1,4]
#   2) IOV = [5,10]
#   3) IOV = [11,infinity)
job.AthExCreateCondAlg.IovRuns = [1, 4, 8, 13]


#--------------------------------------------------------------
# Load POOL, IOVSvc, etc.
#--------------------------------------------------------------

# POOL for reading/writing conditions
import AthenaPoolCnvSvc.AthenaPool

# To register conditions in IOV DB
import RegistrationServices.IOVRegistrationSvc

# Configure the DB connection
import IOVDbSvc.IOVDb
svcMgr.IOVDbSvc.dbConnection  = "sqlite://;schema=mytest.db;dbname=TESTCOOL"

# Only want to create conditions once
theApp.EvtMax = 1

# Define output stream for POOL file
theApp.OutStream = []
from AthenaCommon.AppMgr import ToolSvc
from OutputStreamAthenaPool.OutputStreamAthenaPoolConf import AthenaPoolOutputStreamTool
ToolSvc += AthenaPoolOutputStreamTool("AthExCondStream", OutputFile = "AthExCondStream.root")

# Print job configuration
print job

