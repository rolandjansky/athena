###############################################################
#
# Job options file for IOVDbTestCoolDCS
# read in one or several folders containing DCS data saved
# to COOL using the PVSS->COOL translation tools from Jim Cook
# and print it using the  IOVDbTestCoolDCS algorithm in IOVDbTestAlg
#
# NB: This joboption will NOT work unedited - you must set the
# database connection string to your server, and the folders
# to the folders you want to use
#
#==============================================================
#use McEventSelector
import AthenaCommon.AtlasUnixGeneratorJob

## get a handle to the default top-level algorithm sequence
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

## get a handle to the ServiceManager
from AthenaCommon.AppMgr import ServiceMgr as svcMgr

## get a handle to the ApplicationManager
from AthenaCommon.AppMgr import theApp

#--------------------------------------------------------------
# Load POOL support - to read conditions in POOL, not events
#--------------------------------------------------------------
import AthenaPoolCnvSvc.AthenaPool

#--------------------------------------------------------------
# Access to IOVSvc, IOVDbSvc and CondDBMySQLCnvSvc
#--------------------------------------------------------------
import IOVDbSvc.IOVDb

#--------------------------------------------------------------
# Private Application Configuration options
#--------------------------------------------------------------

# Load "user algorithm" top algorithms to be run, and the libraries that house them
from IOVDbTestAlg.IOVDbTestAlgConf import IOVDbTestCoolDCS
topSequence += IOVDbTestCoolDCS( "IOVDbTestCoolDCS" )

# set the list of folders to be read as AthenaAttributeList
# IOVDbTestCoolDCS.AttrListFolders=[]
# set the list of folders to be read as CondAttrListCollection
IOVDbTestCoolDCS.AttrListCollFolders=["/TRT/DCS/HV/BARREL","/TRT/DCS/HV/ENDCAPA"]

# set all the folders to be read in by IOVDbSvc
svcMgr.IOVDbSvc.Folders += ["/TRT/DCS/HV/BARREL","/TRT/DCS/HV/ENDCAPA"]

#--------------------------------------------------------------
#   COOL ConditionsDB connection
#
svcMgr.IOVDbSvc.OutputLevel        = INFO

# set the connection string to the database you want to use
svcMgr.IOVDbSvc.dbConnection  = "COOLOFL_DCS/COMP200"

#--------------------------------------------------------------
# Set the following when reading back to adjust the run/event numbers
# and timestamps
svcMgr.EventSelector.RunNumber         = 1
svcMgr.EventSelector.EventsPerRun      = 9999
svcMgr.EventSelector.FirstEvent        = 1
# initial time stamp - this is number of seconds since 1st Jan 1970 GMT
# the value given here corresponds to 00:00 GMT on 1st October 2008
svcMgr.EventSelector.InitialTimeStamp  = 1222819200
# increment of 1 minute
svcMgr.EventSelector.TimeStampInterval = 60
theApp.EvtMax                   = 30

