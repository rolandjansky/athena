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
# Thread-specific setup
#--------------------------------------------------------------
from AthenaCommon.ConcurrencyFlags import jobproperties
if jobproperties.ConcurrencyFlags.NumThreads() > 0:
    from AthenaCommon.AlgScheduler import AlgScheduler
    AlgScheduler.CheckDependencies( True )
    AlgScheduler.ShowControlFlow( True )
    AlgScheduler.ShowDataDependencies( True )

#--------------------------------------------------------------
# use auditors
#--------------------------------------------------------------
from AthenaCommon.AppMgr import ServiceMgr
from GaudiSvc.GaudiSvcConf import AuditorSvc
ServiceMgr += AuditorSvc()
theAuditorSvc = ServiceMgr.AuditorSvc
theAuditorSvc.Auditors  += [ "ChronoAuditor"]
theAuditorSvc.Auditors  += [ "MemStatAuditor" ]
theApp.AuditAlgorithms=True

#--------------------------------------------------------------
# Load Geometry
#--------------------------------------------------------------
from AthenaCommon.GlobalFlags import globalflags
globalflags.DetDescrVersion="ATLAS-R2-2015-03-01-00"
globalflags.DetGeo="atlas"
globalflags.InputFormat="pool"
globalflags.DataSource="data"
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

# Set up SCT_DCSConditionsTool and required conditions folders and conditions algorithms
from SCT_ConditionsTools.SCT_DCSConditionsToolSetup import SCT_DCSConditionsToolSetup
sct_DCSConditionsToolSetup = SCT_DCSConditionsToolSetup()
sct_DCSConditionsToolSetup.setup()

#--------------------------------------------------------------
# Load DCSConditions Alg and Service
#--------------------------------------------------------------
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

from SCT_ConditionsAlgorithms.SCT_ConditionsAlgorithmsConf import SCT_DCSConditionsTestAlg
topSequence+= SCT_DCSConditionsTestAlg(SCT_DCSConditionsTool=sct_DCSConditionsToolSetup.getTool())

#--------------------------------------------------------------
# Event selector settings. Use McEventSelector
#--------------------------------------------------------------
import AthenaCommon.AtlasUnixGeneratorJob
ServiceMgr.EventSelector.RunNumber = 310809
# initial time stamp - this is number of seconds since 1st Jan 1970 GMT
ServiceMgr.EventSelector.InitialTimeStamp  = 1476741326 # LB 18 of run 310809, 10/17/2016 @ 9:55pm (UTC)
ServiceMgr.EventSelector.TimeStampInterval = 180 # increment of 3 minutes

theApp.EvtMax = 20

#--------------------------------------------------------------
# Set output lvl (VERBOSE, DEBUG, INFO, WARNING, ERROR, FATAL)
#--------------------------------------------------------------
ServiceMgr.MessageSvc.OutputLevel = 3
#ServiceMgr.SCT_DCSConditionsSvc.OutputLevel = 3
#topSequence.SCT_DCSConditionsTestAlg.OutputLevel = 3

#--------------------------------------------------------------
# Load IOVDbSvc
#--------------------------------------------------------------
IOVDbSvc = Service("IOVDbSvc")
from IOVDbSvc.CondDB import conddb
IOVDbSvc.GlobalTag="CONDBR2-BLKPA-2017-06"
IOVDbSvc.OutputLevel = 3
