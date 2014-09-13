###############################################################
#
# Job options file to test DCS conditions tool
#
#==============================================================

#--------------------------------------------------------------
# Standard includes
#--------------------------------------------------------------
import AthenaCommon.AtlasUnixStandardJob

#--------------------------------------------------------------
# use auditors
#--------------------------------------------------------------
from AthenaCommon.AppMgr import ServiceMgr

from GaudiSvc.GaudiSvcConf import AuditorSvc
#from AthenaCommon.AppMgr import theApp

ServiceMgr += AuditorSvc()
theAuditorSvc = ServiceMgr.AuditorSvc
theAuditorSvc.Auditors  += [ "ChronoAuditor"]
#ChronoStatSvc = Service ( "ChronoStatSvc")
theAuditorSvc.Auditors  += [ "MemStatAuditor" ]
#MemStatAuditor = theAuditorSvc.auditor( "MemStatAuditor" )
theApp.AuditAlgorithms=False

#--------------------------------------------------------------
# Load Geometry
#--------------------------------------------------------------
from AthenaCommon.GlobalFlags import globalflags
globalflags.DetDescrVersion="ATLAS-GEO-16-00-00"
globalflags.DetGeo="atlas"
globalflags.InputFormat="pool"
globalflags.DataSource="geant4"
print 'globalTags.DatabaseInstance', globalflags.DatabaseInstance


#--------------------------------------------------------------
# Set Detector setup
#--------------------------------------------------------------
# --- switch on InnerDetector
from AthenaCommon.DetFlags import DetFlags
DetFlags.detdescr.SCT_setOn()
DetFlags.ID_setOn()
DetFlags.Calo_setOff()
DetFlags.Muon_setOff()
DetFlags.Truth_setOff()
DetFlags.LVL1_setOff()
DetFlags.SCT_setOn()
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


import AtlasGeoModel.SetGeometryVersion
import AtlasGeoModel.GeoModelInit

#--------------------------------------------------------------
# Load DCSConditions Alg and Service
#--------------------------------------------------------------
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

from SCT_ConditionsServices.SCT_ConditionsServicesConf import SCT_TdaqEnabledTestAlg
topSequence+= SCT_TdaqEnabledTestAlg()

from SCT_ConditionsServices.SCT_ConditionsServicesConf import SCT_TdaqEnabledSvc
ServiceMgr += SCT_TdaqEnabledSvc()

SCT_TdaqEnabledSvc.AttrListCollFolders=["/TDAQ/EnabledResources/ATLAS/SCT/Robins"]


#--------------------------------------------------------------
# Event selector settings. Use McEventSelector
#--------------------------------------------------------------
import AthenaCommon.AtlasUnixGeneratorJob
#ServiceMgr+= EventSelector()
#ServiceMgr.EventSelector.FirstEvent = 1
#ServiceMgr.EventSelector.EventsPerRun = 5
#ServiceMgr.EventSelector.RunNumber = 140975
ServiceMgr.EventSelector.RunNumber = 90975

# initial time stamp - this is number of seconds since 1st Jan 1970 GMT
# the value given here corresponds to 10-09-2009 17:01 geneva local time
#ServiceMgr.EventSelector.InitialTimeStamp  = 1252594860
#run 140975 started on 2009-Nov-27 03:16:37 CET and lasted 4:32:40:
ServiceMgr.EventSelector.InitialTimeStamp  = 1259289060
# increment of 3 minutes
ServiceMgr.EventSelector.TimeStampInterval = 180

theApp.EvtMax                   = 5

#--------------------------------------------------------------
# Set output lvl (VERBOSE, DEBUG, INFO, WARNING, ERROR, FATAL)
#--------------------------------------------------------------
ServiceMgr.MessageSvc.OutputLevel = 3

#--------------------------------------------------------------
# Load IOVDbSvc
#--------------------------------------------------------------
IOVDbSvc = Service("IOVDbSvc")
from IOVDbSvc.CondDB import conddb
#IOVDbSvc.GlobalTag="HEAD"
IOVDbSvc.GlobalTag="OFLCOND-FDR-01-02-00"
IOVDbSvc.OutputLevel = 3
conddb.addFolder('',"<db>COOLONL_TDAQ/COMP200</db> /TDAQ/EnabledResources/ATLAS/SCT/Robins")
conddb.addFolder("","<db>COOLONL_SCT/COMP200</db> /SCT/DAQ/Configuration/ROD")
conddb.addFolder("","<db>COOLONL_SCT/COMP200</db> /SCT/DAQ/Configuration/Geog")
conddb.addFolder("","<db>COOLONL_SCT/COMP200</db> /SCT/DAQ/Configuration/RODMUR")
conddb.addFolder("","<db>COOLONL_SCT/COMP200</db> /SCT/DAQ/Configuration/MUR")


