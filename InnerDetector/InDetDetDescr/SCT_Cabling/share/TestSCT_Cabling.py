################################################################################
# Job options file to test the Cabling
################################################################################

# ******** SETUP 20.1.3 ****************
#--------------------------------------------------------------
# use auditors
#--------------------------------------------------------------
from AthenaCommon.AppMgr import ServiceMgr

from GaudiSvc.GaudiSvcConf import AuditorSvc
from AthenaCommon.AppMgr import theApp

ServiceMgr += AuditorSvc()
theAuditorSvc = ServiceMgr.AuditorSvc
theAuditorSvc.Auditors  += [ "ChronoAuditor"]
theAuditorSvc.Auditors  += [ "MemStatAuditor" ]
theApp.AuditAlgorithms=True


#--------------------------------------------------------------
# Load Geometry
#--------------------------------------------------------------
from AthenaCommon.GlobalFlags import globalflags
#globalflags.DetDescrVersion="ATLAS-GEO-16-00-00"
globalflags.DetDescrVersion = "ATLAS-GEO-20-00-01"
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
DetFlags.pixel_setOff()
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
# Load ReadCalibData Alg and Service
#--------------------------------------------------------------
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()


from SCT_Cabling.SCT_CablingConf import SCT_CablingSvc
ToolSvc = ServiceMgr.ToolSvc
ServiceMgr+=SCT_CablingSvc()
IOVDbSvc = Service("IOVDbSvc")
from IOVDbSvc.CondDB import conddb


letsTest='ReadingFromCVP'
if (letsTest=='ReadingFromCoraCool'):
  conddb.addFolder("","<db>oracle://ATLAS_COOLPROD;schema=ATLAS_COOLOFL_SCT;dbname=OFLP200</db><tag>SctDaqConfigurationRod-PERFECT-Aug10-01</tag> /SCT/DAQ/Configuration/ROD")
  conddb.addFolder("","<db>oracle://ATLAS_COOLPROD;schema=ATLAS_COOLOFL_SCT;dbname=OFLP200</db><tag>SctDaqConfigurationMur-PERFECT-Aug10-01</tag> /SCT/DAQ/Configuration/MUR")
  conddb.addFolder("","<db>oracle://ATLAS_COOLPROD;schema=ATLAS_COOLOFL_SCT;dbname=OFLP200</db><tag>SctDaqConfigurationRodmur-PERFECT-Aug10-01</tag> /SCT/DAQ/Configuration/RODMUR")
  conddb.addFolder("","<db>oracle://ATLAS_COOLPROD;schema=ATLAS_COOLOFL_SCT;dbname=OFLP200</db><tag>SctDaqConfigurationGeog-PERFECT-Aug10-01</tag> /SCT/DAQ/Configuration/Geog")
  #conddb.addFolder("SCT","<db>COOLONL_SCT/CONDBR2</db> /SCT/DAQ/Configuration/ROD<forceTimestamp>1409896800</forceTimestamp>")
  #conddb.addFolder("SCT","<db>COOLONL_SCT/CONDBR2</db> /SCT/DAQ/Configuration/RODMUR<forceTimestamp>1409896800</forceTimestamp>")
  #conddb.addFolder("SCT","<db>COOLONL_SCT/CONDBR2</db> /SCT/DAQ/Configuration/MUR<forceTimestamp>1409896800</forceTimestamp>")
  #conddb.addFolder("SCT","<db>COOLONL_SCT/CONDBR2</db> /SCT/DAQ/Configuration/Geog<forceTimestamp>1409896800</forceTimestamp>")
  '''
  conddb.addFolder("SCT","/SCT/DAQ/Configuration/ROD <tag>HEAD</tag>")
  conddb.addFolder("SCT","/SCT/DAQ/Configuration/MUR <tag>HEAD</tag>")
  conddb.addFolder("SCT","/SCT/DAQ/Configuration/RODMUR <tag>HEAD</tag>")
  conddb.addFolder("SCT","/SCT/DAQ/Configuration/Geog <tag>HEAD</tag>")
  '''

  ServiceMgr.SCT_CablingSvc.DataSource='CORACOOL'
elif (letsTest=='ReadingFromFile'):
  ServiceMgr.SCT_CablingSvc.DataSource='SCT_Sept08Cabling_svc.dat'
elif (letsTest=='ReadingFromMCFile'):
  ServiceMgr.SCT_CablingSvc.DataSource='SCT_MC_FullCabling_svc.dat'
elif (letsTest=='ReadingFromCVP'):
  ServiceMgr.SCT_CablingSvc.DataSource='COOLVECTOR'
  conddb.addFolder("","<db>sqlite://none;schema=/tmp/sroe/final.db;dbname=OFLP200</db> /SCT/DAQ/Config/ROD <tag>SctDaqConfigRod-MC-06</tag>")
  conddb.addFolder("","<db>sqlite://none;schema=/tmp/sroe/final.db;dbname=OFLP200</db> /SCT/DAQ/Config/MUR <tag>SctDaqConfigMur-MC-06</tag>")
  conddb.addFolder("","<db>sqlite://none;schema=/tmp/sroe/final.db;dbname=OFLP200</db> /SCT/DAQ/Config/RODMUR <tag>SctDaqConfigRodmur-MC-06</tag>")
  conddb.addFolder("","<db>sqlite://none;schema=/tmp/sroe/final.db;dbname=OFLP200</db> /SCT/DAQ/Config/Geog <tag>SctDaqConfigGeog-MC-06</tag>")
  #conddb.addFolder("SCT","<db>COOLOFL_SCT/OFLP200</db> /SCT/DAQ/Config/ROD <tag>SctDaqConfigRod-Run2-10July2015</tag>")
  #conddb.addFolder("SCT","<db>COOLOFL_SCT/OFLP200</db> /SCT/DAQ/Config/RODMUR <tag>SctDaqConfigRodmur-Run2-10July2015</tag>")
  #conddb.addFolder("SCT","<db>COOLOFL_SCT/OFLP200</db> /SCT/DAQ/Config/MUR <tag>SctDaqConfigMur-Run2-10July2015</tag>")
  #conddb.addFolder("SCT","<db>COOLOFL_SCT/OFLP200</db> /SCT/DAQ/Config/Geog <tag>SctDaqConfigGeog-Run2-10July2015</tag>")


from SCT_Cabling.SCT_CablingConf import SCT_TestCablingAlg
topSequence+= SCT_TestCablingAlg()

#--------------------------------------------------------------
# Event selector settings. Use McEventSelector
#--------------------------------------------------------------
import AthenaCommon.AtlasUnixGeneratorJob
#ServiceMgr+= EventSelector()
#ServiceMgr.EventSelector.FirstEvent = 1
#ServiceMgr.EventSelector.EventsPerRun = 5
#ServiceMgr.EventSelector.RunNumber = 0
theApp.EvtMax                    = 1

#For real data, earliest timestamp is 0
#ServiceMgr.EventSelector.InitialTimeStamp = 1228950000
#3rd march 2010, 10am
#ServiceMgr.EventSelector.InitialTimeStamp =1267606800
ServiceMgr.EventSelector.InitialTimeStamp = 1409756400
if (letsTest=='ReadingFromCVP'):
  #ServiceMgr.EventSelector.InitialTimeStamp =1688045568
  ServiceMgr.EventSelector.RunNumber =200805

#--------------------------------------------------------------
# Set output lvl (VERBOSE, DEBUG, INFO, WARNING, ERROR, FATAL)
#--------------------------------------------------------------
ServiceMgr.MessageSvc.OutputLevel = INFO
ServiceMgr.SCT_CablingSvc.OutputLevel = INFO
topSequence.SCT_TestCablingAlg.OutputLevel = INFO
ServiceMgr.MessageSvc.infoLimit        = 100000

#--------------------------------------------------------------
# Load IOVDbSvc
#--------------------------------------------------------------
IOVDbSvc.GlobalTag='OFLCOND-RUN12-SDR-25'
IOVDbSvc.OutputLevel = DEBUG

