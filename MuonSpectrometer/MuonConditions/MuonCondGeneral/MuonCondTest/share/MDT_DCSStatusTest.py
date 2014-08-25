from AthenaCommon.GlobalFlags import GlobalFlags
from AthenaCommon.AppMgr import ServiceMgr
from AthenaCommon.AppMgr import ToolSvc
from AthenaCommon.AlgSequence import AlgSequence

# --- default is atlas geometry
GlobalFlags.DetGeo.set_atlas()
# --- set defaults
GlobalFlags.DataSource.set_geant4()
GlobalFlags.InputFormat.set_pool()
# --- default is zero luminosity
GlobalFlags.Luminosity.set_zero()
GlobalFlags.Print()



#--------------------------------------------------------------
# Set Detector setup
#--------------------------------------------------------------
# --- switch on InnerDetector
from AthenaCommon.DetFlags import DetFlags
DetFlags.detdescr.SCT_setOff()
DetFlags.ID_setOff()
DetFlags.Calo_setOff()
DetFlags.Muon_setOn()
DetFlags.Truth_setOff()
DetFlags.LVL1_setOff()
DetFlags.SCT_setOff()
DetFlags.TRT_setOff()

#import AtlasGeoModel.SetGeometryVersion
#import AtlasGeoModel.GeoModelInit

#from GeoModelSvc.GeoModelSvcConf import GeoModelSvc

#DetDescrVersion="ATLAS-CSC-01-02-00"
from AthenaCommon.JobProperties import jobproperties
#jobproperties.Global.DetDescrVersion = DetDescrVersion

from GeoModelSvc.GeoModelSvcConf import GeoModelSvc
from AtlasGeoModel import GeoModelInit
from AtlasGeoModel import SetGeometryVersion, GeoModelInit
GeoModelSvc = ServiceMgr.GeoModelSvc
GeoModelSvc.IgnoreTagDifference = True
DetDescrVersion = "ATLAS-GEO-08-00-02"
jobproperties.Global.DetDescrVersion = DetDescrVersion
#DetDescrVersion = GlobalFlags.DetDescrVersion()
GeoModelSvc.AtlasVersion = DetDescrVersion



topSequence=AlgSequence()

#import AthenaPoolCnvSvc.ReadAthenaPool

#ServiceMgr.EventSelector.InputCollections = ["/afs/cern.ch/atlas/maxidisk/d98/14.3.0/MuonSpectrometer/MuonCablings/MuonMDT_Cabling/MuonMDT_Cabling-00-00-04/share/AOD.032240._00100.pool.root.1"]

from MuonCondTest.MuonCondTestConf import MDT_DCSStatusTest

topSequence += MDT_DCSStatusTest()

from MuonCondTool.MuonCondToolConf import MDT_DCSConditionsTool
MDT_DCSConditionsTool = MDT_DCSConditionsTool("MDT_DCSConditionsTool")
MDT_DCSConditionsTool.OutputLevel = VERBOSE 
#MessageSvc.OutputLevel = DEBUG

MDT_DCSConditionsTool.DropChamberFolder = "/MDT/DCS/DROPPEDCH"

ToolSvc += MDT_DCSConditionsTool

dbConn="oracle://ATLAS_COOLPROD;schema=ATLAS_COOLOFL_DCS;dbname=COMP200"
folder="/MDT/DCS/DROPPEDCH"

from IOVDbSvc.IOVDbSvcConf import IOVDbSvc
from IOVDbSvc.CondDB import conddb
include("RegistrationServices/IOVRegistrationSvc_jobOptions.py")

ServiceMgr.IOVDbSvc.dbConnection=dbConn
ServiceMgr.IOVDbSvc.Folders+=[folder+" <tag>HEAD</tag> <dbConnection>"+dbConn+"</dbConnection>"]


import AthenaCommon.AtlasUnixGeneratorJob

ServiceMgr.EventSelector.RunNumber   = 131576 
#import time, calendar
#time in seconds , now
#ServiceMgr.EventSelector.InitialTimeStamp  = calendar.timegm(time.gmtime())
ServiceMgr.EventSelector.InitialTimeStamp  =1253975895  #found valid in db browser?
#ServiceMgr.EventSelector.TimeStampInterval = 6000
svcMgr.EventSelector.OverrideRunNumber=True
theApp.EvtMax                   = 4

ServiceMgr.MessageSvc.Format           = "% F%40W%S%7W%R%T %0W%M"
ServiceMgr.MessageSvc.OutputLevel = VERBOSE 


