#-----------------------------------------------------------------------------
# Athena imports
#-----------------------------------------------------------------------------
from AthenaCommon.Constants import *
from AthenaCommon.AppMgr import theApp
from AthenaCommon.AppMgr import ServiceMgr
import AthenaPoolCnvSvc.ReadAthenaPool



# other imports
#from AthenaCommon.Preparation import *
#from AthenaCommon.Bootstrap import *
#from AthenaCommon.Atlas.UnixStandardJob import *
#from AthenaCommon.Execution import *
#from runargs.RAWtoESD import *
#from RecJobTransforms.skeleton.RAWtoESD_tf import *
#from RecJobTransforms.CommonRecoSkeletonJobOptions import *
#from RecExCommon.RecoUsefulFlags import *
#from RecExCommon.RecExCommon_topOptions import *
#from RecExCommon.RecExCommon_topOptions import *
#from RecExCond.RecExCommon_flags import *
#from RecExConfig.AutoConfigConsistencyCheck import *
#from RecExCommission.RecExCommission import *
#from RecExCommission.MinimalCommissioningSetup import *
#from TrigT1CTMonitoring.CTPFlags import *
#from RecExCond.RecExCommon_DetFlags import *
#from RecExCond.RecExCommon_GlobalFlags import *
#from RecExCond.AllDet_detDescr import *
#from IdDictDetDescrCnv.IdDictDetDescrCnv_joboptions import *
#from TileConditions.TileConditions_jobOptions import *
#from CaloIdCnv.CaloIdCnv_joboptions import *
#from CaloConditions.CaloConditions_jobOptions import *
#from CaloConditions.LArTTCellMap_ATLAS_jobOptions import *
#from CaloConditions.CaloTTIdMap_ATLAS_jobOptions import *
#from TileIdCnv.TileIdCnv_jobOptions import *
#from LArDetDescr.LArDetDescr_joboptions import *
#from LArConditionsCommon.LArConditionsCommon_comm_jobOptions import *
#from LArConditionsCommon.LArIdMap_comm_jobOptions import *
#from InDetBeamSpotService.BeamCondSvc import *
#from AmdcAth.AmdcAth_jobOptions import *
#from PartPropSvc.PartPropSvc import *
#from DetectorStatus.DetStatusSvc_CondDB import *
#from RecExPers.RecoOutputMetadataList_jobOptions import *
#from RecExCommon.BSRead_config import *
#from InDet.RecCabling import *
#from InDetRecExample.InDetReadBS_jobOptions import *
#from InDetRecExample.InDetRecCabling import *
#from MuonCnvExample.MuonReadBS_jobOptions import *
#from TrigT1CaloByteStream.ReadLVL1CaloBS_jobOptions import *
#from TrigT1CaloByteStream.ReadLVL1CaloBSRun2_jobOptions import *
#from L1TopoByteStream.ReadLVL1CaloBSRun2_jobOptions import *
#from LUCID_RawDataByteStreamCnv.LUCID_ByteStreamRawDataCnv import *
#from ZdcByteStream.ZdcByteStreamRawDataCnvV2 import *
#from TriggerRelease.BStoESD_Tier0_HLTConfig_jobOptions import *
#from TrigTier0.BStoESD_Tier0_HLTConfig_jobOptions import *
#from TrigTier0.TriggerConfigCheckHLTpsk import *
#from TrigTier0.TriggerConfigCheckHLTpsk import *
#from LumiBlockComps.LumiBlockMuWriter_jobOptions import *
#from RecExCommon.SystemRec_config import *
#from CaloRec.CaloRec_jobOptions import *
#from LArROD.LArFebErrorSummaryMaker_jobOptions import *
#from TileRec.TileDefaults_jobOptions import *
#from LArClusterRec.LArCluster_jobOptions import *
#from LArClusterRec.LArDigits_fromEMCluster_jobptions import *
#from CaloRec.CaloTopoCluster_jobOptions import *
#from CaloRec.EMTopoCluster_jobOptions import *
#from TileMuId.TileMuId_jobOptions import *
#from LArAffectedRegion.LArAffectedRegion_write import *
#from TileRecAlgs.MBTSTimeDiffEventInfoAlg_jobOptions import *
#from LArCellRec.LArTimeVetoAlg_jobOptions import *
#from TrigT1CaloCalibTools.DecorateL1CaloTriggerTowers_prodJobOFragment import *
#from TrigT1CaloCalibConditions.L1CaloCalibConditions_jobOptions import *
#from TrigT1CaloCalibConditions.L1CaloCalibConditionsRun2_jobOptions import *
#from TrigT1CaloCalibConditions.L1CaloCalibConditionsTier0Run2_jobOptions import *
#from InDetRecExample.InDetRec_jobOptions import *
#from InDetRecExample.InDetRecCaloSeededROISelection import *
#from InDetRecExample.InDetRecConditionsAccess import *
#from InDetRecExample.InDetRecLoadTools import *
#from InDetRecExample.InDetRecPreProcessingSilicon import *
#from InDetRecExample.ConfiguredInDetPreProcessingTRT import *
#from InDetRecExample.ConfiguredNewTrackingSiPattern import *
#from InDetRecExample.ConfiguredNewTrackingTRTExtension import *
#from InDetRecExample.ConfiguredTRTSegmentFinding import *
#from InDetRecExample.ConfiguredBackTracking import *
#from InDetRecExample.ConfiguredTRTStandalone import *
#from InDetRecExample.InDetxAODCreator import *
#from InDetRecExample.InDetRecPostProcessing import *
#from InDetRecExample.ConfiguredInDetTRTPhase import *
#from InDetRecExample.ConfiguredPriVtxAndPartCreation import *
#from InDetRecExample.ConfiguredLowBetaFinder import *
#from InDetRecExample.ConfiguredInDetValidation import *
#from FTK_RecExample.FTKRec_jobOptions import *
#from MuonRecExample.MuonRec_jobOptions import *
#from MuonRecExample.MuonRDO_to_PRD_jobOptions import *
#from MuonRecExample.MuonRecLoadTools import *
#from ForwardRec.ForwardRec_jobOptions import *
#from ForwardRec.LUCID_Rec_jobOptions import *
#from RecExCommon.CombinedRec_config import *
#from egammaRec.egammaRec_jobOptions import *
#from MuonCombinedRecExample.MuonCombinedRec_config import *
#from MuonCombinedRecExample.MuonCombinedRec_preprocessing import *
#from MuonCombinedRecExample.MuonCombinedRec_identification import *
#from MuonCombinedRecExample.MuonCombinedRec_postprocessing import *
#from MuonCombinedRecExample.MuonAODto_xAODCnv_jobOptions import *
#from eflowRec.eflowRec_jobOptions import *
#from eflowRec.eflowRec_config_MC15_JetETMiss import *
#from JetRec.JetRec_jobOptions import *
#from BTagging.BTagging_jobOptions import *
#from BTagging.BTagCalibBroker_jobOptions import *
#from JetTagCalibration.BTagCalibrationBroker_jobOptions import *
#from tauRec.tauRec_config import *
#from tauRec.tauRec_jobOptions import *
#from tauRec.Pi0ClusterMaker_jobOptions import *
#from PanTauAnalysis.JobOptions_Main_PanTau import *
#from TauDiscriminant.TauDiscri_jobOptions import *
#from DiTauRec.DiTauRec_config import *
#from DiTauRec.DiTauRec_jobOptions import *
#from METReconstruction.METReconstruction_jobOptions import *
#from EventTagRawAlgs.RawInfoSummaryForTagWriter_jobOptions import *
#from LArCellRec.LArCollisionTime_jobOptions import *
#from RecBackgroundAlgs.RecBackground_jobOptions import *
#from JiveXML.JiveXML_RecEx_config import *
#from JiveXML.JiveXML_jobOptionBase import *
#from RecJiveXML.RecJiveXML_DataTypes import *
#from TruthJiveXML.TruthJiveXML_DataTypes import *
#from InDetJiveXML.InDetJiveXML_DataTypes import *
#from TrkJiveXML.TrkJiveXML_DataTypes import *
#from CaloJiveXML.CaloJiveXML_DataTypes import *
#from MuonJiveXML.MuonJiveXML_DataTypes import *
#from AnalysisJiveXML.AnalysisJiveXML_DataTypes import *
#from xAODJiveXML.xAODJiveXML_DataTypes import *
#from TrigJiveXML.TrigJiveXML_DataTypes import *
#from EventTagAlgs.GlobalEventTagBuilder_jobOptions import *
#from EventTagUtils.GlobalEventTagTool_jobOptions import *
#from EventTagUtils.EventInfoTagTool_jobOptions import *
#from LumiBlockComps.CreateLumiBlockFromFile_jobOptions import *
#from RecExPers.RecoOutputESDList_jobOptions import *
#from EventAthenaPool.EventAthenaPoolItemList_joboptions import *
#from EventTagAthenaPOOL.EventTagAthenaPOOLItemList_joboptions import *
#from InDetRecExample.WriteInDetESD import *
#from FTK_RecExample.WriteFTK_ESD import *
#from CaloRecEx.CaloRecOutputItemList_jobOptions import *
#from MuonRecExample.MuonRecOutputItemList_jobOptions import *
#from tauRec.TauESDList import *
#from DiTauRec.DiTauESDList import *
#from METReconstruction.METReconstructionOutputESDList_jobOptions import *
#from BTagging.BTaggingReconstructionOutputAODList_jobOptions import *
#from IsolationAlgs.IsoEventShapeOutputItemList_jobOptions import *
#from egammaRec.egammaOutputItemList_jobOptions import *
#from eflowRec.eflowRecESDList import *
#from MuonCombinedRecExample.MuonCombined_OutputItemsESD import *
#from MuonCombinedRecExample.MuonCombined_OutputItemsAOD import *
#from RecExCommon.RecoUtils import *
#from PerfMonComps.PerfMonSvc_jobOptions import *
#from RecExCommon.PrintRecoSummary import *
#from AthenaCommon.runbatch import *
#from AthenaCommon.Preparation import *
#from AthenaCommon.Bootstrap import *
#from AthenaCommon.Atlas.UnixStandardJob import *
#from AthenaCommon.Execution import *
#from AthenaCommon.runbatch import *





from AthenaCommon.AlgSequence import AlgSequence
job = AlgSequence()
 
#-----------------------------------------------------------------------------
# Message Service
#-----------------------------------------------------------------------------
# Set output level threshold (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
ServiceMgr.MessageSvc.OutputLevel = WARNING
import AthenaServices
AthenaServices.AthenaServicesConf.AthenaEventLoopMgr.OutputLevel = WARNING
 
#-----------------------------------------------------------------------------
# Input Datasets
#-----------------------------------------------------------------------------
ServiceMgr.EventSelector.InputCollections = [
    '/data3/tkaji/dEdxAnalysis/WorkArea/run/InputLink.root'
    ]
#    '/data/data3/zp/sawada/tutorial/chain/data15_13TeV.00266904.physics_Main.merge.ZMUMU.f594_m1440._0001.3.aod.idtrkvalid.pool.root'
#    '/data/data3/zp/sawada/tutorial/chain/mc15_13TeV.403215.MGPy8EG_A14N23LO_GG_mixedC1LLP_0p2_1800_500.simul.HITS.e4894_s2698.aod.idtrkvalid.pool.root'
#    '/data/data3/zp/sawada/data/aod/data15_13TeV.00266904.physics_Main.merge.DRAW_ZMUMU.f594_m1440._0001.3.AOD.IDTRKVALID.pool.root'
#    '/data/data3/zp/sawada/tutorial/chain/yourOutputAOD.pool.root'
theApp.EvtMax = -1 # -1 means all events


from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
athenaCommonFlags.FilesInput = svcMgr.EventSelector.InputCollections
from RecExConfig.InputFilePeeker import inputFileSummary
from AthenaCommon.GlobalFlags import globalflags
globalflags.DetGeo = 'atlas'
#commented out by tkaji
globalflags.DataSource = 'data' if inputFileSummary['evt_type'][0] == "IS_DATA" else 'geant4'
globalflags.DetDescrVersion = inputFileSummary['geometry']
from RecExConfig import AutoConfiguration
#globalflags.DetDescrVersion = 'ATLAS-R2-2015-03-01-00'
AutoConfiguration.ConfigureSimulationOrRealData() 
AutoConfiguration.ConfigureGeo() 
#here is an example of the inputFilePeeker to autoconfigure the flags
from AthenaCommon.DetFlags import DetFlags
#DetFlags.detdescr.all_setOff() #skip this line out to leave everything on. But this will take longer
#DetFlags.detdescr.Calo_setOn() #e.g. if I am accessing CaloCellContainer, I need the calo detector description
include("RecExCond/AllDet_detDescr.py")


#-----------------------------------------------------------------------------
# Algorithms
#-----------------------------------------------------------------------------
from PixelStubletsMaker.PixelStubletsMakerConf import *
 
job += PixelStubletsMakerAlg('PixelStubletsMakerAlg1', OutputLevel = INFO)
#job += PixelStubletsMakerAlg('PixelStubletsMakerAlg2', OutputLevel = FATAL)
#job += PixelStubletsMakerAlg('PixelStubletsMakerAlg3', OutputLevel = INFO)
 
print job


#--------------------------------------------------------------
# Access to IOVSvc, IOVDbSvc and CondDBMySQLCnvSvc
#--------------------------------------------------------------
#import IOVDbSvc.IOVDb
#svcMgr.IOVDbSvc.OutputLevel        = DEBUG
#mytestalg.CalibrationFile="mcpar_cb.txt"
#mytestalg.TagID="PixdEdx_January2011_mc-000-00"
# Set time to register - used for IOVDbTestAmdbCorrection
#--------------------------------------------------------------
# For IOVDb: specify dbConnection with COOL and oracle specified
#
#   Default is to use oracle schema named TESTCOOL, to override this use
#     athena  'TESTCOOL="<your test db name>"' IOVDbTestAlgWriteCool.py
#
#try:
#    svcMgr.IOVDbSvc.dbConnection  = "impl=cool;techno=oracle;devdb10:"
#    svcMgr.IOVDbSvc.dbConnection  += TESTCOOL
#    svcMgr.IOVDbSvc.dbConnection  += ":ATLAS_COOLTEST:devdb42cool"
#except:
#    svcMgr.IOVDbSvc.dbConnection  = "impl=cool;techno=oracle;devdb10:TESTCOOL:ATLAS_COOLTEST:devdb42cool"

# uncomment this to use a local SQLite file instead
#svcMgr.IOVDbSvc.dbConnection  = "sqlite://;schema=mytest.db;dbname=OFLP200"

#--------------------------------------------------------------
# Options for IOVRegistrationSvc
import RegistrationServices.IOVRegistrationSvc


# Dont set the tag for ALL folders
# in COOL folder tags must be globally unique
regSvc = svcMgr.IOVRegistrationSvc
# regSvc.IOVDbTag   = "DC1"
regSvc.OutputLevel=2

# Select the following to delete and recreate the folders. Default is
# NOT to recreate
regSvc.RecreateFolders = TRUE
# The following set the interval for each of the IOVDbTest folders
# regSvc.BeginRun   = 4
# Max EndRun: 0x7FFFFFFF
# regSvc.EndRun     = 2147483647 
# regSvc.EndRun     = 3 
# regSvc.BeginLB = 0
# Max EndLB: 0xFFFFFFFF
# regSvc.EndLB   = 4294967295
# regSvc.IOVDbTag   = "DC1"

# extra write list to test all AttributeList types
# IOVDbTestAlg.FancyList=True
# regSvc.OverrideNames+=['FanSmallString','FanBigString']
# regSvc.OverrideTypes+=['String255','String64k']

#--------------------------------------------------------------
# Load POOL support
#--------------------------------------------------------------
import AthenaPoolCnvSvc.WriteAthenaPool

#--------------------------------------------------------------
# AthenaPool details
#--------------------------------------------------------------

# Check the dictionary in memory for completeness
svcMgr.AthenaSealSvc.CheckDictionary = False


#from IOVDbSvc.CondDB import conddb
#if not conddb.folderRequested('PIXEL/PixReco'):
#    conddb.addFolder('PIXEL_OFL','/PIXEL/PixReco')

#
# Configurable version of PixelToTPIDTOol
#
#InDetPixelToTPIDTool = None
#from PixelToTPIDTool.PixelToTPIDToolConf import InDet__PixelToTPIDTool
#InDetPixelToTPIDTool = InDet__PixelToTPIDTool(name = "InDetPixelToTPIDTool")
#InDetPixelToTPIDTool.CalibrationFile="mcpar_signed_234.txt"

#ToolSvc += InDetPixelToTPIDTool
#print  InDetPixelToTPIDTool


## name of the tag to read from database
#if not 'MYTAG' in dir() or MYTAG=='':
# MYTAG="PixelOfflineReco-Extended-000-01"

## database connection
#if not 'DATABASE' in dir() or DATABASE=='':
# DATABASE = "PIXEL_OFL"
## other useful examples:
#DATABASE = "LOCAL"
#DATABASE = "mycool.db;dbname=COMP200"
#DATABASE = "mycool.db;dbname=OFLP200"

## configure tool to access database
#from PixelConditionsTools.PixelConditionsToolsConf import PixelRecoDbTool
#ToolSvc += PixelRecoDbTool()
#ToolSvc.PixelRecoDbTool.InputSource = 2
#ToolSvc.PixelRecoDbTool.OutputLevel = DEBUG
#ToolSvc.PixelRecoDbTool.PixelChargeInterpolationDataFile = \
#	"PixelChargeInterpolationData-" + MYTAG.replace("PixelOfflineReco-","")
#ToolSvc.PixelRecoDbTool.PixelClusterOnTrackErrorDataFile = \
#	"PixelClusterOnTrackErrorData-" + MYTAG.replace("PixelOfflineReco-","") 
#ToolSvc.PixelRecoDbTool.PixelClusterErrorDataFile = \
#	"PixelClusterErrorData-" + MYTAG.replace("PixelOfflineReco-","") 

### configure tag and folder in database
#from IOVDbSvc.CondDB import conddb
#conddb.addFolder(DATABASE,"/PIXEL/PixReco <tag>"+MYTAG+"</tag>")

#DetDescrCnvSvc = Service( "DetDescrCnvSvc" )
#DetDescrCnvSvc.CaloIDFileName = "IdDictCalorimeter_DC3-05.xml"
#DetDescrCnvSvc.LArIDFileName = "IdDictLArCalorimeter_DC3-05.xml" 
#DetDescrCnvSvc.DoIdChecks

#-----------------------------------------------------------------------------
