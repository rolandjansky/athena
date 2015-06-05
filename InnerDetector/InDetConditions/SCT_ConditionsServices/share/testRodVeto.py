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


from AthenaCommon.AlgSequence import AlgSequence

job = AlgSequence()


from SCT_Cabling.SCT_CablingConf import SCT_CablingSvc
ToolSvc = ServiceMgr.ToolSvc
ServiceMgr+=SCT_CablingSvc()

#--------------------------------------------------------------
# Load IOVDbSvc
#--------------------------------------------------------------

IOVDbSvc = Service("IOVDbSvc")
from IOVDbSvc.CondDB import conddb
IOVDbSvc.GlobalTag="OFLCOND-FDR-01-02-00"

conddb.addFolder("SCT","/SCT/DAQ/Configuration/ROD")
conddb.addFolder("SCT","/SCT/DAQ/Configuration/MUR")
conddb.addFolder("SCT","/SCT/DAQ/Configuration/RODMUR")
conddb.addFolder("SCT","/SCT/DAQ/Configuration/Geog")




from SCT_ConditionsServices.SCT_ConditionsServicesConf import SCT_RODVetoSvc
ServiceMgr += SCT_RODVetoSvc()
SCT_RODVeto=ServiceMgr.SCT_RODVetoSvc
SCT_RODVeto.BadRODIdentifiers=[0x240100,0x240030]

from SCT_ConditionsServices.SCT_ConditionsServicesConf import SCT_RODVetoTestAlg
job+= SCT_RODVetoTestAlg()


import AthenaCommon.AtlasUnixGeneratorJob
ServiceMgr.SCT_CablingSvc.OutputLevel = INFO
ServiceMgr.SCT_RODVetoSvc.OutputLevel=VERBOSE
ServiceMgr.EventSelector.InitialTimeStamp = 1409756400
ServiceMgr.EventSelector.RunNumber  = 0
theApp.EvtMax                   = 1
