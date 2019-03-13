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

# use auditors
from AthenaCommon.AppMgr import ServiceMgr

from GaudiSvc.GaudiSvcConf import AuditorSvc

ServiceMgr += AuditorSvc()
theAuditorSvc = ServiceMgr.AuditorSvc
theAuditorSvc.Auditors  += [ "ChronoAuditor"]
theAuditorSvc.Auditors  += [ "MemStatAuditor" ]
theApp.AuditAlgorithms=True

from AthenaCommon.GlobalFlags import globalflags 
globalflags.DetDescrVersion="ATLAS-R2-2016-01-00-01"
globalflags.DetGeo="atlas" 
globalflags.InputFormat="pool" 
globalflags.DataSource="data" 

from AthenaCommon.AlgSequence import AthSequencer
condSeq = AthSequencer("AthCondSeq")

from xAODEventInfoCnv.xAODEventInfoCreator import xAODMaker__EventInfoCnvAlg
condSeq+=xAODMaker__EventInfoCnvAlg(OutputLevel=2)

#--------------------------------------------------------------
# Set Detector setup
#--------------------------------------------------------------
# --- switch on InnerDetector
from AthenaCommon.DetFlags import DetFlags 
DetFlags.ID_setOff()
DetFlags.Calo_setOff()
DetFlags.Muon_setOff()
DetFlags.Truth_setOff()
DetFlags.LVL1_setOff()
DetFlags.SCT_setOn()

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

#------------------------------------------------------------

from AthenaCommon.AlgSequence import AlgSequence
job = AlgSequence()

#------------------------------------------------------------
# Local stuff
#------------------------------------------------------------

from SCT_ConditionsTools.SCT_MonitorConditionsToolSetup import SCT_MonitorConditionsToolSetup
sct_MonitorConditionsToolSetup = SCT_MonitorConditionsToolSetup()
sct_MonitorConditionsToolSetup.setup()

SCT_MonitorConditionsTool=sct_MonitorConditionsToolSetup.getTool()
SCT_MonitorConditionsTool.OutputLevel = DEBUG

from SCT_ConditionsAlgorithms.SCT_ConditionsAlgorithmsConf import SCT_MonitorConditionsTestAlg
job+= SCT_MonitorConditionsTestAlg(SCT_MonitorConditionsTool=SCT_MonitorConditionsTool)

SCT_MonitorTest=job.SCT_MonitorConditionsTestAlg
SCT_MonitorTest.OutputLevel  = 2

#--------------------------------------------------------------
# Load IOVDbSvc
#     - access to COOL -
#--------------------------------------------------------------
#from IOVDbSvc.CondDB import conddb
IOVDbSvc = Service("IOVDbSvc")
IOVDbSvc.GlobalTag='CONDBR2-BLKPA-2017-10'

import RegistrationServices.IOVRegistrationSvc
regSvc = ServiceMgr.IOVRegistrationSvc
regSvc.RecreateFolders = False

#--------------------------------------------------------------
# Load POOL support
#--------------------------------------------------------------
import AthenaPoolCnvSvc.WriteAthenaPool

#--------------------------------------------------------------
# AthenaPool details
#--------------------------------------------------------------
# Check the dictionary in memory for completeness
ServiceMgr.AthenaSealSvc.CheckDictionary = TRUE

#--------------------------------------------------------------
import AthenaCommon.AtlasUnixGeneratorJob

ServiceMgr.EventSelector.RunNumber  = 310809
ServiceMgr.EventSelector.InitialTimeStamp  = 1476741326 # LB 18 of run 310809, 10/17/2016 @ 9:55pm (UTC)
ServiceMgr.EventSelector.TimeStampInterval = 180 # increment of 3 minutes
theApp.EvtMax                       = 20


MessageSvc.OutputLevel = INFO
