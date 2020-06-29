###############################################################
#
# Job options file for LArHVCommissionTest 
# read channels in HV folders containing DCS data saved
# to COOL using the PVSS->COOL translation tools from Jim Cook
# and print it.
#
# NB: This joboption will NOT work unedited - you must set the
# database connection string to your server, and the folders
# to the folders you want to use
#
#==============================================================

include( "AthenaCommon/Atlas_Gen.UnixStandardJob.py" )

# include( "LArDetDescr/LArDetDescr_joboptions.py" )

include( "LArIdCnv/LArIdCnv_joboptions.py" )

#--------------------------------------------------------------
# Access to IOVSvc, IOVDbSvc and CondDBMySQLCnvSvc
#--------------------------------------------------------------
include ( "IOVDbSvc/IOVDbSvc_jobOptions.py" )

#--------------------------------------------------------------
# Private Application Configuration options
#--------------------------------------------------------------
#load relevant libraries
theApp.Dlls += [ "LArTools" ]
theApp.Dlls += [ "LArCondUtils" ]
theApp.Dlls += [ "IdDictTest" ]
theApp.Dlls += [ "LArConditionsTest" ]
include( "LArCondAthenaPool/LArCondAthenaPool_joboptions.py" )


# set all the folders to be read in by IOVDbSvc
IOVDbSvc=Service("IOVDbSvc")
IOVDbSvc.Folders += [ "/LAR/DCS/HV/BARREL/I16" ]

#--------------------------------------------------------------
#   COOL ConditionsDB connection
#
IOVDbSvc.OutputLevel        = DEBUG

# set the connection string to the database you want to use
#IOVDbSvc.dbConnection  = "impl=cool;techno=oracle;devdb10:TESTCOOL:ATLAS_COOLTEST:cool4devdb"
IOVDbSvc.dbConnection  = "impl=cool;techno=oracle;schema=ATLAS_COOL_LAR;ATLAS_COOLPROD:DCSP130:ATLAS_COOL_READER:COOLRED4PRO"

#--------------------------------------------------------------
# Set the following when reading back to adjust the run/event numbers
# and timestamps
EventSelector = Service( "EventSelector" )
EventSelector.RunNumber         = 1
EventSelector.EventsPerRun      = 9999
EventSelector.FirstEvent        = 1
# initial time stamp - this is number of seconds since 1st Jan 1970 GMT
# the value given here corresponds to 07:15 GMT on Tuesday 13th Sept 2005
#EventSelector.InitialTimeStamp  = 1126595520
EventSelector.InitialTimeStamp  = 1174924410 #1160666211
# increment of 3 minutes
EventSelector.TimeStampInterval = 10 #180
theApp.EvtMax                   = 1  #30
#
#--------------------------------------------------------------
# Load POOL support
#--------------------------------------------------------------
#include("AthenaPoolCnvSvc/AthenaPool_jobOptions.py")
theApp.Dlls   += ["PoolSvc","AthenaPoolCnvSvc","AthenaRootStreamerSvc"]
EventPersistencySvc = Service( "EventPersistencySvc" )
EventPersistencySvc.CnvServices     += [ "AthenaPoolCnvSvc" ]

theApp.TopAlg+=["LArHVCommissionTest"]
LArHVCommissionTest = Algorithm ("LArHVCommissionTest")
LArHVCommissionTest.OutputLevel = DEBUG
LArHVCommissionTest.LArHVTools = [ "LArHVCommissionTool" ]

ToolSvc = Service ( "ToolSvc" )
ToolSvc.LArHVCommissionTool.OutputLevel = DEBUG

MessageSvc = Service ("MessageSvc")
MessageSvc.OutputLevel = FATAL
MessageSvc.defaultLimit = 999999

