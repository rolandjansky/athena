from AthenaCommon.AppMgr                    import theApp
from AthenaCommon.AppMgr                    import ServiceMgr as svcMgr
from AthenaCommon.AlgSequence               import AlgSequence
from GaudiSvc.GaudiSvcConf                  import THistSvc
import AthenaRootComps.ReadAthenaxAODHybrid

# ART File
testFile = 'ART_Rec.root'
svcMgr.EventSelector.InputCollections = [testFile]

job = CfgMgr.AthSequencer("AthAlgSeq")
job += CfgMgr.ArtTest()

theApp.EvtMax = 2000
svcMgr.THistSvc.Output += ["MONITORING DATAFILE='ART-monitoring.root' OPT='RECREATE'"]


print ( "\n\nALL OK\n\n" )
