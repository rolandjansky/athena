from AtlasGeoModel.InDetGMJobProperties import InDetGeometryFlags
from HitAnalysis.HitAnalysisConf import SiHitAnalysis
topSequence += SiHitAnalysis('PixelHitAnalysis')
topSequence.PixelHitAnalysis.CollectionName='PixelHits'
topSequence += SiHitAnalysis('SCTHitAnalysis')
topSequence.SCTHitAnalysis.CollectionName='SCT_Hits'
topSequence.PixelHitAnalysis.HistPath='/SiHitAnalysis/'
if InDetGeometryFlags.isSLHC():
 topSequence.PixelHitAnalysis.isITK = True
topSequence.SCTHitAnalysis.HistPath='/SiHitAnalysis/'
if InDetGeometryFlags.isSLHC():
 topSequence.SCTHitAnalysis.isITK = True
topSequence.PixelHitAnalysis.ExpertMode= "off"
topSequence.SCTHitAnalysis.ExpertMode= "off"

if not hasattr(ServiceMgr, 'THistSvc'):
    from GaudiSvc.GaudiSvcConf import THistSvc
    ServiceMgr += THistSvc()

ServiceMgr.THistSvc.Output  += [ "SiHitAnalysis DATAFILE='SiHitValid.root' OPT='RECREATE'"]

