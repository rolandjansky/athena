# Digital clustering
# ToolSvc.InDetPixelClusterOnTrackTool.CorrectDigitalCentroid=True
# ToolSvc.InDetPixelClusterOnTrackTool.PositionStrategy=0
# ToolSvc.InDetPixelClusterOnTrackTool.ErrorStrategy=1
# ToolSvc.InDetPixelClusterOnTrackToolDigital.PositionStrategy=0
# ToolSvc.InDetPixelClusterOnTrackToolDigital.ErrorStrategy=1
# ToolSvc.InDetSCT_ClusterOnTrackTool.CorrectionStrategy=0
# ToolSvc.InDetSCT_ClusterOnTrackTool.ErrorStrategy=0

# Analogue Clustering
ToolSvc.InDetPixelClusterOnTrackTool.PositionStrategy=1
ToolSvc.InDetPixelClusterOnTrackTool.ErrorStrategy=2
ToolSvc.InDetPixelClusterOnTrackTool.ITkAnalogueClustering = True
#Run pattern recognition with digital errors
ToolSvc.InDetPixelClusterOnTrackToolDigital.PositionStrategy=0
ToolSvc.InDetPixelClusterOnTrackToolDigital.ErrorStrategy=1
ToolSvc.InDetSCT_ClusterOnTrackTool.CorrectionStrategy=0
ToolSvc.InDetSCT_ClusterOnTrackTool.ErrorStrategy=0

from AthenaCommon.AlgSequence import AthSequencer
condSeq = AthSequencer("AthAlgSeq") #should be changed to AthCondSeq in master
from AtlasGeoModel.InDetGMJobProperties import InDetGeometryFlags as geoFlags
if geoFlags.isSLHC() and not hasattr(condSeq, 'PixelITkOfflineCalibCondAlg'):
    from PixelConditionsAlgorithms.PixelConditionsAlgorithmsConf import PixelITkOfflineCalibCondAlg
    condSeq += PixelITkOfflineCalibCondAlg(name="PixelITkOfflineCalibCondAlg", ReadKey="/PIXEL/ITkClusterError")
    PixelITkOfflineCalibCondAlg.InputSource = 2

    from IOVDbSvc.CondDB import conddb
    from AthenaCommon.GlobalFlags import globalflags
    CoolDataBaseFolder = '/PIXEL/ITkClusterError'
    DetDescrVersion = globalflags.DetDescrVersion()

    if(DetDescrVersion.startswith('ATLAS-P2-ITK-17') or DetDescrVersion.startswith('ATLAS-P2-ITK-22')):
        ctag = 'PixelITkError_v3_' + DetDescrVersion
    else:
        ctag = 'PixelITkError_v4_' + DetDescrVersion

    cfoldertag = CoolDataBaseFolder+' <tag>'+ctag+'</tag>'
    conddb.addFolderSplitMC('PIXEL',cfoldertag,cfoldertag)
