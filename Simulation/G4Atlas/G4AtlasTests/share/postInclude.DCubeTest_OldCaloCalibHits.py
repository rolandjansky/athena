from AthenaCommon.AppMgr import ServiceMgr
if not hasattr(ServiceMgr, 'THistSvc'):
    from GaudiSvc.GaudiSvcConf import THistSvc
    ServiceMgr += THistSvc("THistSvc")
ServiceMgr.THistSvc.Output  += ["truth DATAFILE='truth.root' OPT='RECREATE'"]


# note: some of these tests will need an active DecDescManager
#       so please use something like
#             preExec='simFlags.ReleaseGeoModel=False;'
#       with your transform
from AthenaCommon.AlgSequence import AlgSequence
job = AlgSequence()

from G4AtlasApps.SimFlags import simFlags
simFlags.ReleaseGeoModel=False;

if not hasattr(job, 'G4TestAlg'):
    from G4AtlasTests.G4AtlasTestsConf import G4TestAlg
    job += G4TestAlg()
from AthenaCommon import CfgGetter
job.G4TestAlg.SimTestTools += [CfgGetter.getPrivateTool("TruthTestTool", checkType=True)]
job.G4TestAlg.SimTestTools += [CfgGetter.getPrivateTool("PixelHitsTestTool", checkType=True)]
job.G4TestAlg.SimTestTools += [CfgGetter.getPrivateTool("SCT_HitsTestTool", checkType=True)]
job.G4TestAlg.SimTestTools += [CfgGetter.getPrivateTool("TrtHitsTestTool", checkType=True)]
job.G4TestAlg.SimTestTools += [CfgGetter.getPrivateTool("EMBHitsTestTool", checkType=True)]
job.G4TestAlg.SimTestTools += [CfgGetter.getPrivateTool("EMECHitsTestTool", checkType=True)]
job.G4TestAlg.SimTestTools += [CfgGetter.getPrivateTool("FCALHitsTestTool", checkType=True)]
job.G4TestAlg.SimTestTools += [CfgGetter.getPrivateTool("HECHitsTestTool", checkType=True)]
job.G4TestAlg.SimTestTools += [CfgGetter.getPrivateTool("TileHitsTestTool", checkType=True)]
##job.G4TestAlg.SimTestTools += [CfgGetter.getPrivateTool("MDTHitsTestTool", checkType=True)]
##job.G4TestAlg.SimTestTools += [CfgGetter.getPrivateTool("RPCHitsTestTool", checkType=True)]
##job.G4TestAlg.SimTestTools += [CfgGetter.getPrivateTool("CSCHitsTestTool", checkType=True)]
##job.G4TestAlg.SimTestTools += [CfgGetter.getPrivateTool("TGCHitsTestTool", checkType=True)]
job.G4TestAlg.SimTestTools += [CfgGetter.getPrivateTool("LArActiveCaloCalibHitsTestTool", checkType=True)]
job.G4TestAlg.SimTestTools += [CfgGetter.getPrivateTool("LArInactiveCaloCalibHitsTestTool", checkType=True)]
##job.G4TestAlg.SimTestTools += [CfgGetter.getPrivateTool("LArDeadMaterialCaloCalibHitsTestTool", checkType=True)]
job.G4TestAlg.SimTestTools += [CfgGetter.getPrivateTool("TileCellCaloCalibHitsTestTool", checkType=True)]
##job.G4TestAlg.SimTestTools += [CfgGetter.getPrivateTool("TileDeadMaterialCaloCalibHitsTestTool", checkType=True)]
