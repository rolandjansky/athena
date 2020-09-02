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
DetDescrVersion = "ATLAS-DC3-02"
include( "AthenaCommon/Atlas_Gen.UnixStandardJob.py" )

from AthenaCommon.DetFlags import DetFlags
DetFlags.Calo_setOn()
DetFlags.ID_setOff()
DetFlags.Muon_setOff()
DetFlags.Truth_setOff()
DetFlags.LVL1_setOff()
DetFlags.digitize.all_setOff()
# Other LAr related
# setup GeoModel
include( "AtlasGeoModel/SetGeometryVersion.py" )
include( "AtlasGeoModel/GeoModelInit.py" )

# include( "LArDetDescr/LArDetDescr_joboptions.py" )

include( "LArIdCnv/LArIdCnv_joboptions.py" )
include( "CaloDetMgrDetDescrCnv/CaloDetMgrDetDescrCnv_joboptions.py" )
include( "IdDictDetDescrCnv/IdDictDetDescrCnv_joboptions.py" )
#include( "LArCondCnv/LArCondCnv_G4_jobOptions.py" )
include( "LArCondCnv/LArCondCnv_IdMapAtlas_jobOptions.py" )

#--------------------------------------------------------------
# Access to IOVSvc, IOVDbSvc and CondDBMySQLCnvSvc
#--------------------------------------------------------------
include ( "IOVDbSvc/IOVDbSvc_jobOptions.py" )

#--------------------------------------------------------------
# Private Application Configuration options
#--------------------------------------------------------------
#load relevant libraries
#theApp.Dlls += [ "IOVDbTestAlg" ]
theApp.Dlls += [ "LArTools" ]
theApp.Dlls += [ "LArCondUtils" ]
theApp.Dlls += [ "LArRawUtils" ]
theApp.Dlls += [ "IdDictTest" ]
theApp.Dlls += [ "LArConditionsTest" ]
include( "LArCondAthenaPool/LArCondAthenaPool_joboptions.py" )
include ("LArRawConditions/LArRawConditionsDict_joboptions.py")

#theApp.TopAlg+=["IOVDbTestCoolDCS"]
#IOVDbTestCoolDCS=Service("IOVDbTestCoolDCS")

# set the list of folders to be read as AthenaAttributeList
# IOVDbTestCoolDCS.AttrListFolders=["/dcs/temp","/dcs/temp2"]
# set the list of folders to be read as CondAttrListCollection
#IOVDbTestCoolDCS.AttrListCollFolders=["/LAR/DCS/CLVTEMP"]

# set all the folders to be read in by IOVDbSvc
IOVDbSvc=Service("IOVDbSvc")
IOVDbSvc.Folders += [ "/LAR/DCS/CLVTEMP" ]

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
EventSelector.InitialTimeStamp  = 1154510000
# increment of 3 minutes
EventSelector.TimeStampInterval = 180
theApp.EvtMax                   = 30
#
#--------------------------------------------------------------
# Load POOL support
#--------------------------------------------------------------
include("AthenaPoolCnvSvc/AthenaPool_jobOptions.py")

theApp.TopAlg+=["LArFecLvTempDcsTest"]
LArFecLvTempDcsTest = Algorithm ("LArFecLvTempDcsTest")
LArFecLvTempDcsTest.OutputLevel = DEBUG

ToolSvc = Service ( "ToolSvc" )
ToolSvc.LArFecLvTempDcsTool.OutputLevel = DEBUG

MessageSvc = Service ("MessageSvc")
MessageSvc.OutputLevel = FATAL
MessageSvc.defaultLimit = 999999

# include ("IOVDbSvc/IOVRecExCommon.py")
