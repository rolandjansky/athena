from AthenaCommon.AppMgr import ServiceMgr 
if not hasattr(ServiceMgr, 'THistSvc'):
    from GaudiSvc.GaudiSvcConf import THistSvc 
    ServiceMgr += THistSvc("THistSvc") 
    ServiceMgr.THistSvc.Output = ["truth DATAFILE='truth.root' OPT='RECREATE'"];

from AthenaCommon.AlgSequence import AlgSequence 
job = AlgSequence() 

if not hasattr(job, 'G4TestAlg'):
    from G4AtlasTests.G4AtlasTestsConf import G4TestAlg 
    job += G4TestAlg()
from AthenaCommon import CfgGetter
job.G4TestAlg.SimTestTools += [CfgGetter.getPrivateTool("TruthTestTool", checkType=True)]
job.G4TestAlg.SimTestTools += [CfgGetter.getPrivateTool("ALFA_SimHitsTestTool", checkType=True)]
