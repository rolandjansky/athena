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
# Load IOVDbSvc
#--------------------------------------------------------------

IOVDbSvc = Service("IOVDbSvc")
from IOVDbSvc.CondDB import conddb
#IOVDbSvc.GlobalTag="OFLCOND-FDR-01-02-00"
#'''
#DBname='<dbConnection>oracle://DEVDB10;schema=ATLAS_SCT_COMMCOND_DEV;dbname=ROE2;user=ATLAS_SCT_COMMCOND_DEV</dbConnection>'

#IOVDbSvc.Folders += [ DBname +' /SCT/Sensors']
#'''
IOVDbSvc.GlobalTag="OFLCOND-FDR-01-02-00"
IOVDbSvc.OutputLevel = 3
conddb.addFolder('',"<db>COOLOFL_SCT/COMP200</db> /SCT/Sensors <tag>HEAD</tag>")

from AthenaCommon.AlgSequence import AlgSequence
job = AlgSequence()

from SCT_ConditionsServices.SCT_ConditionsServicesConf import SCT_SensorsTestAlg
job+= SCT_SensorsTestAlg()

from SCT_ConditionsServices.SCT_ConditionsServicesConf import SCT_SensorsSvc
ServiceMgr +=SCT_SensorsSvc()

#SCT_SensorsSvc.AttrListCollFolders=["/SCT/Sensors"]

import AthenaCommon.AtlasUnixGeneratorJob


ServiceMgr.EventSelector.RunNumber  = 0
theApp.EvtMax                   = 1
