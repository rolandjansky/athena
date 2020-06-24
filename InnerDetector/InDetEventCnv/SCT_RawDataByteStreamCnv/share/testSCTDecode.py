###############################################################
#
# Job options file to test SCT decoding
#
#==============================================================

if not "doPrint" in dir():
    doPrint = False
if not "doDump" in dir():
    doDump = False
if not "EvtMax" in dir():
    EvtMax = 10

msg = None
if doPrint:
    from AthenaCommon.Logging import logging
    msg = logging.getLogger("testSCTDecode")
    msg.setLevel(logging.INFO)

#--------------------------------------------------------------
# Standard includes
#--------------------------------------------------------------
import AthenaCommon.AtlasUnixStandardJob

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
globalflags.DetDescrVersion="ATLAS-R2-2016-01-00-01"
globalflags.DetGeo="atlas"
globalflags.InputFormat="bytestream"
globalflags.DataSource="data"
if doPrint:
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
DetFlags.makeRIO.SCT_setOn()
DetFlags.writeBS.all_setOff()
DetFlags.readRDOBS.all_setOff()
DetFlags.readRDOBS.SCT_setOn()
DetFlags.readRIOBS.all_setOff()
DetFlags.readRIOPool.all_setOff()
DetFlags.writeRIOPool.all_setOff()

if doPrint:
    DetFlags.Print()

import AtlasGeoModel.SetGeometryVersion
import AtlasGeoModel.GeoModelInit
import MagFieldServices.SetupField

#--------------------------------------------------------------
# Load IOVDbSvc
#--------------------------------------------------------------
IOVDbSvc = Service("IOVDbSvc")
from IOVDbSvc.CondDB import conddb
conddb.dbdata="CONDBR2"
IOVDbSvc.GlobalTag="CONDBR2-BLKPA-2018-03"
IOVDbSvc.OutputLevel = WARNING

# Set input byte stream file (from q431 test)
inputBSFiles = ["/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/Tier0ChainTests/data17_13TeV.00330470.physics_Main.daq.RAW._lb0310._SFO-1._0001.data"]
include("ByteStreamCnvSvc/BSEventStorageEventSelector_jobOptions.py")
ServiceMgr.EventSelector.Input = inputBSFiles
from AthenaCommon.AthenaCommonFlags  import athenaCommonFlags
athenaCommonFlags.FilesInput = inputBSFiles

from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

# Set up byte stream converters (SCTRawDataProvider, SCTRawDataProviderTool, SCT_RodDecoder, SCTEventFlagWriter)
include("InDetRecExample/InDetReadBS_jobOptions.py")
topSequence.InDetSCTRawDataProvider.OutputLevel = INFO
topSequence.InDetSCTRawDataProvider.ProviderTool.Decoder.OutputLevel = INFO
topSequence.InDetSCTEventFlagWriter.OutputLevel = INFO
if numThreads >= 2:
    topSequence.InDetSCTRawDataProvider.Cardinality = numThreads
    topSequence.InDetSCTEventFlagWriter.Cardinality = numThreads

# Set up SCT clsutering
from SiLorentzAngleTool.SCTLorentzAngleToolSetup import SCTLorentzAngleToolSetup
sctLorentzAngleToolSetup = SCTLorentzAngleToolSetup()
from SiClusterizationTool.SiClusterizationToolConf import InDet__ClusterMakerTool
InDetClusterMakerTool = InDet__ClusterMakerTool(name = "InDetClusterMakerTool",
                                                PixelCablingSvc = None,
                                                PixelModuleData = "",
                                                PixelChargeCalibCondData = "",
                                                PixelOfflineCalibData = '',
                                                PixelLorentzAngleTool = None,
                                                SCTLorentzAngleTool = sctLorentzAngleToolSetup.SCTLorentzAngleTool)
# SCT conditions setups
from SCT_ConditionsTools.SCT_ConfigurationConditionsToolSetup import SCT_ConfigurationConditionsToolSetup
sct_ConfigurationConditionsToolSetup = SCT_ConfigurationConditionsToolSetup()
sct_ConfigurationConditionsToolSetup.setup()
from SCT_ConditionsTools.SCT_ReadCalibDataToolSetup import SCT_ReadCalibDataToolSetup
sct_ReadCalibDataToolSetup = SCT_ReadCalibDataToolSetup()
sct_ReadCalibDataToolSetup.setup()
from SCT_ConditionsTools.SCT_MonitorConditionsToolSetup import SCT_MonitorConditionsToolSetup
sct_MonitorConditionsToolSetup = SCT_MonitorConditionsToolSetup()
sct_MonitorConditionsToolSetup.setup()
from SCT_ConditionsTools.SCT_ByteStreamErrorsToolSetup import SCT_ByteStreamErrorsToolSetup
sct_ByteStreamErrorsToolSetup = SCT_ByteStreamErrorsToolSetup()
sct_ByteStreamErrorsToolSetup.setConfigTool(sct_ConfigurationConditionsToolSetup.getTool())
sct_ByteStreamErrorsToolSetup.setup()
from SCT_ConditionsTools.SCT_DCSConditionsToolSetup import SCT_DCSConditionsToolSetup
sct_DCSConditionsToolSetup = SCT_DCSConditionsToolSetup()
sct_DCSConditionsToolSetup.setup()
from SCT_ConditionsTools.SCT_TdaqEnabledToolSetup import SCT_TdaqEnabledToolSetup
sct_TdaqEnabledToolSetup = SCT_TdaqEnabledToolSetup()
sct_TdaqEnabledToolSetup.setup()
from SCT_ConditionsTools.SCT_ConditionsSummaryToolSetup import SCT_ConditionsSummaryToolSetup
sct_ConditionsSummaryToolSetupWithoutFlagged = SCT_ConditionsSummaryToolSetup("InDetSCT_ConditionsSummaryToolWithoutFlagged")
sct_ConditionsSummaryToolSetupWithoutFlagged.setup()
sct_ConditionsSummaryToolSetupWithoutFlagged.ConditionsTools=[sct_ConfigurationConditionsToolSetup.getTool().getFullName(),
                                                              sct_ReadCalibDataToolSetup.getTool().getFullName(),
                                                              sct_MonitorConditionsToolSetup.getTool().getFullName(),
                                                              sct_ByteStreamErrorsToolSetup.getTool().getFullName(),
                                                              sct_DCSConditionsToolSetup.getTool().getFullName(),
                                                              sct_TdaqEnabledToolSetup.getTool().getFullName()]
from SiClusterizationTool.SiClusterizationToolConf import InDet__SCT_ClusteringTool
InDetSCT_ClusteringTool = InDet__SCT_ClusteringTool(name = "InDetSCT_ClusteringTool",
                                                    OutputLevel = INFO,
                                                    globalPosAlg = InDetClusterMakerTool,
                                                    timeBins = "01X",
                                                    conditionsTool = sct_ConditionsSummaryToolSetupWithoutFlagged.getTool())
from InDetPrepRawDataFormation.InDetPrepRawDataFormationConf import InDet__SCT_Clusterization
InDetSCT_Clusterization = InDet__SCT_Clusterization(name = "InDetSCT_Clusterization",
                                                    OutputLevel = INFO,
                                                    clusteringTool = InDetSCT_ClusteringTool,
                                                    conditionsTool = sct_ConditionsSummaryToolSetupWithoutFlagged.getTool(),
                                                    DataObjectName = "SCT_RDOs",
                                                    ClustersName = "SCT_Clusters",
                                                    maxFiredStrips = 384)
if numThreads >= 2:
    InDetSCT_Clusterization.Cardinality = numThreads

topSequence += InDetSCT_Clusterization

# Set up SCT AxAOD converters
from InDetPrepRawDataToxAOD.InDetPrepRawDataToxAODConf import SCT_RawDataToxAOD
xAOD_SCT_RawDataToxAOD = SCT_RawDataToxAOD(name = "SCTxAOD_SCT_RawDataToxAOD",
                                           OutputLevel = INFO)
topSequence += xAOD_SCT_RawDataToxAOD
from InDetPrepRawDataToxAOD.InDetPrepRawDataToxAODConf import SCT_PrepDataToxAOD
xAOD_SCT_PrepDataToxAOD = SCT_PrepDataToxAOD(name = "SCTxAOD_SCT_PrepDataToxAOD",
                                             OutputLevel = INFO,
                                             UseTruthInfo = False,
                                             WriteSDOs = False)
topSequence += xAOD_SCT_PrepDataToxAOD
if numThreads >= 2:
    xAOD_SCT_RawDataToxAOD.Cardinality = numThreads
    xAOD_SCT_PrepDataToxAOD.Cardinality = numThreads

# Set up SCT_ConditionsSummaryTestAlg with SCT_FlaggedConditionTool
from SCT_ConditionsTools.SCT_FlaggedConditionToolSetup import SCT_FlaggedConditionToolSetup
sct_FlaggedConditionToolSetup = SCT_FlaggedConditionToolSetup()
sct_FlaggedConditionToolSetup.setup()
sct_ConditionsSummaryToolSetup = SCT_ConditionsSummaryToolSetup()
sct_ConditionsSummaryToolSetup.setup()
SCT_ConditionsSummaryTool = sct_ConditionsSummaryToolSetup.getTool()
SCT_ConditionsSummaryTool.ConditionsTools=[sct_ConfigurationConditionsToolSetup.getTool().getFullName(),
                                           sct_ReadCalibDataToolSetup.getTool().getFullName(),
                                           sct_MonitorConditionsToolSetup.getTool().getFullName(),
                                           sct_ByteStreamErrorsToolSetup.getTool().getFullName(),
                                           sct_DCSConditionsToolSetup.getTool().getFullName(),
                                           sct_TdaqEnabledToolSetup.getTool().getFullName(),
                                           sct_FlaggedConditionToolSetup.getTool().getFullName()]
from SCT_ConditionsAlgorithms.SCT_ConditionsAlgorithmsConf import SCT_ConditionsSummaryTestAlg
topSequence += SCT_ConditionsSummaryTestAlg(SCT_ConditionsSummaryTool=SCT_ConditionsSummaryTool)
if numThreads >= 2:
    topSequence.SCT_ConditionsSummaryTestAlg.Cardinality = numThreads

# Set up space point formation
from AthenaCommon.AlgSequence import AthSequencer
condSeq = AthSequencer("AthCondSeq")
if not hasattr(condSeq, "InDetSiElementPropertiesTableCondAlg"):
    from SiSpacePointFormation.SiSpacePointFormationConf import InDet__SiElementPropertiesTableCondAlg
    condSeq += InDet__SiElementPropertiesTableCondAlg(name = "InDetSiElementPropertiesTableCondAlg")
from SiSpacePointTool.SiSpacePointToolConf import InDet__SiSpacePointMakerTool
InDetSiSpacePointMakerTool = InDet__SiSpacePointMakerTool(name = "InDetSiSpacePointMakerTool")
from SiSpacePointFormation.SiSpacePointFormationConf import InDet__SiTrackerSpacePointFinder
InDetSiTrackerSpacePointFinder = InDet__SiTrackerSpacePointFinder(name                   = "InDetSiTrackerSpacePointFinder",
                                                                  SiSpacePointMakerTool  = InDetSiSpacePointMakerTool,
                                                                  PixelsClustersName     = "",
                                                                  SCT_ClustersName       = "SCT_Clusters",
                                                                  SpacePointsPixelName   = "",
                                                                  SpacePointsSCTName     = "SCT_SpacePoints",
                                                                  SpacePointsOverlapName = "OverlapSpacePoints",
                                                                  ProcessPixels          = False,
                                                                  ProcessSCTs            = True,
                                                                  ProcessOverlaps        = False,
                                                                  OverrideBeamSpot       = True)
if numThreads >= 2:
    InDetSiTrackerSpacePointFinder.Cardinality = numThreads
topSequence += InDetSiTrackerSpacePointFinder

# Print algorithms
if doPrint:
    msg.info(topSequence)

# Set the number of events to be processed
theApp.EvtMax = EvtMax

# Output file
if doDump:
    from AthenaPoolCnvSvc.WriteAthenaPool import AthenaPoolOutputStream
    outStream = AthenaPoolOutputStream("OutStream", "testSCTDecode.pool.root")
    outStream.ItemList  = ["xAOD::EventInfo#EventInfo", "xAOD::EventAuxInfo#EventInfoAux."]
    outStream.ItemList += ["SCT_RDO_Container#SCT_RDOs"]
    outStream.ItemList += ["InDetBSErrContainer#SCT_ByteStreamErrs"]
    outStream.ItemList += ["InDet::SCT_ClusterContainer#SCT_Clusters"]
    outStream.ItemList += ["SCT_FlaggedCondData#SCT_FlaggedCondData"]
    # outStream.ItemList += ["SpacePointContainer#SCT_SpacePoints"]
    outStream.ItemList += ["xAOD::SCTRawHitValidationContainer#*", "xAOD::SCTRawHitValidationAuxContainer#*"]
    outStream.ItemList += ["xAOD::TrackMeasurementValidationContainer#*", "xAOD::TrackMeasurementValidationAuxContainer#*"]

#--------------------------------------------------------------
# Set output lvl (VERBOSE, DEBUG, INFO, WARNING, ERROR, FATAL)
#--------------------------------------------------------------
ServiceMgr.MessageSvc.OutputLevel = INFO

if numThreads >= 2:
    from SCT_ConditionsAlgorithms.SCTCondAlgCardinality import sctCondAlgCardinality
    sctCondAlgCardinality.set(numThreads)
