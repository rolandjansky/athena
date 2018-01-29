useDB=True

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
globalflags.DetDescrVersion="ATLAS-R2-2015-03-01-00"
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

# Disable SiLorentzAngleSvc to remove
# ERROR ServiceLocatorHelper::createService: wrong interface id IID_665279653 for service
ServiceMgr.GeoModelSvc.DetectorTools['PixelDetectorTool'].LorentzAngleSvc=""
ServiceMgr.GeoModelSvc.DetectorTools['SCT_DetectorTool'].LorentzAngleSvc=""

from AthenaCommon.AlgSequence import AlgSequence
job = AlgSequence()

#--------------------------------------------------------------
# Load IOVDbSvc
#--------------------------------------------------------------
IOVDbSvc = Service("IOVDbSvc")
from IOVDbSvc.CondDB import conddb
IOVDbSvc.GlobalTag="OFLCOND-MC16-SDR-18"
IOVDbSvc.OutputLevel = 3

if useDB:
    from IOVDbSvc.CondDB import conddb
    sctDCSStateFolder = '/SCT/DCS/CHANSTAT'
    sctDCSTempFolder = '/SCT/DCS/MODTEMP'
    sctDCSHVFolder = '/SCT/DCS/HV'
    if not conddb.folderRequested(sctDCSStateFolder):
        conddb.addFolder("DCS_OFL", sctDCSStateFolder, className="CondAttrListCollection")
    if not conddb.folderRequested(sctDCSTempFolder):
        conddb.addFolder("DCS_OFL", sctDCSTempFolder, className="CondAttrListCollection")
    if not conddb.folderRequested(sctDCSHVFolder):
        conddb.addFolder("DCS_OFL", sctDCSHVFolder, className="CondAttrListCollection")

    from AthenaCommon.AlgSequence import AthSequencer
    condSeq = AthSequencer("AthCondSeq")
    # For SCT_DCSConditionsSvc
    if not hasattr(condSeq, "SCT_DCSConditionsHVCondAlg"):
        from SCT_ConditionsServices.SCT_ConditionsServicesConf import SCT_DCSConditionsHVCondAlg
        condSeq += SCT_DCSConditionsHVCondAlg(name = "SCT_DCSConditionsHVCondAlg",
                                              ReadKey = sctDCSHVFolder)
    if not hasattr(condSeq, "SCT_DCSConditionsStatCondAlg"):
        from SCT_ConditionsServices.SCT_ConditionsServicesConf import SCT_DCSConditionsStatCondAlg
        condSeq += SCT_DCSConditionsStatCondAlg(name = "SCT_DCSConditionsStatCondAlg",
                                                ReadKeyHV = sctDCSHVFolder,
                                                ReadKeyState = sctDCSStateFolder)
    if not hasattr(condSeq, "SCT_DCSConditionsTempCondAlg"):
        from SCT_ConditionsServices.SCT_ConditionsServicesConf import SCT_DCSConditionsTempCondAlg
        condSeq += SCT_DCSConditionsTempCondAlg(name = "SCT_DCSConditionsTempCondAlg",
                                                ReadKey = sctDCSTempFolder)
    # For SCT_SiliconConditionsSvc
    if not hasattr(condSeq, "SCT_SiliconTempCondAlg"):
        from SCT_ConditionsServices.SCT_ConditionsServicesConf import SCT_SiliconTempCondAlg
        condSeq += SCT_SiliconTempCondAlg(name = "SCT_SiliconTempCondAlg")
    if not hasattr(condSeq, "SCT_SiliconHVCondAlg"):
        from SCT_ConditionsServices.SCT_ConditionsServicesConf import SCT_SiliconHVCondAlg
        condSeq += SCT_SiliconHVCondAlg(name = "SCT_SiliconHVCondAlg")

    from SCT_ConditionsServices.SCT_ConditionsServicesConf import SCT_DCSConditionsSvc
    SCT_DCSConditionsSvc.AttrListCollFolders=[sctDCSStateFolder, sctDCSTempFolder, sctDCSHVFolder]
    ServiceMgr += SCT_DCSConditionsSvc(name="InDetSCT_DCSConditionsSvc")

from SCT_ConditionsServices.SCT_ConditionsServicesConf import SCT_SiliconConditionsSvc
ServiceMgr += SCT_SiliconConditionsSvc()
SCT_SiliconConditions=ServiceMgr.SCT_SiliconConditionsSvc
SCT_SiliconConditions.UseDB = useDB

from SCT_ConditionsServices.SCT_ConditionsServicesConf import SCT_SiliconConditionsTestAlg
job+= SCT_SiliconConditionsTestAlg()

import AthenaCommon.AtlasUnixGeneratorJob

ServiceMgr.EventSelector.RunNumber  = 300000 # MC16c 2017 run number
import time, calendar
#time in seconds , now
#ServiceMgr.EventSelector.InitialTimeStamp  = calendar.timegm(time.gmtime())
ServiceMgr.EventSelector.InitialTimeStamp  = 1500000000
ServiceMgr.EventSelector.TimeStampInterval = 0
theApp.EvtMax                   = 1

ServiceMgr.MessageSvc.Format           = "% F%40W%S%7W%R%T %0W%M"
ServiceMgr.MessageSvc.OutputLevel = 3
