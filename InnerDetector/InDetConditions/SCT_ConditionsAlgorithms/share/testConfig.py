test="run2MC"
useSlim=True

import AthenaCommon.AtlasUnixStandardJob

# Setup logger
from AthenaCommon.Logging import logging
msg = logging.getLogger("testConfig")
msg.setLevel(logging.INFO)

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

# use auditors
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
if test=="run1MC":
  globalflags.DetDescrVersion = "ATLAS-R1-2012-03-00-00"
  globalflags.DataSource="geant4"
elif test=="run2MC":
  globalflags.DetDescrVersion = "ATLAS-R2-2016-01-00-01"
  globalflags.DataSource="geant4"
elif test=="run2Data":
  globalflags.DetDescrVersion = "ATLAS-R2-2016-01-00-01"
  globalflags.DataSource="data"

globalflags.DetGeo="atlas"
globalflags.InputFormat="pool"
msg.info(globalflags)

#--------------------------------------------------------------
# Set Detector setup
#--------------------------------------------------------------
# --- switch on InnerDetector
from AthenaCommon.DetFlags import DetFlags 
DetFlags.detdescr.SCT_setOn()
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

from AthenaCommon.AlgSequence import AlgSequence
job = AlgSequence()

# Set up SCT cabling
from AthenaCommon.Include import include
include('InDetRecExample/InDetRecCabling.py')

#--------------------------------------------------------------
# Load IOVDbSvc
#--------------------------------------------------------------
IOVDbSvc = Service("IOVDbSvc")
from IOVDbSvc.CondDB import conddb
if (test=='run1MC'):
  IOVDbSvc.GlobalTag='OFLCOND-RUN12-SDR-25'
elif (test=='run2MC'):
  IOVDbSvc.GlobalTag='OFLCOND-RUN12-SDR-25'
elif (test=='run2Data'):
  IOVDbSvc.GlobalTag='CONDBR2-BLKPA-2018-14'

IOVDbSvc.OutputLevel = DEBUG

#
#NOTE: Testing with run2 requires presence of local sqlite file 'configTest.db'
# available from http://sroe.web.cern.ch/sroe/configTest.db
#
if (test=='run1MC'):
  #ToolSvc = ServiceMgr.ToolSvc
  conddb.addFolder("","<db>sqlite://none;schema=/tmp/sroe/mycool.db;dbname=LOCAL</db> /SCT/DAQ/Configuration/ROD <tag>SctDaqConfigurationRod-MC-06</tag><forceTimestamp>1333597198</forceTimestamp>")
  conddb.addFolder("","<db>sqlite://none;schema=/tmp/sroe/mycool.db;dbname=LOCAL</db> /SCT/DAQ/Configuration/MUR <tag>SctDaqConfigurationMur-MC-06</tag><forceTimestamp>1333597198</forceTimestamp>")
  conddb.addFolder("","<db>sqlite://none;schema=/tmp/sroe/mycool.db;dbname=LOCAL</db> /SCT/DAQ/Configuration/RODMUR <tag>SctDaqConfigurationRodmur-MC-06</tag><forceTimestamp>1333597198</forceTimestamp>")
  conddb.addFolder("","<db>sqlite://none;schema=/tmp/sroe/mycool.db;dbname=LOCAL</db> /SCT/DAQ/Configuration/Geog <tag>SctDaqConfigurationGeog-MC-06</tag><forceTimestamp>1333597198</forceTimestamp>")
  conddb.addFolder("","<db>sqlite://none;schema=/tmp/sroe/mycool.db;dbname=LOCAL</db> /SCT/DAQ/Configuration/Chip <tag>SctDaqConfigurationChip-MC-06</tag><forceTimestamp>1333597198</forceTimestamp>")
  conddb.addFolder("","<db>sqlite://none;schema=/tmp/sroe/mycool.db;dbname=LOCAL</db> /SCT/DAQ/Configuration/Module <tag>SctDaqConfigurationModule-MC-06</tag><forceTimestamp>1333597198</forceTimestamp>")
  '''
  conddb.addFolder("","<db>COOLOFL_SCT/OFLP200</db> /SCT/DAQ/Configuration/Chip <tag>SctDaqConfigurationChip-MC-06</tag> <forceTimestamp>1333597198</forceTimestamp>")
  conddb.addFolder("","<db>COOLOFL_SCT/OFLP200</db> /SCT/DAQ/Configuration/Module <tag>SctDaqConfigurationModule-MC-06</tag> <forceTimestamp>1333597198</forceTimestamp>")
  conddb.addFolder("","<db>COOLOFL_SCT/OFLP200</db> /SCT/DAQ/Configuration/MUR <tag>SctDaqConfigurationMur-MC-06</tag> <forceTimestamp>1333597198</forceTimestamp>")
  conddb.addFolder("","<db>COOLOFL_SCT/OFLP200</db> /SCT/DAQ/Configuration/ROD <tag>SctDaqConfigurationRod-MC-06</tag> <forceTimestamp>1333597198</forceTimestamp>")
  conddb.addFolder("","<db>COOLOFL_SCT/OFLP200</db> /SCT/DAQ/Configuration/Geog <tag>SctDaqConfigurationGeog-MC-06</tag> <forceTimestamp>1333597198</forceTimestamp>")
  conddb.addFolder("","<db>COOLOFL_SCT/OFLP200</db> /SCT/DAQ/Configuration/RODMUR <tag>SctDaqConfigurationRodmur-MC-06</tag> <forceTimestamp>1333597198</forceTimestamp>")
  '''
elif (test=='run2MC'):
  '''
  conddb.addFolder("","<db>sqlite://none;schema=/tmp/sroe/mycool.db;dbname=OFLP200</db> /SCT/DAQ/Config/ROD <tag>SctDaqConfigRod-MC-06</tag><forceRunNumber>20</forceRunNumber>")
  conddb.addFolder("","<db>sqlite://none;schema=/tmp/sroe/mycool.db;dbname=OFLP200</db> /SCT/DAQ/Config/MUR <tag>SctDaqConfigMur-MC-06</tag><forceRunNumber>20</forceRunNumber>")
  conddb.addFolder("","<db>sqlite://none;schema=/tmp/sroe/mycool.db;dbname=OFLP200</db> /SCT/DAQ/Config/RODMUR <tag>SctDaqConfigRodmur-MC-06</tag><forceRunNumber>20</forceRunNumber>")
  conddb.addFolder("","<db>sqlite://none;schema=/tmp/sroe/mycool.db;dbname=OFLP200</db> /SCT/DAQ/Config/Geog <tag>SctDaqConfigGeog-MC-06</tag><forceRunNumber>20</forceRunNumber>")
  conddb.addFolder("","<db>sqlite://none;schema=/tmp/sroe/mycool.db;dbname=OFLP200</db> /SCT/DAQ/Config/Chip <tag>SctDaqConfigChip-MC-06</tag><forceRunNumber>20</forceRunNumber>")
  conddb.addFolder("","<db>sqlite://none;schema=/tmp/sroe/mycool.db;dbname=OFLP200</db> /SCT/DAQ/Config/Module <tag>SctDaqConfigModule-MC-06</tag><forceRunNumber>20</forceRunNumber>")
  
  conddb.addFolder("","<db>sqlite://none;schema=/tmp/sroe/mycool7.db;dbname=OFLP200</db> /SCT/DAQ/Config/ROD <tag>SctDaqConfigRod-MC-06</tag><forceRunNumber>200805</forceRunNumber>")
  conddb.addFolder("","<db>sqlite://none;schema=/tmp/sroe/mycool7.db;dbname=OFLP200</db> /SCT/DAQ/Config/MUR <tag>SctDaqConfigMur-MC-06</tag><forceRunNumber>200805</forceRunNumber>")
  conddb.addFolder("","<db>sqlite://none;schema=/tmp/sroe/mycool7.db;dbname=OFLP200</db> /SCT/DAQ/Config/RODMUR <tag>SctDaqConfigRodmur-MC-06</tag><forceRunNumber>200805</forceRunNumber>")
  conddb.addFolder("","<db>sqlite://none;schema=/tmp/sroe/mycool7.db;dbname=OFLP200</db> /SCT/DAQ/Config/Geog <tag>SctDaqConfigGeog-MC-06</tag><forceRunNumber>200805</forceRunNumber>")
  conddb.addFolder("","<db>sqlite://none;schema=/tmp/sroe/mycool7.db;dbname=OFLP200</db> /SCT/DAQ/Config/Chip <tag>SctDaqConfigChip-MC-06</tag><forceRunNumber>200805</forceRunNumber>")
  conddb.addFolder("","<db>sqlite://none;schema=/tmp/sroe/mycool7.db;dbname=OFLP200</db> /SCT/DAQ/Config/Module <tag>SctDaqConfigModule-MC-06</tag><forceRunNumber>200805</forceRunNumber>")
  
  conddb.addFolder("","<db>COOLOFL_SCT/OFLP200</db> /SCT/DAQ/Config/Chip <tag>SctDaqConfigChip-MC-06</tag><forceRunNumber>200805</forceRunNumber>")
  conddb.addFolder("","<db>COOLOFL_SCT/OFLP200</db> /SCT/DAQ/Config/Module <tag>SctDaqConfigModule-MC-06</tag><forceRunNumber>200805</forceRunNumber>")
  conddb.addFolder("","<db>COOLOFL_SCT/OFLP200</db> /SCT/DAQ/Config/MUR <tag>SctDaqConfigMur-MC-06</tag><forceRunNumber>200805</forceRunNumber>")
  conddb.addFolder("","<db>COOLOFL_SCT/OFLP200</db> /SCT/DAQ/Config/ROD <tag>SctDaqConfigRod-MC-06</tag><forceRunNumber>200805</forceRunNumber>")
  conddb.addFolder("","<db>COOLOFL_SCT/OFLP200</db> /SCT/DAQ/Config/Geog <tag>SctDaqConfigGeog-MC-06</tag><forceRunNumber>200805</forceRunNumber>")
  conddb.addFolder("","<db>COOLOFL_SCT/OFLP200</db> /SCT/DAQ/Config/RODMUR <tag>SctDaqConfigRodmur-MC-06</tag><forceRunNumber>200805</forceRunNumber>")
  
  #test 'perfect' local db
  conddb.addFolder("","<db>sqlite://none;schema=output.db;dbname=CONDBR2</db> /SCT/DAQ/Config/ROD <tag>SctDaqConfigRod-Oct2016_00</tag><forceRunNumber>20</forceRunNumber>")
  conddb.addFolder("","<db>sqlite://none;schema=output.db;dbname=CONDBR2</db> /SCT/DAQ/Config/MUR <tag>SctDaqConfigMur-Oct2016_00</tag><forceRunNumber>20</forceRunNumber>")
  conddb.addFolder("","<db>sqlite://none;schema=output.db;dbname=CONDBR2</db> /SCT/DAQ/Config/RODMUR <tag>SctDaqConfigRodmur-Oct2016_00</tag><forceRunNumber>20</forceRunNumber>")
  conddb.addFolder("","<db>sqlite://none;schema=output.db;dbname=CONDBR2</db> /SCT/DAQ/Config/Geog <tag>SctDaqConfigGeog-Oct2016_00</tag><forceRunNumber>20</forceRunNumber>")
  conddb.addFolder("","<db>sqlite://none;schema=output.db;dbname=CONDBR2</db> /SCT/DAQ/Config/Chip <tag>SctDaqConfigChip-Oct2016_00</tag><forceRunNumber>20</forceRunNumber>")
  conddb.addFolder("","<db>sqlite://none;schema=output.db;dbname=CONDBR2</db> /SCT/DAQ/Config/Module <tag>SctDaqConfigModule-Oct2016_00</tag><forceRunNumber>20</forceRunNumber>")
  '''

  #test perfect DB on server
  # conddb.addFolder("","<db>COOLOFL_SCT/OFLP200</db> /SCT/DAQ/Config/Chip <tag>SctDaqConfigChip-PERFECT-Oct2016_00</tag><forceRunNumber>200805</forceRunNumber>", className="CondAttrListVec")
  # conddb.addFolder("","<db>COOLOFL_SCT/OFLP200</db> /SCT/DAQ/Config/Module <tag>SctDaqConfigModule-PERFECT-Oct2016_00</tag><forceRunNumber>200805</forceRunNumber>", className="CondAttrListVec")
  # conddb.addFolder("","<db>COOLOFL_SCT/OFLP200</db> /SCT/DAQ/Config/MUR <tag>SctDaqConfigMur-PERFECT-Oct2016_00</tag><forceRunNumber>200805</forceRunNumber>", className="CondAttrListVec")
  if not conddb.folderRequested("/SCT/DAQ/Config/ROD"):
    conddb.addFolder("","<db>COOLOFL_SCT/OFLP200</db> /SCT/DAQ/Config/ROD <tag>SctDaqConfigRod-PERFECT-Oct2016_00</tag><forceRunNumber>200805</forceRunNumber>")
  if not conddb.folderRequested("/SCT/DAQ/Config/Geog"):
    conddb.addFolder("","<db>COOLOFL_SCT/OFLP200</db> /SCT/DAQ/Config/Geog <tag>SctDaqConfigGeog-PERFECT-Oct2016_00</tag><forceRunNumber>200805</forceRunNumber>")
  if not conddb.folderRequested("/SCT/DAQ/Config/RODMUR"):
    conddb.addFolder("","<db>COOLOFL_SCT/OFLP200</db> /SCT/DAQ/Config/RODMUR <tag>SctDaqConfigRodmur-PERFECT-Oct2016_00</tag><forceRunNumber>200805</forceRunNumber>")
  

from SCT_ConditionsTools.SCT_ConfigurationConditionsToolSetup import SCT_ConfigurationConditionsToolSetup
sct_ConfigurationConditionsToolSetup = SCT_ConfigurationConditionsToolSetup()

if (test=='run1MC'):
  sct_ConfigurationConditionsToolSetup.setChannelFolderDb("<db>sqlite://none;schema=/tmp/sroe/mycool.db;dbname=LOCAL</db> /SCT/DAQ/Configuration/Chip <tag>SctDaqConfigurationChip-MC-06</tag><forceTimestamp>1333597198</forceTimestamp>")
  sct_ConfigurationConditionsToolSetup.setModuleFolderDb("<db>sqlite://none;schema=/tmp/sroe/mycool.db;dbname=LOCAL</db> /SCT/DAQ/Configuration/Module <tag>SctDaqConfigurationModule-MC-06</tag><forceTimestamp>1333597198</forceTimestamp>")
  sct_ConfigurationConditionsToolSetup.setMurFolderDb("<db>sqlite://none;schema=/tmp/sroe/mycool.db;dbname=LOCAL</db> /SCT/DAQ/Configuration/MUR <tag>SctDaqConfigurationMur-MC-06</tag><forceTimestamp>1333597198</forceTimestamp>")
  sct_ConfigurationConditionsToolSetup.setDbInstance("")
elif (test=='run2MC'):
  sct_ConfigurationConditionsToolSetup.setChannelFolderDb("<db>COOLOFL_SCT/OFLP200</db> /SCT/DAQ/Config/Chip <tag>SctDaqConfigChip-PERFECT-Oct2016_00</tag><forceRunNumber>200805</forceRunNumber>")
  sct_ConfigurationConditionsToolSetup.setModuleFolderDb("<db>COOLOFL_SCT/OFLP200</db> /SCT/DAQ/Config/Module <tag>SctDaqConfigModule-PERFECT-Oct2016_00</tag><forceRunNumber>200805</forceRunNumber>")
  sct_ConfigurationConditionsToolSetup.setMurFolderDb("<db>COOLOFL_SCT/OFLP200</db> /SCT/DAQ/Config/MUR <tag>SctDaqConfigMur-PERFECT-Oct2016_00</tag><forceRunNumber>200805</forceRunNumber>")
  sct_ConfigurationConditionsToolSetup.setDbInstance("")
elif (test=='run2Data'):
  sct_ConfigurationConditionsToolSetup.setChannelFolderDb("/SCT/DAQ/Config/Chip")
  sct_ConfigurationConditionsToolSetup.setModuleFolderDb("/SCT/DAQ/Config/Module")
  sct_ConfigurationConditionsToolSetup.setMurFolderDb("/SCT/DAQ/Config/MUR")

if useSlim:
  if (test=='run2MC'):
    sct_ConfigurationConditionsToolSetup.setChannelFolder("/SCT/DAQ/Config/ChipSlim")
    # conddb.blockFolder("/SCT/DAQ/Config/Chip")
    # conddb.addFolderWithTag("SCT_OFL","/SCT/DAQ/Config/Chip","SctDaqConfigChip-MC-14",force=True,forceMC=True,className="CondAttrListVec");
    # conddb.blockFolder("/SCT/DAQ/Config/ChipSlim")
    conddb.addFolderWithTag("SCT_OFL","/SCT/DAQ/Config/ChipSlim","SctDaqConfigChipSlim-MC-14",force=True,forceMC=True,className="CondAttrListVec");

sct_ConfigurationConditionsToolSetup.setup()

from SCT_ConditionsAlgorithms.SCT_ConditionsAlgorithmsConf import SCT_ConfigurationConditionsTestAlg
MyConfigTestAlg = SCT_ConfigurationConditionsTestAlg("MyConfigTestAlg")
MyConfigTestAlg.SCT_ConfigurationConditionsTool = sct_ConfigurationConditionsToolSetup.getTool()
job += MyConfigTestAlg

if numThreads >= 2:
  from SCT_ConditionsAlgorithms.SCTCondAlgCardinality import sctCondAlgCardinality
  sctCondAlgCardinality.set(numThreads)
  MyConfigTestAlg.Cardinality = numThreads

import AthenaCommon.AtlasUnixGeneratorJob
if (test=='run1MC') or (test=='run2MC'):
  ServiceMgr.EventSelector.RunNumber=200805
  ServiceMgr.EventSelector.InitialTimeStamp=1333597198
elif (test=='run2Data'):
  ServiceMgr.EventSelector.RunNumber=364214
  ServiceMgr.EventSelector.InitialTimeStamp=1540243090 # 2018-10-22T23:18:10+02:00, Run 364214, LB 51
theApp.EvtMax = 20

ServiceMgr.MessageSvc.Format = "% F%40W%S%7W%R%T %0W%M"
ServiceMgr.MessageSvc.OutputLevel = DEBUG
