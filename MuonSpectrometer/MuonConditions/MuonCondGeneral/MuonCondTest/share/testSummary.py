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
theApp.AuditAlgorithms=False


from AthenaCommon.GlobalFlags import GlobalFlags
# --- default is atlas geometry
GlobalFlags.DetGeo.set_atlas()
# --- set defaults
GlobalFlags.DataSource.set_geant4()    
GlobalFlags.InputFormat.set_pool()    
# --- default is zero luminosity
GlobalFlags.Luminosity.set_zero()
GlobalFlags.Print()

#--------------------------------------------------------------
# Set Detector setup
#--------------------------------------------------------------

from AthenaCommon.DetFlags import DetFlags 
DetFlags.detdescr.SCT_setOn()
DetFlags.ID_setOff()
DetFlags.Calo_setOff()
DetFlags.Muon_setOn()
DetFlags.Truth_setOff()
DetFlags.LVL1_setOff()
DetFlags.SCT_setOff()
DetFlags.TRT_setOff()


#switch off tasks
#DetFlags.pileup.all_setOff()
#DetFlags.simulate.all_setOff()
#DetFlags.makeRIO.all_setOff()
#DetFlags.writeBS.all_setOff()
#DetFlags.readRDOBS.all_setOff()
#DetFlags.readRIOBS.all_setOff()
#DetFlags.readRIOPool.all_setOff()
#DetFlags.writeRIOPool.all_setOff()

import AtlasGeoModel.SetGeometryVersion
import AtlasGeoModel.GeoModelInit

from AthenaCommon.AlgSequence import AlgSequence

job = AlgSequence()


include("MuonCondTest/MDTCondSummary_topOptions.py")
include("MuonCondTest/RPCCondSummary_topOptions.py")


from MuonCondSvc.MuonCondSvcConf import MuonConditionsSummarySvc
ServiceMgr +=MuonConditionsSummarySvc()

MuonConditionsSummary=ServiceMgr.MuonConditionsSummarySvc
MuonConditionsSummary.ConditionsServices=["MDTCondSummarySvc","RPCCondSummarySvc"]

#from MuonCondTest.MuonCondTestConf import MuonDetectorStatusTest
#job+= MuonDetectorStatusTest()

from MuonCondTest.MuonCondTestConf import MuonConditionsSummaryTestAlg
job+= MuonConditionsSummaryTestAlg()

import AthenaCommon.AtlasUnixGeneratorJob

ServiceMgr.EventSelector.RunNumber  = 122189
#ServiceMgr.EventSelector.RunNumber  = 40341 #1204110576 seconds epoch
import time, calendar
#time in seconds , now
#ServiceMgr.EventSelector.InitialTimeStamp  = calendar.timegm(time.gmtime())
#ServiceMgr.EventSelector.InitialTimeStamp  = 1204216576 #found valid in db browser?
#ServiceMgr.EventSelector.InitialTimeStamp  = 429496
theApp.EvtMax                   = 30 

ServiceMgr.MessageSvc.Format           = "% F%40W%S%7W%R%T %0W%M"
ServiceMgr.MessageSvc.OutputLevel = 3
