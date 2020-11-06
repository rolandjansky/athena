################################################################################
# Job options file to test the Cabling
################################################################################

#--------------------------------------------------------------
# Thread-specific setup
#-------------------------------------------------------------- 
from AthenaCommon.ConcurrencyFlags import jobproperties
numThreads = jobproperties.ConcurrencyFlags.NumThreads()
if numThreads > 0:
  from AthenaCommon.AlgScheduler import AlgScheduler
  AlgScheduler.CheckDependencies( True )
  AlgScheduler.ShowControlFlow( True )
  AlgScheduler.ShowDataDependencies( True )

from AthenaCommon.AppMgr import theApp
theApp.EvtMax = 1
theApp.AuditAlgorithms = True

from AthenaCommon.AppMgr import ServiceMgr

#--------------------------------------------------------------
# Set output lvl (VERBOSE, DEBUG, INFO, WARNING, ERROR, FATAL)
#--------------------------------------------------------------
ServiceMgr.MessageSvc.OutputLevel = INFO
ServiceMgr.MessageSvc.infoLimit = 100000

#--------------------------------------------------------------
# Event selector settings. Use McEventSelector
#--------------------------------------------------------------
import AthenaCommon.AtlasUnixGeneratorJob
# ServiceMgr.EventSelector.RunNumber = 310809
ServiceMgr.EventSelector.InitialTimeStamp = 1565613360

#--------------------------------------------------------------
# Load Geometry
#--------------------------------------------------------------
from AthenaCommon.GlobalFlags import globalflags
globalflags.DetDescrVersion = "ATLAS-R2-2016-01-00-01"
globalflags.DetGeo="atlas"
globalflags.InputFormat="pool"
globalflags.DataSource="data"

#--------------------------------------------------------------
# Load IOVDbSvc
#--------------------------------------------------------------
from IOVDbSvc.CondDB import conddb
conddb.dbdata = "CONDBR2"
ServiceMgr.IOVDbSvc.GlobalTag = "CONDBR2-BLKPA-2018-15"
ServiceMgr.IOVDbSvc.OutputLevel = DEBUG
ServiceMgr.IOVDbSvc.OutputToFile = True

#--------------------------------------------------------------
# use auditors
#--------------------------------------------------------------
from GaudiCommonSvc.GaudiCommonSvcConf import AuditorSvc
ServiceMgr += AuditorSvc()
ServiceMgr.AuditorSvc.Auditors += [ "ChronoAuditor"]
ServiceMgr.AuditorSvc.Auditors += [ "MemStatAuditor" ]

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

#--------------------------------------------------------------
# Load CondAlg, Tool and TestAlg
#--------------------------------------------------------------
from AthenaCommon.AlgSequence import AthSequencer
condSeq = AthSequencer("AthCondSeq")
condAlgName = "SCT_CablingCondAlgFromCoraCool"
if not hasattr(condSeq, condAlgName):
  from AthenaCommon.CfgGetter import getAlgorithm
  SCT_CablingCondAlgFromCoraCool = getAlgorithm(condAlgName)
  condSeq += SCT_CablingCondAlgFromCoraCool

from AthenaCommon.CfgGetter import getPrivateTool
SCT_CablingTool = getPrivateTool("SCT_CablingTool")
SCT_CablingTool.DataSource = "COOLVECTOR"
SCT_CablingTool.OutputLevel = INFO

from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()
from SCT_Cabling.SCT_CablingConf import SCT_TestCablingAlg
topSequence += SCT_TestCablingAlg(SCT_CablingTool=SCT_CablingTool,
                                  OutputLevel = INFO,
                                  POSIXtime = ServiceMgr.EventSelector.InitialTimeStamp)

if numThreads >= 2:
  from SCT_ConditionsAlgorithms.SCTCondAlgCardinality import sctCondAlgCardinality
  sctCondAlgCardinality.set(numThreads)
  topSequence.SCT_TestCablingAlg = numThreads
  

