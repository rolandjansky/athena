##############################################################
#
# Job options file for writing TRTStrawStatusTool bad channels
# See doc in README
#
#==============================================================
#--------------------------------------------------------------
# Private Application Configuration options
#--------------------------------------------------------------
include ( "IOVDbSvc/IOVDbSvc_jobOptions.py" )
#load relevant libraries
theApp.Dlls += [ "IOVSvc", "IOVDbSvc"]
theApp.ExtSvc += [ "IOVSvc" ]
ProxyProviderSvc = Service( "ProxyProviderSvc" )
ProxyProviderSvc.ProviderNames += [ "IOVDbSvc" ]

#AlgolibTRT_ConditionsAlgs.so
theApp.Dlls += [ "TRT_ConditionsAlgs" ]
#converters
include( "InDetConditionsAthenaPool/InDetConditionsAthenaPool_joboptions.py" )

theApp.TopAlg = [ "TRTStrawStatusWrite" ]

#--------------------------------------------------------------
# Set output level threshold (1=VERBOSE, 2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
#--------------------------------------------------------------
MessageSvc = Service( "MessageSvc" )
MessageSvc.OutputLevel      = 2
TRTStrawStatusWrite = Algorithm( "TRTStrawStatusWrite" )
TRTStrawStatusWrite.OutputLevel      = 2
IOVDbSvc = Service( "IOVDbSvc" )
IOVDbSvc.OutputLevel      = 2
#StoreGateSvc.OutputLevel    = 2;
#--------------------------------------------------------------

# job options for FillIOVTool
include ( "IOVDbTools/IOVDbTools_jobOptions.py" )

# job option to load the converters for the AlignableTransform object
#include ( "DetDescrCondAthenaPool/DetDescrCondAthenaPool_joboptions.py" )

#uncomment below
include ("RDBAccessSvc/RDBAccessSvcDevdb_jobOptions.py")
include( "NovaCnvSvc/NovaCnvSvc_jobOptions.py" )
include( "NovaConverters/NovaConverters_jobOptions.py" )
theApp.Dlls += [ "GeoModelSvc" ]
theApp.ExtSvc += [ "GeoModelSvc"]
include ("AtlasGeoModel/InDetGeoModelCTB.py")
GeoModelSvc.InDetVersionOverride = "InnerDetector-CTB-04"

#--------------------------------------------------------------
# Set output level threshold (1=VERBOSE, 2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
#--------------------------------------------------------------
MessageSvc = Service( "MessageSvc" )
MessageSvc.OutputLevel      = 1
IOVDbSvc = Service( "IOVDbSvc" )
IOVDbSvc.OutputLevel      = 1
#StoreGateSvc.OutputLevel    = 1;
#--------------------------------------------------------------
# For IOVDb: set database name, and tag for data to be read
#
# Test server

#IOVDbSvc.ServerName  = "atlasdbdev.cern.ch"
#IOVDbSvc.DBname = "ConditionsDB_AA"
#IOVDbSvc.userName    = "conddbdev"
#IOVDbSvc.userPwd     = "pwd4conddb"


#IOVDbSvc.Folders += [ "/trt/TRTStrawStatusData" ]

# production database:

#IOVDbSvc.ServerName = "atlobk01.cern.ch"
#IOVDbSvc.DBname     = "conditions_ctb_2004"
#IOVDbSvc.userName   = "trt"
#IOVDbSvc.userPwd    = "xxxxxxxx"


IOVDbSvc.ServerName = "atlmysql01.cern.ch"
IOVDbSvc.DBname     = "conditions_ctb_2004"
IOVDbSvc.userName   = "condwriter"
IOVDbSvc.userPwd    = "xxxxxxxxxx"



#
#--------------------------------------------------------------
# Choose the following to fill an IOV DB. Otherwise, one
# simply reads back from the IOV DB.
#IOVDbTestAlg.FillIOV = TRUE
#--------------------------------------------------------------
# Options for FillIOVTool



# Set the tag for ALL folders (not yet able to have different tags per folder)
ToolSvc = Service( "ToolSvc" )

#control the alg via the tool svs
ToolSvc.TRTStrawStatusTool.Readjob = False
ToolSvc.TRTStrawStatusTool.Writejob = True
if ToolSvc.TRTStrawStatusTool.Writejob:
	ToolSvc.TRTStrawStatusTool.InputTRTBadChanFile="TRTBadChan_run2102519.txt";

# Select the following to delete and recreate the folders. Default is
# NOT to recreate
#
#ToolSvc.FillIOVTool.RecreateFolders = TRUE;

# The following set the interval for each of the IOVDbTest folders
ToolSvc.FillIOVTool.BeginRun   = 2102519;
#Max EndRun: 0x7FFFFFFF
ToolSvc.FillIOVTool.EndRun     = 2102519;
ToolSvc.FillIOVTool.BeginEvent = 0;
#Max EndEvent: 0xFFFFFFFF
#ToolSvc.FillIOVTool.EndEvent   = 4;
ToolSvc.FillIOVTool.EndEvent   = 99999999;
ToolSvc.FillIOVTool.IOVDbTag   = "TRTBadChan_1";

#
#--------------------------------------------------------------
# The following turns off the ability to set EventSelector parameters
# below
#ApplicationMgr.EvtSel = "NONE";
# Number of events to be processed (default is 10)
#ApplicationMgr.EvtMax = 1;
#--------------------------------------------------------------
# Set the following when reading back to adjust the run/event numbers
#   (don't forget ApplicationMgr.EvtMax)
EventSelector = Service( "EventSelector" )
#EventSelector.RunNumber=1
#EventSelector.EventsPerRun=5
#EventSelector.FirstEvent=1
theApp.EvtMax = 2
#--------------------------------------------------------------
# Load POOL support
#--------------------------------------------------------------
include( "AthenaPoolCnvSvc/WriteAthenaPool_jobOptions.py" )

#--------------------------------------------------------------
# Define the output Db parameters (the default value are shown)
#--------------------------------------------------------------
# *** for now output is to local dir ***
#
PoolSvc = Service( "PoolSvc" )
#PoolSvc.Output    = "TRT_TB2004_BadChannels.root"
#
PoolSvc.Collection = "NewPoolTry"
#PoolSvc.DbOpen = "update"; # "read", "update" "create" "overwrite"
#PoolSvc.DbOpen = "update";
PoolSvc.DbOpen = "update";
#careful to use update so we don't overwrite everything!
PoolSvc.WriteCatalog="file:PFC_2102519.xml"
#
#--------------------------------------------------------------
#  List each object to be written out
#    We use a separate stream2 for writing out conditions data
#--------------------------------------------------------------
theApp.OutStream     =["Stream2"]; 
theApp.OutStreamType ="AthenaOutputStream"; 
Stream2 = Algorithm( "Stream2" )
#Stream2 = Service( "Stream2" )
Stream2.EvtConversionSvc     ="AthenaPoolCnvSvc"; 
Stream2.StoreName            ="DetectorStore"; 
Stream2.OutputFile    = "TRT_TB2004_BadChannels_run2102519.root"
# Use IOVDbTestAlg as veto 
#   - default has filter passed == true i.e. don't write
Stream2.VetoAlgs             = ["TRTStrawStatusWrite"]; 
# the chan stat object
#Stream2.ItemList+=["264834577#*"]
Stream2.ItemList+=["247680275#*"]


