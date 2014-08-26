from AthenaCommon.AlgSequence import AlgSequence

topSequence = AlgSequence()


from PileupRTT.PileupRTTConf import PileupRTT__PileupMCEventCollectionChecker as PileupMCEventCollectionChecker

topSequence += PileupMCEventCollectionChecker("PileupMCEventCollectionChecker",
                                 OutputLevel = DEBUG)

#topSequence.PileupMCEventCollectionChecker.McEventCollection="GEN_AOD"
topSequence.PileupMCEventCollectionChecker.McEventCollection="TruthEvent"

# produce ROOT ntuple using THistSvc
from AthenaCommon.AppMgr import ServiceMgr
if not hasattr(ServiceMgr, 'THistSvc'):
    from GaudiSvc.GaudiSvcConf import THistSvc
    ServiceMgr += THistSvc()
ServiceMgr.THistSvc.Output+=[ "AANT DATAFILE='Out.root' OPT='RECREATE'"]

# get a handle on the ServiceManager which holds all the services
# from AthenaCommon.AppMgr import ServiceMgr

