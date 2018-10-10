from AthenaCommon.AppMgr import ServiceMgr
if not hasattr(ServiceMgr, 'THistSvc'):
    from GaudiSvc.GaudiSvcConf import THistSvc
    ServiceMgr += THistSvc("THistSvc")
ServiceMgr.THistSvc.Output += ["truth DATAFILE='truth.root' OPT='RECREATE'"]


# note: some of these tests will need an active DecDescManager
#       so please use something like
#             preExec='simFlags.ReleaseGeoModel=False;'
#       with your transform
from AthenaCommon.AlgSequence import AlgSequence
job = AlgSequence()

if not hasattr(job, 'G4TestAlg'):
    from G4AtlasTests.G4AtlasTestsConf import G4TestAlg
    job += G4TestAlg()
from AthenaCommon.DetFlags import DetFlags
from G4AtlasApps.SimFlags import simFlags
simFlags.ReleaseGeoModel=False;

from AthenaCommon import CfgGetter
if DetFlags.Truth_on():
    job.G4TestAlg.SimTestTools += [CfgGetter.getPrivateTool("TruthTestTool", checkType=True)]
    job.G4TestAlg.SimTestTools += [CfgGetter.getPrivateTool("BeamTruthTestTool", checkType=True)]
    job.G4TestAlg.SimTestTools += [CfgGetter.getPrivateTool("EvgenTruthTestTool", checkType=True)]
if DetFlags.pixel_on():
    job.G4TestAlg.SimTestTools += [CfgGetter.getPrivateTool("PixelHitsTestTool", checkType=True)]
if DetFlags.SCT_on():
    job.G4TestAlg.SimTestTools += [CfgGetter.getPrivateTool("SCT_HitsTestTool", checkType=True)]
if DetFlags.TRT_on():
    job.G4TestAlg.SimTestTools += [CfgGetter.getPrivateTool("TrtHitsTestTool", checkType=True)]
if DetFlags.LAr_on():
    job.G4TestAlg.SimTestTools += [CfgGetter.getPrivateTool("EMBHitsTestTool", checkType=True)]
    job.G4TestAlg.SimTestTools += [CfgGetter.getPrivateTool("EMECHitsTestTool", checkType=True)]
    job.G4TestAlg.SimTestTools += [CfgGetter.getPrivateTool("FCALHitsTestTool", checkType=True)]
    job.G4TestAlg.SimTestTools += [CfgGetter.getPrivateTool("HECHitsTestTool", checkType=True)]
    if DetFlags.Truth_on():
        job.G4TestAlg.SimTestTools += [CfgGetter.getPrivateTool("CaloEntryLayerTestTool", checkType=True)]
if DetFlags.Tile_on():
    job.G4TestAlg.SimTestTools += [CfgGetter.getPrivateTool("TileHitsTestTool", checkType=True)]
if simFlags.CalibrationRun.get_Value()=='LAr+Tile':
    job.G4TestAlg.SimTestTools += [CfgGetter.getPrivateTool("LArActiveCaloCalibHitsTestTool", checkType=True)]
    job.G4TestAlg.SimTestTools += [CfgGetter.getPrivateTool("LArInactiveCaloCalibHitsTestTool", checkType=True)]
    ##job.G4TestAlg.SimTestTools += [CfgGetter.getPrivateTool("LArDeadMaterialCaloCalibHitsTestTool", checkType=True)]
    job.G4TestAlg.SimTestTools += [CfgGetter.getPrivateTool("TileActiveCellCaloCalibHitsTestTool", checkType=True)]
    job.G4TestAlg.SimTestTools += [CfgGetter.getPrivateTool("TileInactiveCellCaloCalibHitsTestTool", checkType=True)]
    ##job.G4TestAlg.SimTestTools += [CfgGetter.getPrivateTool("TileDeadMaterialCaloCalibHitsTestTool", checkType=True)]
if DetFlags.Muon_on():
    job.G4TestAlg.SimTestTools += [CfgGetter.getPrivateTool("MDTHitsTestTool", checkType=True)]
    job.G4TestAlg.SimTestTools += [CfgGetter.getPrivateTool("RPCHitsTestTool", checkType=True)]
    if hasattr(simFlags, 'SimulateNewSmallWheel') and simFlags.SimulateNewSmallWheel():
        job.G4TestAlg.SimTestTools += [CfgGetter.getPrivateTool("MMHitsTestTool",  checkType=True)]
        job.G4TestAlg.SimTestTools += [CfgGetter.getPrivateTool("sTGCHitsTestTool",  checkType=True)]
    else:
        job.G4TestAlg.SimTestTools += [CfgGetter.getPrivateTool("CSCHitsTestTool", checkType=True)]
    job.G4TestAlg.SimTestTools += [CfgGetter.getPrivateTool("TGCHitsTestTool", checkType=True)]
    if DetFlags.Truth_on():
        job.G4TestAlg.SimTestTools += [CfgGetter.getPrivateTool("MuonEntryLayerTestTool", checkType=True)]
        job.G4TestAlg.SimTestTools += [CfgGetter.getPrivateTool("MuonExitLayerTestTool", checkType=True)]
if DetFlags.Lucid_on():
    job.G4TestAlg.SimTestTools += [CfgGetter.getPrivateTool("LucidHitsTestTool", checkType=True)]
if DetFlags.ALFA_on():
    job.G4TestAlg.SimTestTools += [CfgGetter.getPrivateTool("ALFA_SimHitsTestTool", checkType=True)]
if DetFlags.ZDC_on():
    job.G4TestAlg.SimTestTools += [CfgGetter.getPrivateTool("ZDCHitsTestTool", checkType=True)]
