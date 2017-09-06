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
globalflags.DetDescrVersion="ATLAS-R2-2016-01-00-01"
globalflags.DetGeo="atlas"
globalflags.InputFormat="pool"
globalflags.DataSource="data"
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

# Disable SiLorentzAngleSvc to remove
# ERROR ServiceLocatorHelper::createService: wrong interface id IID_665279653 for service
ServiceMgr.GeoModelSvc.DetectorTools['PixelDetectorTool'].LorentzAngleSvc=""
ServiceMgr.GeoModelSvc.DetectorTools['SCT_DetectorTool'].LorentzAngleSvc=""

from IOVSvc.IOVSvcConf import CondSvc 
ServiceMgr += CondSvc()
from AthenaCommon.AlgSequence import AthSequencer 
condSeq = AthSequencer("AthCondSeq")

from xAODEventInfoCnv.xAODEventInfoCreator import xAODMaker__EventInfoCnvAlg
condSeq+=xAODMaker__EventInfoCnvAlg(OutputLevel=2)

from IOVSvc.IOVSvcConf import CondInputLoader 
condSeq += CondInputLoader( "CondInputLoader",OutputLevel=2)
import StoreGate.StoreGateConf as StoreGateConf 
ServiceMgr += StoreGateConf.StoreGateSvc("ConditionStore")
from  SCT_ConditionsServices.SCT_ConditionsServicesConf import SCT_TdaqEnabledCondAlg
condSeq += SCT_TdaqEnabledCondAlg( "SCT_TdaqEnabledCondAlg" ) 

#--------------------------------------------------------------
# Load DCSConditions Alg and Service
#--------------------------------------------------------------
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

from SCT_ConditionsServices.SCT_ConditionsServicesConf import SCT_TdaqEnabledTestAlg
topSequence+= SCT_TdaqEnabledTestAlg()

from SCT_ConditionsServices.SCT_ConditionsServicesConf import SCT_TdaqEnabledSvc
ServiceMgr += SCT_TdaqEnabledSvc()

SCT_TdaqEnabledSvc.AttrListCollFolders=["/TDAQ/Resources/ATLAS/SCT/Robins"]


#--------------------------------------------------------------
# Event selector settings. Use McEventSelector
#--------------------------------------------------------------
import AthenaCommon.AtlasUnixGeneratorJob
ServiceMgr.EventSelector.RunNumber = 310809

# initial time stamp - this is number of seconds since 1st Jan 1970 GMT
# run 310809 Recording start/end 2016-Oct-17 21:39:18 / 2016-Oct-18 16:45:23 UTC
ServiceMgr.EventSelector.InitialTimeStamp  = 1476741326 # LB 18 of run 310809, 10/17/2016 @ 9:55pm (UTC)
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
IOVDbSvc.GlobalTag="CONDBR2-BLKPA-2017-06"
IOVDbSvc.OutputLevel = 3
conddb.addFolder("TDAQ", "/TDAQ/Resources/ATLAS/SCT/Robins", className="CondAttrListCollection")
conddb.addFolderSplitMC("SCT", "/SCT/DAQ/Config/ROD", "/SCT/DAQ/Config/ROD")
conddb.addFolderSplitMC("SCT", "/SCT/DAQ/Config/Geog", "/SCT/DAQ/Config/Geog")
conddb.addFolderSplitMC("SCT", "/SCT/DAQ/Config/RODMUR", "/SCT/DAQ/Config/RODMUR")
conddb.addFolderSplitMC("SCT", "/SCT/DAQ/Config/MUR", "/SCT/DAQ/Config/MUR")
