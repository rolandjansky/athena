# Use auditors
import AthenaCommon.AtlasUnixStandardJob
from AthenaCommon.AppMgr import theApp
from AthenaCommon.AppMgr import ServiceMgr

from GaudiSvc.GaudiSvcConf import AuditorSvc

ServiceMgr += AuditorSvc()
theAuditorSvc = ServiceMgr.AuditorSvc
theAuditorSvc.Auditors  += [ "ChronoAuditor"]
#ChronoStatSvc = Service ( "ChronoStatSvc")
theAuditorSvc.Auditors  += [ "MemStatAuditor" ]
#MemStatAuditor = theAuditorSvc.auditor( "MemStatAuditor" )
theApp.AuditAlgorithms=True
#theApp.Dlls += [ "GaudiAud" ]
#
# write out a summary of the time spent
#
#theAuditorSvc = AuditorSvc()
#theAuditorSvc.Auditors  += [ "ChronoAuditor"]
#
# write out a short message upon entering or leaving each algorithm
#
#theAuditorSvc.Auditors  += [ "NameAuditor" ]
#ChronoStatSvc = Service ( "ChronoStatSvc")
#theAuditorSvc.Auditors  += [ "MemStatAuditor" ]
#MemStatAuditor = theAuditorSvc.auditor( "MemStatAuditor" )
#theApp.AuditAlgorithms=True
#MemStatAuditor.OutputLevel = INFO
                                                                                
#use McEventSelector
#include( "AthenaCommon/Atlas_Gen.UnixStandardJob.py" )

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
#include( "AthenaCommon/AthenaCommonFlags.py" )

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

#include ("AtlasGeoModel/SetGeometryVersion.py")
import AtlasGeoModel.SetGeometryVersion
#include ("AtlasGeoModel/GeoModelInit.py")
import AtlasGeoModel.GeoModelInit

from AthenaCommon.AlgSequence import AlgSequence
job = AlgSequence()
from AthenaCommon.AppMgr import ToolSvc

#--- IODbSvc --------------------------------------------------
# must be before GeoModel
#include("IOVDbSvc/IOVRecExCommon.py")
#IOVDbSvc = Service( "IOVDbSvc" )

#DetDescrVersion = "ATLAS-DC3-05"
# This is a interim solution until it possible to override folder tags.
# This job option fragment needs to included before
# AtlasGeoModel/GeoModelInit.py is included.
# Block the defaults.
#include.block("InDetAlignExample/InDetAlignable.py") 
##
#include ("AtlasGeoModel/SetGeometryVersion.py")
#include ("AtlasGeoModel/GeoModelInit.py")
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

from PixelConditionsTools.PixelConditionsToolsConf import PixelCalibDbTool
ToolSvc +=PixelCalibDbTool()


#theApp.Dlls += [ "PixelConditionsTools" ]

# PixelCalibDbTestWrite.py
# demonstrates writing of a file into a common CLOB folder in COOL
# the COOL folder must be created BEFOREHAND using make_coolstrfile.py
# see the mainpage.h doc for more details
# define two instances of the Read/WritePixelCalibDB algorithm to manipulate
# the data files

#theApp.DLLs+=["DetDescrCondTools","DetDescrCondExample"]

#theApp.TopAlg+=["PixelCalibDbTestWriteRead/PixelDbWrite"]

from PixelConditionsTools.PixelConditionsToolsConf import PixelCalibDbTestWriteRead

job +=PixelCalibDbTestWriteRead(Write = True,
                                Read = False,
                                InputTextFile ="pcd_p1.dat",
                                OutputTextFile ="pixelcalib_dump.txt")

# calibation data into /PIXEL/PixCalib
#PixelDbWrite =Algorithm("PixelDbWrite")
# this does writing/reading
#PixelDbWrite.Write = True
#PixelDbWrite.Read = False
#PixelDbWrite.InputTextFile ="CalibrationData.txt"
#PixelDbWrite.OutputTextFile ="pixelcalib_dump.txt"
#ToolSvc = Service( "ToolSvc" )
# data is written to conditions database using OutputConditionsAlg
# include basic setup

#include("RegistrationServices/python/OutputConditionsAlg.py")

from RegistrationServices.OutputConditionsAlg import OutputConditionsAlg
myOCA=OutputConditionsAlg(outputFile="dummy.root")
# set options specific to this use of OutputConditionsAlg 
# in the case of PixelCalibDbTool, which uses COOL inline payload, the
# output POOL file contains nothing useful and  can be thrown away

#ToolSvc.ConditionsAlgStream.OutputFile="dummy.root" 

# list of things to be written - since we are using two channels in the
# same folder, only one write operation is needed for everything
#
myOCA.ObjectList=[ "CondAttrListCollection#/PIXEL/PixCalib" ]
myOCA.WriteIOV=True
myOCA.Run2=96537
#myOCA.Run1=96538

# set the interval of validity for the file here
# putting nothing falls back on default which is to be valid for all run/event
#OutputConditionsAlg.Run1=0
#OutputConditionsAlg.Event1=0
#OutputConditionsAlg.Run2=99999
#OutputConditionsAlg.Event2=99999
################
# specify a conditions DB tag here (or omit for HEAD tag only)
#myOCA.IOVTagList=["PixCalib-Assembly-02"] 
#myOCA.IOVTagList=["PixCalib-Assembly-00-05"]
myOCA.IOVTagList=["PixCalib-002-00"]
### configure IOVRegistrationSvc for writing of CLOBs

from RegistrationServices.RegistrationServicesConf import IOVRegistrationSvc
ServiceMgr += IOVRegistrationSvc()
regSvc = ServiceMgr.IOVRegistrationSvc

regSvc.OverrideNames = [ "data" ]
regSvc.OverrideTypes = [ "String4k" ]

### set up IOVDbSvc

#include ( "IOVDbSvc/CondDBConfig.py" )
#IOVDbSvc=Service("IOVDbSvc")

from IOVDbSvc.IOVDbSvcConf import IOVDbSvc
IOVDbSvc.GlobalTag = "DEFAULTCOND"

################
# this example is for a local SQlite file mysqlfile.db, COOL DB named COOLTEST
IOVDbSvc.dbConnection = "sqlite://X;schema=mysqlfile_p1.db;dbname=OFLP200"
#IOVDbSvc.dbConnection ="impl=cool;techno=sqlite;schema=mysqlfile.db;X:OFLP200"
#IOVDbSvc.dbConnection ="impl=cool;techno=sqlite;schema=mysqlfile.db;X:CMCP200"
#IOVDbSvc.dbConnection="impl=cool;techno=oracle;schema=ATLAS_COOL_PIXEL;ATLAS_COOLPROD:COMP130:ATLAS_COOL_PIXEL_W:xxxx"
#IOVDbSvc.servername  = "ATLAS_COOLPROD"
#IOVDbSvc.dbname      = "COMP130"
#IOVDbSvc.userName    = "ATLAS_COOL_PIXEL_W"
#IOVDbSvc.userPwd     = "xxxx"
# setup PixelCalibDbTool in ToolSvc
#PixelCalibDbTool = Service("ToolSvc.PixelCalibDbTool")

PixelCalibDbTool.CalibFolder ="/PIXEL/PixCalib"
PixelCalibDbTool.WriteDB = True
# Set the following when reading back to adjust the run/event numbers
#EventSelector = Service( "EventSelector" )
#EventSelector.RunNumber         = 1
#EventSelector.EventsPerRun      = 5
#EventSelector.FirstEvent        = 1
theApp.EvtMax = 1

#--------------------------------------------------------------
# Set output level threshold (1=VERBOSE, 2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
#--------------------------------------------------------------
MessageSvc.OutputLevel = INFO
# --- change output format
MessageSvc.Format       = "% F%30W%S%7W%R%T %0W%M"
MessageSvc.defaultLimit = 9999999
