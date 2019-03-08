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
globalflags.DetDescrVersion = "ATLAS-R2-2015-03-01-00"
globalflags.DetGeo="atlas"
globalflags.InputFormat="pool"
globalflags.DataSource="geant4"
print globalflags

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
# Load ReadCalibData Alg and Service
#--------------------------------------------------------------
DataSource = ''

from IOVDbSvc.CondDB import conddb

# letsTest='ReadingFromCoraCool'
# letsTest='ReadingFromFile'
# letsTest='ReadingFromMCFile' # Some ERROR messages are shown.
letsTest='ReadingFromCVP'
if (letsTest=='ReadingFromCoraCool'):
  conddb.addFolder("","<db>oracle://ATLAS_COOLPROD;schema=ATLAS_COOLOFL_SCT;dbname=OFLP200</db><tag>SctDaqConfigurationRod-PERFECT-Aug10-01</tag> /SCT/DAQ/Configuration/ROD", className="CondAttrListVec")
  conddb.addFolder("","<db>oracle://ATLAS_COOLPROD;schema=ATLAS_COOLOFL_SCT;dbname=OFLP200</db><tag>SctDaqConfigurationMur-PERFECT-Aug10-01</tag> /SCT/DAQ/Configuration/MUR", className="CondAttrListVec")
  conddb.addFolder("","<db>oracle://ATLAS_COOLPROD;schema=ATLAS_COOLOFL_SCT;dbname=OFLP200</db><tag>SctDaqConfigurationRodmur-PERFECT-Aug10-01</tag> /SCT/DAQ/Configuration/RODMUR", className="CondAttrListVec")
  conddb.addFolder("","<db>oracle://ATLAS_COOLPROD;schema=ATLAS_COOLOFL_SCT;dbname=OFLP200</db><tag>SctDaqConfigurationGeog-PERFECT-Aug10-01</tag> /SCT/DAQ/Configuration/Geog", className="CondAttrListVec")
  #conddb.addFolder("SCT","<db>COOLONL_SCT/CONDBR2</db> /SCT/DAQ/Configuration/ROD<forceTimestamp>1409896800</forceTimestamp>", className="CondAttrListVec")
  #conddb.addFolder("SCT","<db>COOLONL_SCT/CONDBR2</db> /SCT/DAQ/Configuration/RODMUR<forceTimestamp>1409896800</forceTimestamp>", className="CondAttrListVec")
  #conddb.addFolder("SCT","<db>COOLONL_SCT/CONDBR2</db> /SCT/DAQ/Configuration/MUR<forceTimestamp>1409896800</forceTimestamp>", className="CondAttrListVec")
  #conddb.addFolder("SCT","<db>COOLONL_SCT/CONDBR2</db> /SCT/DAQ/Configuration/Geog<forceTimestamp>1409896800</forceTimestamp>", className="CondAttrListVec")
  '''
  conddb.addFolder("SCT","/SCT/DAQ/Configuration/ROD <tag>HEAD</tag>", className="CondAttrListVec")
  conddb.addFolder("SCT","/SCT/DAQ/Configuration/MUR <tag>HEAD</tag>", className="CondAttrListVec")
  conddb.addFolder("SCT","/SCT/DAQ/Configuration/RODMUR <tag>HEAD</tag>", className="CondAttrListVec")
  conddb.addFolder("SCT","/SCT/DAQ/Configuration/Geog <tag>HEAD</tag>", className="CondAttrListVec")
  '''

  DataSource='CORACOOL'
elif (letsTest=='ReadingFromFile'):
  DataSource='SCT_Sept08Cabling_svc.dat'
  # File is available at https://svnweb.cern.ch/trac/atlasoff/browser/InnerDetector/InDetDetDescr/SCT_Cabling/trunk/share/
elif (letsTest=='ReadingFromMCFile'):
  DataSource='SCT_MC_FullCabling_svc.dat'
  # File is available at https://svnweb.cern.ch/trac/atlasoff/browser/InnerDetector/InDetDetDescr/SCT_Cabling/trunk/share/
elif (letsTest=='ReadingFromCVP'):
  DataSource='COOLVECTOR'
  #conddb.addFolder("","<db>sqlite://none;schema=/tmp/sroe/final.db;dbname=OFLP200</db> /SCT/DAQ/Config/ROD <tag>SctDaqConfigRod-MC-06</tag>", className="CondAttrListVec")
  #conddb.addFolder("","<db>sqlite://none;schema=/tmp/sroe/final.db;dbname=OFLP200</db> /SCT/DAQ/Config/MUR <tag>SctDaqConfigMur-MC-06</tag>", className="CondAttrListVec")
  #conddb.addFolder("","<db>sqlite://none;schema=/tmp/sroe/final.db;dbname=OFLP200</db> /SCT/DAQ/Config/RODMUR <tag>SctDaqConfigRodmur-MC-06</tag>", className="CondAttrListVec")
  #conddb.addFolder("","<db>sqlite://none;schema=/tmp/sroe/final.db;dbname=OFLP200</db> /SCT/DAQ/Config/Geog <tag>SctDaqConfigGeog-MC-06</tag>", className="CondAttrListVec")
  conddb.addFolder("SCT","<db>COOLOFL_SCT/OFLP200</db> /SCT/DAQ/Config/ROD <tag>SctDaqConfigRod-Run2-10July2015</tag>", className="CondAttrListVec")
  conddb.addFolder("SCT","<db>COOLOFL_SCT/OFLP200</db> /SCT/DAQ/Config/RODMUR <tag>SctDaqConfigRodmur-Run2-10July2015</tag>", className="CondAttrListVec")
  conddb.addFolder("SCT","<db>COOLOFL_SCT/OFLP200</db> /SCT/DAQ/Config/MUR <tag>SctDaqConfigMur-Run2-10July2015</tag>", className="CondAttrListVec")
  conddb.addFolder("SCT","<db>COOLOFL_SCT/OFLP200</db> /SCT/DAQ/Config/Geog <tag>SctDaqConfigGeog-Run2-10July2015</tag>", className="CondAttrListVec")

from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

if (letsTest=='ReadingFromCoraCool') or (letsTest=='ReadingFromCVP'):
  condAlgName = "SCT_CablingCondAlgFromCoraCool"
  from AthenaCommon.AlgSequence import AthSequencer
  condSeq = AthSequencer("AthCondSeq")
  if not hasattr(condSeq, condAlgName):
    if letsTest=='ReadingFromCVP':
      preFix = "/SCT/DAQ/Config/"
    else:
      preFix = "/SCT/DAQ/Configuration/"
    from SCT_Cabling.SCT_CablingConf import SCT_CablingCondAlgFromCoraCool
    condSeq += SCT_CablingCondAlgFromCoraCool(name = condAlgName,
                                              ReadKeyRod = preFix+"ROD",
                                              ReadKeyRodMur = preFix+"RODMUR",
                                              ReadKeyMur = preFix+"MUR",
                                              ReadKeyGeo = preFix+"Geog")
elif (letsTest=='ReadingFromFile') or (letsTest=='ReadingFromMCFile'):
  condAlgName = "SCT_CablingCondAlgFromText"
  from SCT_Cabling.SCT_CablingConf import SCT_CablingCondAlgFromText
  topSequence+= SCT_CablingCondAlgFromText(name = condAlgName,
                                           DataSource = DataSource)

from AthenaCommon.CfgGetter import getPrivateTool
SCT_CablingTool = getPrivateTool("SCT_CablingTool")
from SCT_Cabling.SCT_CablingConf import SCT_TestCablingAlg
topSequence+= SCT_TestCablingAlg(SCT_CablingTool=SCT_CablingTool)
topSequence.SCT_TestCablingAlg.SCT_CablingTool.DataSource = DataSource

#--------------------------------------------------------------
# Event selector settings. Use McEventSelector
#--------------------------------------------------------------
import AthenaCommon.AtlasUnixGeneratorJob
#ServiceMgr+= EventSelector()
#ServiceMgr.EventSelector.FirstEvent = 1
#ServiceMgr.EventSelector.EventsPerRun = 5
#ServiceMgr.EventSelector.RunNumber = 0
theApp.EvtMax                    = 20

#For real data, earliest timestamp is 0
#ServiceMgr.EventSelector.InitialTimeStamp = 1228950000
#3rd march 2010, 10am
#ServiceMgr.EventSelector.InitialTimeStamp =1267606800
ServiceMgr.EventSelector.InitialTimeStamp = 1409756400
if (letsTest=='ReadingFromCVP'):
  ServiceMgr.EventSelector.RunNumber = 310809
  # initial time stamp - this is number of seconds since 1st Jan 1970 GMT
  # run 310809 Recording start/end 2016-Oct-17 21:39:18 / 2016-Oct-18 16:45:23 UTC
  ServiceMgr.EventSelector.InitialTimeStamp  = 1476741326 # LB 18 of run 310809, 10/17/2016 @ 9:55pm (UTC)

#--------------------------------------------------------------
# Set output lvl (VERBOSE, DEBUG, INFO, WARNING, ERROR, FATAL)
#--------------------------------------------------------------
ServiceMgr.MessageSvc.OutputLevel = INFO
SCT_CablingTool.OutputLevel = INFO
topSequence.SCT_TestCablingAlg.OutputLevel = INFO
ServiceMgr.MessageSvc.infoLimit        = 100000

#--------------------------------------------------------------
# Load IOVDbSvc
#--------------------------------------------------------------
IOVDbSvc = Service("IOVDbSvc")
IOVDbSvc.GlobalTag='OFLCOND-RUN12-SDR-25'
IOVDbSvc.OutputLevel = DEBUG

