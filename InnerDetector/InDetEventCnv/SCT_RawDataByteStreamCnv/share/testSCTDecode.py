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
ServiceMgr.ByteStreamInputSvc.FullFileName = inputBSFiles
from AthenaCommon.AthenaCommonFlags  import athenaCommonFlags
athenaCommonFlags.FilesInput = inputBSFiles

from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

# Set up event info cnv alg
from xAODEventInfoCnv.xAODEventInfoCnvConf import xAODMaker__EventInfoCnvAlg
topSequence += xAODMaker__EventInfoCnvAlg()

# Set up byte stream converters (SCTRawDataProvider, SCTRawDataProviderTool, SCT_RodDecoder)
include("InDetRecExample/InDetReadBS_jobOptions.py")
topSequence.InDetSCTRawDataProvider.OutputLevel = INFO
topSequence.InDetSCTRawDataProvider.ProviderTool.Decoder.OutputLevel = INFO

# Set up SCT clsutering
from SiLorentzAngleSvc.SCTLorentzAngleSvcSetup import sctLorentzAngleSvcSetup
from SiLorentzAngleSvc.PixelLorentzAngleSvcSetup import pixelLorentzAngleSvcSetup
from SiClusterizationTool.SiClusterizationToolConf import InDet__ClusterMakerTool
InDetClusterMakerTool = InDet__ClusterMakerTool(name = "InDetClusterMakerTool",
                                                PixelCalibSvc        = None,
                                                PixelOfflineCalibSvc = None,
                                                UsePixelCalibCondDB  = False)
ToolSvc += InDetClusterMakerTool
# SCT conditions setup
SCTConfigurationFolderPath='/SCT/DAQ/Config/'
SCTRodConfigPath=SCTConfigurationFolderPath+'ROD'
SCTMurConfigPath=SCTConfigurationFolderPath+'MUR'
SCTRodMurConfigPath=SCTConfigurationFolderPath+'RODMUR'
SCTGeogConfigPath=SCTConfigurationFolderPath+'Geog'
SCTChipConfigurationPath=SCTConfigurationFolderPath+'Chip'
SCTModuleConfigurationPath=SCTConfigurationFolderPath+'Module'
SCTMurConfigurationPath=SCTConfigurationFolderPath+'MUR'
if not conddb.folderRequested(SCTRodConfigPath):
    conddb.addFolderSplitMC("SCT",SCTRodConfigPath,SCTRodConfigPath)
if not conddb.folderRequested(SCTMurConfigPath):
    conddb.addFolderSplitMC("SCT",SCTMurConfigPath,SCTMurConfigPath)
if not conddb.folderRequested(SCTRodMurConfigPath):
    conddb.addFolderSplitMC("SCT",SCTRodMurConfigPath,SCTRodMurConfigPath)
if not conddb.folderRequested(SCTGeogConfigPath):
    conddb.addFolderSplitMC("SCT",SCTGeogConfigPath,SCTGeogConfigPath)
if not conddb.folderRequested(SCTChipConfigurationPath):
    conddb.addFolderSplitMC("SCT",SCTChipConfigurationPath,SCTChipConfigurationPath)
if not conddb.folderRequested(SCTModuleConfigurationPath):
    conddb.addFolderSplitMC("SCT",SCTModuleConfigurationPath,SCTModuleConfigurationPath)
if not conddb.folderRequested(SCTMurConfigurationPath):
    conddb.addFolderSplitMC("SCT",SCTMurConfigurationPath,SCTMurConfigurationPath)
from SCT_ConditionsServices.SCT_ConditionsServicesConf import SCT_ConfigurationConditionsSvc
InDetSCT_ConfigurationConditionsSvc = SCT_ConfigurationConditionsSvc(name = "InDetSCT_ConfigurationConditionsSvc")
ServiceMgr += InDetSCT_ConfigurationConditionsSvc

if not conddb.folderRequested('/SCT/DAQ/Calibration/NPtGainDefects'):
    conddb.addFolderSplitMC("SCT","/SCT/DAQ/Calibration/NPtGainDefects","/SCT/DAQ/Calibration/NPtGainDefects")
if not conddb.folderRequested('/SCT/DAQ/Calibration/NoiseOccupancyDefects'):
    conddb.addFolderSplitMC("SCT","/SCT/DAQ/Calibration/NoiseOccupancyDefects","/SCT/DAQ/Calibration/NoiseOccupancyDefects")
from SCT_ConditionsServices.SCT_ConditionsServicesConf import SCT_ReadCalibDataSvc
InDetSCT_ReadCalibDataSvc = SCT_ReadCalibDataSvc(name = "InDetSCT_ReadCalibDataSvc")
ServiceMgr += InDetSCT_ReadCalibDataSvc

if not conddb.folderRequested('/SCT/Derived/Monitoring'):
    conddb.addFolder("SCT_OFL","/SCT/Derived/Monitoring")
from SCT_ConditionsServices.SCT_ConditionsServicesConf import SCT_MonitorConditionsSvc
InDetSCT_MonitorConditionsSvc = SCT_MonitorConditionsSvc(name          = "InDetSCT_MonitorConditionsSvc",
                                                         OutputLevel   = INFO,
                                                         WriteCondObjs = False,
                                                         RegisterIOV   = False,
                                                         ReadWriteCool = True)
ServiceMgr += InDetSCT_MonitorConditionsSvc

from SCT_ConditionsServices.SCT_ConditionsServicesConf import SCT_ByteStreamErrorsSvc
SCT_ByteStreamErrorsSvc = SCT_ByteStreamErrorsSvc(name = "SCT_ByteStreamErrorsSvc")
ServiceMgr += SCT_ByteStreamErrorsSvc

if not conddb.folderRequested('/SCT/DCS/CHANSTAT'):
    conddb.addFolder("DCS_OFL","/SCT/DCS/CHANSTAT")
if not conddb.folderRequested('/SCT/DCS/MODTEMP'):
    conddb.addFolder("DCS_OFL","/SCT/DCS/MODTEMP")
if not conddb.folderRequested('/SCT/DCS/HV'):
    conddb.addFolder("DCS_OFL","/SCT/DCS/HV")
from SCT_ConditionsServices.SCT_ConditionsServicesConf import SCT_DCSConditionsSvc
InDetSCT_DCSConditionsSvc = SCT_DCSConditionsSvc(name = "InDetSCT_DCSConditionsSvc")        
ServiceMgr += InDetSCT_DCSConditionsSvc

tdaqFolder = '/TDAQ/EnabledResources/ATLAS/SCT/Robins'
if (conddb.dbdata == "CONDBR2"):
    tdaqFolder = '/TDAQ/Resources/ATLAS/SCT/Robins'
if not conddb.folderRequested(tdaqFolder):
    conddb.addFolder("TDAQ", tdaqFolder)
from SCT_ConditionsServices.SCT_ConditionsServicesConf import SCT_TdaqEnabledSvc
InDetSCT_TdaqEnabledSvc = SCT_TdaqEnabledSvc(name = "InDetSCT_TdaqEnabledSvc")
ServiceMgr += InDetSCT_TdaqEnabledSvc

from SCT_ConditionsServices.SCT_ConditionsServicesConf import SCT_FlaggedConditionSvc
InDetSCT_FlaggedConditionSvc = SCT_FlaggedConditionSvc(name = "InDetSCT_FlaggedConditionSvc")
ServiceMgr += InDetSCT_FlaggedConditionSvc

from SCT_ConditionsServices.SCT_ConditionsServicesConf import SCT_ConditionsSummarySvc
InDetSCT_ConditionsSummarySvc = SCT_ConditionsSummarySvc(name = "InDetSCT_ConditionsSummarySvc")
ServiceMgr += InDetSCT_ConditionsSummarySvc
InDetSCT_ConditionsSummarySvc.ConditionsServices= [ "InDetSCT_ConfigurationConditionsSvc",
                                                    "InDetSCT_ReadCalibDataSvc",
                                                    "InDetSCT_MonitorConditionsSvc",
                                                    "SCT_ByteStreamErrorsSvc",
                                                    "InDetSCT_DCSConditionsSvc",
                                                    "InDetSCT_TdaqEnabledSvc",
                                                    "InDetSCT_FlaggedConditionSvc"]

from SiClusterizationTool.SiClusterizationToolConf import InDet__SCT_ClusteringTool
InDetSCT_ClusteringTool = InDet__SCT_ClusteringTool(name = "InDetSCT_ClusteringTool",
                                                    globalPosAlg = InDetClusterMakerTool,
                                                    conditionsService = InDetSCT_ConditionsSummarySvc,
                                                    timeBins = "01X")
from InDetPrepRawDataFormation.InDetPrepRawDataFormationConf import InDet__SCT_Clusterization
InDetSCT_Clusterization = InDet__SCT_Clusterization(name = "InDetSCT_Clusterization",
                                                    clusteringTool = InDetSCT_ClusteringTool,
                                                    DataObjectName = "SCT_RDOs",
                                                    ClustersName = "SCT_Clusters",
                                                    maxRDOs = 384)
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

# Set up SCT_ConditionsSummaryTestAlg
from SCT_ConditionsServices.SCT_ConditionsServicesConf import SCT_ConditionsSummaryTestAlg
topSequence += SCT_ConditionsSummaryTestAlg(ConditionsSummarySvc=InDetSCT_ConditionsSummarySvc)

# Set up space point formation
from SiSpacePointTool.SiSpacePointToolConf import InDet__SiSpacePointMakerTool
InDetSiSpacePointMakerTool = InDet__SiSpacePointMakerTool(name = "InDetSiSpacePointMakerTool")
ToolSvc += InDetSiSpacePointMakerTool
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
