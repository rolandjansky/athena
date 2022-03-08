#----------------------------
# Algorithms
#----------------------------
from TrkSpacePointAnalysis.TrkSpacePointAnalysisConf import SpacePointAnalysis
pixelSpacePointAnalysis = SpacePointAnalysis(name="PixelSpacePointAnalysis")
pixelSpacePointAnalysis.NtupleFileName = '/PixelSpacePointAnalysis/'
pixelSpacePointAnalysis.HistPath = '/PixelSpacePointAnalysis/'
pixelSpacePointAnalysis.InputKey = "PixelSpacePoints"
pixelSpacePointAnalysis.UsePixel = True
pixelSpacePointAnalysis.UseOverlap = False
topSequence += pixelSpacePointAnalysis

sctSpacePointAnalysis = SpacePointAnalysis(name="SCT_SpacePointAnalysis")
sctSpacePointAnalysis.NtupleFileName = '/SCT_SpacePointAnalysis/'
sctSpacePointAnalysis.HistPath = '/SCT_SpacePointAnalysis/'
sctSpacePointAnalysis.InputKey = "SCT_SpacePoints"
sctSpacePointAnalysis.InputOverlapKey = "OverlapSpacePoints"
sctSpacePointAnalysis.UsePixel = False
sctSpacePointAnalysis.UseOverlap = True
topSequence += sctSpacePointAnalysis


#----------------------------
# Histogram and Tree Service
#----------------------------
if not hasattr(ServiceMgr, 'THistSvc'):
    from GaudiSvc.GaudiSvcConf import THistSvc
    ServiceMgr += THistSvc()
ServiceMgr.THistSvc.Output += ["PixelSpacePointAnalysis DATAFILE='PixelSpacePointAnalysis.root' OPT='RECREATE'"]
ServiceMgr.THistSvc.Output += ["SCT_SpacePointAnalysis DATAFILE='SCT_SpacePointAnalysis.root' OPT='RECREATE'"]
