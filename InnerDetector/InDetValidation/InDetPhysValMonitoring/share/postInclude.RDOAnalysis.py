from RDOAnalysis.RDOAnalysisConf import PixelRDOAnalysis
topSequence += PixelRDOAnalysis()
PixelRDOAnalysis = PixelRDOAnalysis()
PixelRDOAnalysis.NtupleFileName = '/PixelRDOAnalysis/'
PixelRDOAnalysis.HistPath = '/PixelRDOAnalysis/'

from RDOAnalysis.RDOAnalysisConf import SCT_RDOAnalysis
topSequence += SCT_RDOAnalysis()
SCT_RDOAnalysis = SCT_RDOAnalysis()
SCT_RDOAnalysis.NtupleFileName = '/SCT_RDOAnalysis/'
SCT_RDOAnalysis.HistPath = '/SCT_RDOAnalysis/'

#----------------------------
# Histogram and Tree Service
#----------------------------
if not hasattr(ServiceMgr, 'THistSvc'):
   from GaudiSvc.GaudiSvcConf import THistSvc
   ServiceMgr += THistSvc()
ServiceMgr.THistSvc.Output += ["PixelRDOAnalysis DATAFILE='PixelRDOAnalysis.root' OPT='RECREATE'"]
ServiceMgr.THistSvc.Output += ["SCT_RDOAnalysis DATAFILE='SCT_RDOAnalysis.root' OPT='RECREATE'"]
