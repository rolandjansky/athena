
################################################################################
################################################################################

## setup both tools used for clusterization in HGTD
from HGTD_PadClusterizationTools.HGTD_PadClusterizationToolsConf import HGTD_ClusterMakerTool

cluster_maker_tool = HGTD_ClusterMakerTool(name="HGTD_ClusterMakerTool")
ToolSvc += cluster_maker_tool

from HGTD_PadClusterizationTools.HGTD_PadClusterizationToolsConf import HGTD__SinglePadClusterTool

clusterization_tool = HGTD__SinglePadClusterTool(name="SinglePadClusterTool")
clusterization_tool.ClusterMakerTool = cluster_maker_tool
ToolSvc += clusterization_tool

## schedule the clusterisation tool to produce HGTD_Clusters out of HGTD_RDOs
from HGTD_PRDFormation.HGTD_PRDFormationConf import HGTD__PadClusterizationAlg

hgtd_cluster_maker_alg = HGTD__PadClusterizationAlg(name="PadClusterizationAlg")
hgtd_cluster_maker_alg.ClusterizationTool = clusterization_tool
hgtd_cluster_maker_alg.PRDContainerName = "HGTD_Cluster"

topSequence += hgtd_cluster_maker_alg

################################################################################
################################################################################
## setup of tools needed in the track extension

## to classify clusters as coming from HS/PU/... a specific tool is used
from HGTD_TruthTools.HGTD_TruthToolsConf import HGTD__ClusterTruthTool

hgtd_cluster_truth_tool = HGTD__ClusterTruthTool(name="ClusterTruthTool")

ToolSvc += hgtd_cluster_truth_tool


## from the tacking side, extrapolator and Kalman updator are needed
extrapolator = ToolSvc.AtlasExtrapolator

kalman_updator = ToolSvc.InDetUpdator

## beam service tool
from AthenaCommon.AppMgr import ServiceMgr
ServiceMgr += Service('BeamCondSvc')
beam_conditions_svc = ServiceMgr.BeamCondSvc

## for the time of flight correction, us the straight line at speed of light hypothesis
from HGTD_TimeCalibrationTools.HGTD_TimeCalibrationToolsConf import HGTD__StraightLineTOFcorrectionTool

hgtd_tof_corr_tool = HGTD__StraightLineTOFcorrectionTool(name="StraightLineTOFcorrectionTool")

ToolSvc += hgtd_tof_corr_tool

## the extension itself is done within a tool that is called in the algorithm
## configure this tool first
from HGTD_TrackTimeExtensionTools.HGTD_TrackTimeExtensionToolsConf import HGTD_IterativeExtensionTool

htgd_ext_tool = HGTD_IterativeExtensionTool(name="IterativeExtensionTool")
htgd_ext_tool.ExtrapolatorTool = extrapolator
htgd_ext_tool.UpdatorTool = kalman_updator
htgd_ext_tool.TOFCorrTool = hgtd_tof_corr_tool
# htgd_ext_tool.BeamCondSvc = beam_conditions_svc
htgd_ext_tool.ClusterTruthTool = hgtd_cluster_truth_tool
htgd_ext_tool.Chi2Cut = 5.0

ToolSvc += htgd_ext_tool

## schedule the extension algorithm from ITk to HGTD
from HGTD_TrackTimeExtension.HGTD_TrackTimeExtensionConf import HGTD__TrackTimeExtensionAlg

hgtd_alg = HGTD__TrackTimeExtensionAlg(name="TrackTimeExtensionAlg")
hgtd_alg.TimeExtensionTool = htgd_ext_tool
hgtd_alg.HGTDClusterContainerName = "HGTD_Cluster"
hgtd_alg.TruthTool = hgtd_cluster_truth_tool

topSequence += hgtd_alg
