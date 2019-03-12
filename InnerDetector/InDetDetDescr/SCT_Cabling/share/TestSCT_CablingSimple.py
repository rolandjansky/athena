################################################################################
# Job options file to test the Cabling
################################################################################

from AthenaCommon.AppMgr import theApp
theApp.EvtMax = 20
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
ServiceMgr.EventSelector.RunNumber = 310809
# initial time stamp - this is number of seconds since 1st Jan 1970 GMT
# run 310809 Recording start/end 2016-Oct-17 21:39:18 / 2016-Oct-18 16:45:23 UTC
ServiceMgr.EventSelector.InitialTimeStamp = 1476741326 # LB 18 of run 310809, 10/17/2016 @ 9:55pm (UTC)

#--------------------------------------------------------------
# Load IOVDbSvc
#--------------------------------------------------------------
from IOVDbSvc.CondDB import conddb
ServiceMgr.IOVDbSvc.GlobalTag = "OFLCOND-RUN12-SDR-25"
ServiceMgr.IOVDbSvc.OutputLevel = DEBUG

#--------------------------------------------------------------
# use auditors
#--------------------------------------------------------------
from GaudiSvc.GaudiSvcConf import AuditorSvc
ServiceMgr += AuditorSvc()
ServiceMgr.AuditorSvc.Auditors += [ "ChronoAuditor"]
ServiceMgr.AuditorSvc.Auditors += [ "MemStatAuditor" ]

#--------------------------------------------------------------
# Load Geometry
#--------------------------------------------------------------
from AthenaCommon.GlobalFlags import globalflags
globalflags.DetDescrVersion = "ATLAS-R2-2015-03-01-00"
globalflags.DetGeo="atlas"
globalflags.InputFormat="pool"
globalflags.DataSource="geant4"

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
                                  OutputLevel = INFO)

