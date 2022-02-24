#----------------------------
# Algorithms
#----------------------------
from TrkSpacePointAnalysis.TrkSpacePointAnalysisConf import SpacePointAnalysis
pixelSpacePointAnalysis = SpacePointAnalysis()
pixelSpacePointAnalysis.NtupleFileName = '/PixelSpacePointAnalysis/'
pixelSpacePointAnalysis.HistPath = '/PixelSpacePointAnalysis/'
pixelSpacePointAnalysis.UsePixel = True
topSequence += pixelSpacePointAnalysis

sctSpacePointAnalysis = SpacePointAnalysis()
sctSpacePointAnalysis.NtupleFileName = '/SCT_SpacePointAnalysis/'
sctSpacePointAnalysis.HistPath = '/SCT_SpacePointAnalysis/'
sctSpacePointAnalysis.UsePixel = False
topSequence += sctSpacePointAnalysis

#----------------------------
# Histogram and Tree Service
#----------------------------
if not hasattr(ServiceMgr, 'THistSvc'):
    from GaudiSvc.GaudiSvcConf import THistSvc
    ServiceMgr += THistSvc()
ServiceMgr.THistSvc.Output += ["PixelSpacePointAnalysis DATAFILE='PixelSpacePointAnalysis.root' OPT='RECREATE'"]
ServiceMgr.THistSvc.Output += ["SCT_SpacePointAnalysis DATAFILE='SCT_SpacePointAnalysis.root' OPT='RECREATE'"]
