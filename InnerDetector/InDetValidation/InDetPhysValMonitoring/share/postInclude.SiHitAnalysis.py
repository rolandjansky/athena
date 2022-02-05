from AtlasGeoModel.InDetGMJobProperties import InDetGeometryFlags
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()
from HitAnalysis.HitAnalysisConf import SiHitAnalysis
topSequence += SiHitAnalysis('PixelHitAnalysis')
topSequence.PixelHitAnalysis.CollectionName='PixelHits'
topSequence += SiHitAnalysis('SCTHitAnalysis')
topSequence.SCTHitAnalysis.CollectionName='SCT_Hits'
topSequence.PixelHitAnalysis.HistPath='/SiHitAnalysis/'
topSequence.SCTHitAnalysis.HistPath='/SiHitAnalysis/'
# Uncomment to enable expert mode
# topSequence.PixelHitAnalysis.ExpertMode= True
# topSequence.SCTHitAnalysis.ExpertMode= True

if not hasattr(ServiceMgr, 'THistSvc'):
    from GaudiSvc.GaudiSvcConf import THistSvc
    ServiceMgr += THistSvc()

ServiceMgr.THistSvc.Output  += [ "SiHitAnalysis DATAFILE='SiHitValid.root' OPT='RECREATE'"]
