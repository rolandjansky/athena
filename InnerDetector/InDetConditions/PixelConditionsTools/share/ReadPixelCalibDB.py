# Use auditors
theApp.Dlls += [ "GaudiAud" ]
#
# write out a summary of the time spent
#
theAuditorSvc = AuditorSvc()
theAuditorSvc.Auditors  += [ "ChronoAuditor"]
#
# write out a short message upon entering or leaving each algorithm
#
theAuditorSvc.Auditors  += [ "NameAuditor" ]
ChronoStatSvc = Service ( "ChronoStatSvc")
theAuditorSvc.Auditors  += [ "MemStatAuditor" ]
MemStatAuditor = theAuditorSvc.auditor( "MemStatAuditor" )
theApp.AuditAlgorithms=True
MemStatAuditor.OutputLevel = INFO

#use McEventSelector
include( "AthenaCommon/Atlas_Gen.UnixStandardJob.py" )

from AthenaCommon.GlobalFlags import GlobalFlags
# --- default is atlas geometry
GlobalFlags.DetGeo.set_atlas()
# --- set defaults
GlobalFlags.DataSource.set_geant4()    
GlobalFlags.InputFormat.set_pool()    
# --- default is zero luminosity
GlobalFlags.Luminosity.set_zero()
GlobalFlags.Print()
#--------------------------------------------------------------
# Get Configuration flags
#--------------------------------------------------------------
include( "AthenaCommon/AthenaCommonFlags.py" )

#--------------------------------------------------------------
# Set Detector setup
#--------------------------------------------------------------
# --- switch on InnerDetector
from AthenaCommon.DetFlags import DetFlags 
DetFlags.ID_setOn()
DetFlags.Calo_setOff()
DetFlags.Muon_setOff()
DetFlags.Truth_setOff()
DetFlags.LVL1_setOff()
DetFlags.SCT_setOff()
DetFlags.TRT_setOff()

# ---- switch parts of ID off/on as follows
#switch off tasks
DetFlags.pileup.all_setOff()
DetFlags.simulate.all_setOff()
DetFlags.makeRIO.all_setOff()
DetFlags.writeBS.all_setOff()
DetFlags.readRDOBS.all_setOff()
DetFlags.readRIOBS.all_setOff()
DetFlags.readRIOPool.all_setOff()
DetFlags.writeRIOPool.all_setOff()

#--- IODbSvc --------------------------------------------------
# must be before GeoModel
include("IOVDbSvc/IOVRecExCommon.py")
IOVDbSvc = Service( "IOVDbSvc" )

DetDescrVersion = "ATLAS-DC3-05"
# This is a interim solution until it possible to override folder tags.
# This job option fragment needs to included before
# AtlasGeoModel/GeoModelInit.py is included.
# Block the defaults.
include.block("PixelConditionsTools/InDetAlignable.py") 
##
include ("AtlasGeoModel/SetGeometryVersion.py")
include ("AtlasGeoModel/GeoModelInit.py")
#--------------------------------------------------------------
# Access to IOVSvc, IOVDbSvc and CondDBMySQLCnvSvc
#--------------------------------------------------------------
#include ( "IOVDbSvc/IOVDbSvc_jobOptions.py" )

#--------------------------------------------------------------
# Private Application Configuration options
#--------------------------------------------------------------
#theApp.Dlls +=[ "PixelConditionsServices" ]

#configure PixelCalibDbSvc
#theApp.ExtSvc +=[ "PixelCalibSvc" ]
#PixelCalibSvc = Service( "PixelCalibSvc" )

#PixelCalibSvc.DBToolType = "PixelCalib::PixelCalibDbTool"
#PixelCalibSvc.DBToolName = "PixelCalib_PixelCalibDbTool"

theApp.Dlls += [ "PixelConditionsTools" ]

# PixelCalibDbTestWrite.py
# demonstrates writing of a file into a common CLOB folder in COOL
# the COOL folder must be created BEFOREHAND using make_coolstrfile.py
# see the mainpage.h doc for more details
# define two instances of the Read/WritePixelCalibDB algorithm to manipulate
# the data files

theApp.Dlls+=["DetDescrCondTools","DetDescrCondExample"]

theApp.TopAlg+=["PixelCalibDbTestWriteRead"]

# calibation data into /PIXEL/PixCalib
PixelCalibDbTestWriteRead =Algorithm("PixelCalibDbTestWriteRead")
# this does writing/reading
PixelCalibDbTestWriteRead.Write = False
PixelCalibDbTestWriteRead.Read = True
PixelCalibDbTestWriteRead.OutputTextFile ="pixelcalib_dump.txt"
# setup for ToolSvc
ToolSvc = Service( "ToolSvc" )

#IOVDbSvc.OutputLevel = DEBUG
#IOVDbSvc.Folders+=[CondDBCool.PIXEL + "/PIXEL/PixCalib" + "<tag>PixCalib-Assembly-01</tag>"]

from IOVDbSvc.CondDB import conddb
conddb.addFolder("PIXEL","/PIXEL/PixCalib <tag>PixCalib-Assembly-01</tag>")

# Options for IOVRegistrationSvc
#include( "RegistrationServices/IOVRegistrationSvc_jobOptions.py" )
#regSvc = Service( "IOVRegistrationSvc" )
# setup PixelCalibDbTool in ToolSvc
PixelCalibDbTool = Service("ToolSvc.PixelCalibDbTool")
PixelCalibDbTool.CalibFolder ="/PIXEL/PixCalib"
PixelCalibDbTool.CalibLocation = "PixCalibKey"
PixelCalibDbTool.WriteDB =False
# Set the following when reading back to adjust the run/event numbers
#EventSelector = Service( "EventSelector" )
#EventSelector.RunNumber         = 1
#EventSelector.EventsPerRun      = 5
#EventSelector.FirstEvent        = 1
theApp.EvtMax = 5

#--------------------------------------------------------------
# Set output level threshold (1=VERBOSE, 2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
#--------------------------------------------------------------
MessageSvc.OutputLevel = DEBUG
# --- change output format
MessageSvc.Format       = "% F%30W%S%7W%R%T %0W%M"
MessageSvc.defaultLimit = 9999999
