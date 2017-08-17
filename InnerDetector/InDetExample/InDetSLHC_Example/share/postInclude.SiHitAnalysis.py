from SiHitAnalysis.SiHitAnalysisConf import SiHitAnalysis
topSequence+=SiHitAnalysis("SiHitAnalysis")

if not hasattr(ServiceMgr, 'THistSvc'):
    from GaudiSvc.GaudiSvcConf import THistSvc
    ServiceMgr += THistSvc()
 
ServiceMgr.THistSvc.Output  += ["HIT_NTUPLE DATAFILE='SiHitValid.root' OPT='RECREATE'"] 
