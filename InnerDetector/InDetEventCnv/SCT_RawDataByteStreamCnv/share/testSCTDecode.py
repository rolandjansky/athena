###############################################################
#
# Job options file to test SCT decoding
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
globalflags.DetDescrVersion="ATLAS-R2-2016-01-00-01"
globalflags.DetGeo="atlas"
globalflags.InputFormat="bytestream"
globalflags.DataSource="data"
print globalflags

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

# Set up event info cnv alg
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()
from xAODEventInfoCnv.xAODEventInfoCnvConf import xAODMaker__EventInfoCnvAlg
topSequence += xAODMaker__EventInfoCnvAlg()

# Set up byte stream converters (SCTRawDataProvider, SCTRawDataProviderTool, SCT_RodDecoder, SCTEventFlagWriter)
include("InDetRecExample/InDetReadBS_jobOptions.py")
topSequence.InDetSCTRawDataProvider.OutputLevel = DEBUG
topSequence.InDetSCTRawDataProvider.ProviderTool.Decoder.OutputLevel = INFO
topSequence.InDetSCTEventFlagWriter.OutputLevel = DEBUG

# Set up SCT clsutering
from SiLorentzAngleTool.SCTLorentzAngleToolSetup import SCTLorentzAngleToolSetup
from SiLorentzAngleTool.PixelLorentzAngleToolSetup import PixelLorentzAngleToolSetup
sctLorentzAngleToolSetup = SCTLorentzAngleToolSetup()
from SiClusterizationTool.SiClusterizationToolConf import InDet__ClusterMakerTool
InDetClusterMakerTool = InDet__ClusterMakerTool(name = "InDetClusterMakerTool",
                                                PixelCalibSvc        = None,
                                                UsePixelCalibCondDB  = False,
                                                PixelLorentzAngleTool = None,
                                                SCTLorentzAngleTool = sctLorentzAngleToolSetup.SCTLorentzAngleTool)
from SiClusterizationTool.SiClusterizationToolConf import InDet__SCT_ClusteringTool
InDetSCT_ClusteringTool = InDet__SCT_ClusteringTool(name = "InDetSCT_ClusteringTool",
                                                    OutputLevel = DEBUG,
                                                    globalPosAlg = InDetClusterMakerTool,
                                                    timeBins = "01X")
from InDetPrepRawDataFormation.InDetPrepRawDataFormationConf import InDet__SCT_Clusterization
InDetSCT_Clusterization = InDet__SCT_Clusterization(name = "InDetSCT_Clusterization",
                                                    OutputLevel = DEBUG,
                                                    clusteringTool = InDetSCT_ClusteringTool,
                                                    DataObjectName = "SCT_RDOs",
                                                    ClustersName = "SCT_Clusters",
                                                    maxFiredStrips = 384)
topSequence += InDetSCT_Clusterization

# Set up SCT AxAOD converters
from InDetPrepRawDataToxAOD.InDetPrepRawDataToxAODConf import SCT_RawDataToxAOD
xAOD_SCT_RawDataToxAOD = SCT_RawDataToxAOD(name = "SCTxAOD_SCT_RawDataToxAOD",
                                           OutputLevel = DEBUG)
topSequence += xAOD_SCT_RawDataToxAOD
from InDetPrepRawDataToxAOD.InDetPrepRawDataToxAODConf import SCT_PrepDataToxAOD
xAOD_SCT_PrepDataToxAOD = SCT_PrepDataToxAOD(name = "SCTxAOD_SCT_PrepDataToxAOD",
                                             OutputLevel = DEBUG,
                                             UseTruthInfo = False,
                                             WriteSDOs = False)
topSequence += xAOD_SCT_PrepDataToxAOD

# Set up SCT_ConditionsSummaryTestAlg with SCT_FlaggedConditionTool
from SCT_ConditionsTools.SCT_FlaggedConditionToolSetup import SCT_FlaggedConditionToolSetup
sct_FlaggedConditionToolSetup = SCT_FlaggedConditionToolSetup()
sct_FlaggedConditionToolSetup.setup()
from SCT_ConditionsTools.SCT_ConditionsSummaryToolSetup import SCT_ConditionsSummaryToolSetup
sct_ConditionsSummaryToolSetup = SCT_ConditionsSummaryToolSetup()
sct_ConditionsSummaryToolSetup.setup()
SCT_ConditionsSummaryTool = sct_ConditionsSummaryToolSetup.getTool()
SCT_ConditionsSummaryTool.ConditionsTools=[sct_FlaggedConditionToolSetup.getTool().getFullName()]
from SCT_ConditionsAlgorithms.SCT_ConditionsAlgorithmsConf import SCT_ConditionsSummaryTestAlg
topSequence += SCT_ConditionsSummaryTestAlg(SCT_ConditionsSummaryTool=SCT_ConditionsSummaryTool)

# Print algorithms
print topSequence

# SCT conditions setup
from SCT_ConditionsTools.SCT_ConfigurationConditionsToolSetup import SCT_ConfigurationConditionsToolSetup
sct_ConfigurationConditionsToolSetup = SCT_ConfigurationConditionsToolSetup()
sct_ConfigurationConditionsToolSetup.setup()

# Set the number of events to be processed
theApp.EvtMax = 20

#--------------------------------------------------------------
# Set output lvl (VERBOSE, DEBUG, INFO, WARNING, ERROR, FATAL)
#--------------------------------------------------------------
ServiceMgr.MessageSvc.OutputLevel = INFO
