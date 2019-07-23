#----------------------------
# Algorithms
#----------------------------
from RDOAnalysis.RDOAnalysisConf import PixelRDOAnalysis
topSequence += PixelRDOAnalysis()
PixelRDOAnalysis = PixelRDOAnalysis()
PixelRDOAnalysis.NtupleFileName = '/PixelRDOAnalysis/'
PixelRDOAnalysis.HistPath = '/PixelRDOAnalysis/'
PixelRDOAnalysis.DoITk = True
PixelRDOAnalysis.DoPosition = True

from RDOAnalysis.RDOAnalysisConf import SCT_RDOAnalysis
topSequence += SCT_RDOAnalysis()
SCT_RDOAnalysis = SCT_RDOAnalysis()
SCT_RDOAnalysis.NtupleFileName = '/SCT_RDOAnalysis/'
SCT_RDOAnalysis.HistPath = '/SCT_RDOAnalysis/'
SCT_RDOAnalysis.DoITk = True
SCT_RDOAnalysis.DoPosition = True

#----------------------------
# Histogram and Tree Service
#----------------------------
if not hasattr(ServiceMgr, 'THistSvc'):
    from GaudiSvc.GaudiSvcConf import THistSvc
    ServiceMgr += THistSvc()
ServiceMgr.THistSvc.Output += ["PixelRDOAnalysis DATAFILE='PixelRDOAnalysis.root' OPT='RECREATE'"]
ServiceMgr.THistSvc.Output += ["SCT_RDOAnalysis DATAFILE='SCT_RDOAnalysis.root' OPT='RECREATE'"]
