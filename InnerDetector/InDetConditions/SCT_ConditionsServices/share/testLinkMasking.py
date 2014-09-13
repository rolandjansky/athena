import AthenaCommon.AtlasUnixStandardJob

# use auditors
from AthenaCommon.AppMgr import ServiceMgr

from GaudiSvc.GaudiSvcConf import AuditorSvc

ServiceMgr += AuditorSvc()
theAuditorSvc = ServiceMgr.AuditorSvc
theAuditorSvc.Auditors  += [ "ChronoAuditor"]
#ChronoStatSvc = Service ( "ChronoStatSvc")
theAuditorSvc.Auditors  += [ "MemStatAuditor" ]
#MemStatAuditor = theAuditorSvc.auditor( "MemStatAuditor" )
theApp.AuditAlgorithms=True


#--------------------------------------------------------------
# Load Geometry
#--------------------------------------------------------------
from AthenaCommon.GlobalFlags import globalflags
globalflags.DetDescrVersion="ATLAS-GEO-16-00-00"
globalflags.DetGeo="atlas"
globalflags.InputFormat="pool"
globalflags.DataSource="geant4"
print globalflags

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


from AthenaCommon.AlgSequence import AlgSequence

job = AlgSequence()

#--------------------------------------------------------------
# Load IOVDbSvc
#--------------------------------------------------------------
IOVDbSvc = Service("IOVDbSvc")
from IOVDbSvc.CondDB import conddb
IOVDbSvc.GlobalTag="OFLCOND-FDR-01-02-00"
IOVDbSvc.OutputLevel = 3

conddb.addFolder("","<db>COOLONL_SCT/COMP200</db> /SCT/DAQ/Configuration/ROD")
conddb.addFolder("","<db>COOLONL_SCT/COMP200</db> /SCT/DAQ/Configuration/Geog")
conddb.addFolder("","<db>COOLONL_SCT/COMP200</db> /SCT/DAQ/Configuration/RODMUR")
conddb.addFolder("","<db>COOLONL_SCT/COMP200</db> /SCT/DAQ/Configuration/MUR")
conddb.addFolder("","<dbConnection>sqlite://;schema=LMM_Test.dbw;dbname=DUMMYDB</dbConnection> /purple/pants")

from SCT_ConditionsServices.SCT_ConditionsServicesConf import SCT_LinkMaskingSvc
LinkMaskingSvc = SCT_LinkMaskingSvc(name = "SCT_LinkMaskingSvc")
ServiceMgr += LinkMaskingSvc

from SCT_ConditionsServices.SCT_ConditionsServicesConf import SCT_LinkMaskingTestAlg
LinkMaskingTestAlg = SCT_LinkMaskingTestAlg(name = "SCT_LinkMaskingTestAlg")
LinkMaskingTestAlg.LinkMaskingSvc =  LinkMaskingSvc
job += LinkMaskingTestAlg

import AthenaCommon.AtlasUnixGeneratorJob

ServiceMgr.EventSelector.RunNumber  = 40341 #1204110576 seconds epoch
ServiceMgr.EventSelector.InitialTimeStamp  = 1204216576 
theApp.EvtMax                   = 1

ServiceMgr.MessageSvc.Format           = "% F%40W%S%7W%R%T %0W%M"
ServiceMgr.MessageSvc.OutputLevel = 3
