#----------------------------
# Algorithms
#----------------------------
from TrkClusterAnalysis.TrkClusterAnalysisConf import PixelClusterAnalysis
pixelClusterAnalysis = PixelClusterAnalysis()
pixelClusterAnalysis.NtupleFileName = '/PixelClusterAnalysis/'
pixelClusterAnalysis.HistPath = '/PixelClusterAnalysis/'
topSequence += pixelClusterAnalysis

from TrkClusterAnalysis.TrkClusterAnalysisConf import SCT_ClusterAnalysis
sctClusterAnalysis = SCT_ClusterAnalysis()
sctClusterAnalysis.NtupleFileName = '/SCT_ClusterAnalysis/'
sctClusterAnalysis.HistPath = '/SCT_ClusterAnalysis/'
topSequence += sctClusterAnalysis

#----------------------------
# Histogram and Tree Service
#----------------------------
if not hasattr(ServiceMgr, 'THistSvc'):
    from GaudiSvc.GaudiSvcConf import THistSvc
    ServiceMgr += THistSvc()
ServiceMgr.THistSvc.Output += ["PixelClusterAnalysis DATAFILE='PixelClusterAnalysis.root' OPT='RECREATE'"]
ServiceMgr.THistSvc.Output += ["SCT_ClusterAnalysis DATAFILE='SCT_ClusterAnalysis.root' OPT='RECREATE'"]
