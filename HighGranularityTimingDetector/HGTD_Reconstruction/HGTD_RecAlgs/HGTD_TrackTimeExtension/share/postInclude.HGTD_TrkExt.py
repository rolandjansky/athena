##call clustermakertools before running extension when using RDO input

from HGTD_PadClusterizationTools.HGTD_PadClusterizationToolsConf import HGTD_ClusterMakerTool

cluster_maker_tool = HGTD_ClusterMakerTool(name="HGTD_ClusterMakerTool")
cluster_maker_tool.OutputLevel = DEBUG
ToolSvc += cluster_maker_tool

from HGTD_PadClusterizationTools.HGTD_PadClusterizationToolsConf import HGTD__SinglePadClusterTool

clusterization_tool = HGTD__SinglePadClusterTool(name="SinglePadClusterTool")
clusterization_tool.ClusterMakerTool = cluster_maker_tool
clusterization_tool.OutputLevel = DEBUG
ToolSvc += clusterization_tool


##FIXME NAME!! this needs change in the CMAKE
from HGTD_PRDFormation.PadClusterizationAlgConf import HGTD__PadClusterizationAlg

hgtd_cluster_maker_alg = HGTD__PadClusterizationAlg(name="PadClusterizationAlg")
hgtd_cluster_maker_alg.OutputLevel = DEBUG
hgtd_cluster_maker_alg.ClusterizationTool = clusterization_tool
hgtd_cluster_maker_alg.PRDContainerName = "HGTD_Cluster"

topSequence += hgtd_cluster_maker_alg

##continue with the extension

extrapolator = ToolSvc.AtlasExtrapolator

kalman_updator = ToolSvc.InDetUpdator

from HGTD_TimeCalibrationTools.HGTD_TimeCalibrationToolsConf import HGTD__StraightLineTOFcorrectionTool

hgtd_tof_corr_tool = HGTD__StraightLineTOFcorrectionTool(name="StraightLineTOFcorrectionTool")
hgtd_tof_corr_tool.OutputLevel = DEBUG

ToolSvc += hgtd_tof_corr_tool

from HGTD_TrackTimeExtensionTools.HGTD_TrackTimeExtensionToolsConf import HGTD_IterativeExtensionTool

htgd_ext_tool = HGTD_IterativeExtensionTool(name="IterativeExtensionTool")
htgd_ext_tool.OutputLevel = DEBUG
htgd_ext_tool.ExtrapolatorTool = extrapolator
htgd_ext_tool.UpdatorTool = kalman_updator
htgd_ext_tool.TOFCorrTool = hgtd_tof_corr_tool
htgd_ext_tool.Chi2Cut = 5.0

ToolSvc += htgd_ext_tool

from HGTD_TruthTools.HGTD_TruthToolsConf import HGTD__ClusterTruthTool

hgtd_cluster_truth_tool = HGTD__ClusterTruthTool(name="ClusterTruthTool")
hgtd_cluster_truth_tool.OutputLevel = DEBUG

ToolSvc += hgtd_cluster_truth_tool

##FIXME NAME!! this needs change in the CMAKE
from HGTD_TrackTimeExtension.TrackTimeExtensionAlgConf import HGTD__TrackTimeExtensionAlg

hgtd_alg = HGTD__TrackTimeExtensionAlg(name="TrackTimeExtensionAlg")
hgtd_alg.OutputLevel = DEBUG
hgtd_alg.TimeExtensionTool = htgd_ext_tool
hgtd_alg.HGTDClusterContainerName = "HGTD_Cluster"
hgtd_alg.TruthTool = hgtd_cluster_truth_tool

topSequence += hgtd_alg

# print "========= NOW SETTING UP FOR OUTPUT ============"

#   # --- load setup
from AthenaPoolCnvSvc.WriteAthenaPool import AthenaPoolOutputStream
  # --- check dictionary
ServiceMgr.AthenaSealSvc.CheckDictionary = True
  # --- commit interval (test)
ServiceMgr.AthenaPoolCnvSvc.CommitInterval = 10

# MC truth information
truthList = [ 'xAOD::TruthEventContainer#TruthEvents',
            'xAOD::TruthEventAuxContainer#TruthEventsAux.',
            'xAOD::TruthEventContainer#TruthEvents_PU',
            'xAOD::TruthEventAuxContainer#TruthEvents_PUAux.',
            'xAOD::TruthEventContainer#SpclMCPU',
            'xAOD::TruthEventAuxContainer#SpclMCPUAux.',
            'xAOD::TruthPileupEventContainer#TruthPileupEvents',
            'xAOD::TruthPileupEventAuxContainer#TruthPileupEventsAux.',
            'xAOD::TruthParticleContainer#TruthParticles',
            'xAOD::TruthParticleAuxContainer#TruthParticlesAux.',
            'xAOD::TruthVertexContainer#TruthVertices',
            'xAOD::TruthVertexAuxContainer#TruthVerticesAux.',
            'PileUpEventInfo#OverlayEvent' ]

# --- create stream
StreamESD            = AthenaPoolOutputStream ( "StreamESD2", "ESD.HGTD.full.pool.root",asAlg=True)
# --- save MC collections if truth turned on
StreamESD.ItemList += truthList
# ---- load list of objects
include ( "InDetRecExample/WriteInDetESD.py" )
StreamESD.ItemList  += InDetESDList
StreamESD.ItemList  += [ 'xAOD::EventInfo#EventInfo' , 'xAOD::EventAuxInfo#EventInfoAux.' ]
StreamESD.ForceRead = True # otherwise unread stuff is not copied

# StreamESD.ItemList  += InDetESDList;

#topSequence.StreamESD.ItemList+=[\"xAOD::EventInfo#EventInfo\", \"xAOD::EventAuxInfo#EventInfoAux.\"];topSequence.StreamESD.ItemList += [ \"xAOD::TrackParticleAuxContainer#InDetTrackParticlesAuxDyn.HGTD_*\"];topSequence.StreamESD.ForceRead = True;
