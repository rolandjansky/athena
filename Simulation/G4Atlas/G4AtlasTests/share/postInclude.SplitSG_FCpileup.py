# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Updates the `AthAlgTool::EvtStore` property of `SimTestToolBase`
# objects to point at the correct store when running full chain (e.g.
# FastChain_tf.py) transforms with pile-up.
# Adapted from `postInclude.DCubeTest_FCpileup.py`.

from AthenaCommon import CfgGetter
from G4AtlasApps.SimFlags import simFlags
from AthenaCommon.DetFlags import DetFlags
from AthenaCommon.AlgSequence import AlgSequence
from AthenaCommon.AppMgr import ServiceMgr
if not hasattr(ServiceMgr, 'THistSvc'):
    from GaudiSvc.GaudiSvcConf import THistSvc
    ServiceMgr += THistSvc("THistSvc")
ServiceMgr.THistSvc.Output += ["truth DATAFILE='truth.root' OPT='RECREATE'"]


# note: some of these tests will need an active DetDescrManager
#       so please use something like
#             preExec='simFlags.ReleaseGeoModel=False;'
#       with your transform
job = AlgSequence()

if not hasattr(job, 'G4TestAlg'):
    from G4AtlasTests.G4AtlasTestsConf import G4TestAlg
    job += G4TestAlg()
simFlags.ReleaseGeoModel = False

# "new" SG key
newSG = "OriginalEvent_SG"

if DetFlags.Truth_on():
    job.G4TestAlg.SimTestTools += [
        CfgGetter.getPrivateTool("TruthTestTool", checkType=True)]
    job.G4TestAlg.SimTestTools += [CfgGetter.getPrivateTool(
        "EvgenTruthTestTool", checkType=True)]
    job.G4TestAlg.SimTestTools += [CfgGetter.getPrivateTool(
        "PileupTruthTestTool", checkType=True)]
    job.G4TestAlg.SimTestTools += [CfgGetter.getPrivateTool(
        "PileupEvgenTruthTestTool", checkType=True)]
if DetFlags.pixel_on():
    job.G4TestAlg.SimTestTools += [
        CfgGetter.getPrivateTool("PixelHitsTestTool", checkType=True)]
    job.G4TestAlg.SimTestTools['PixelHitsTestTool'].EvtStore = newSG
    job.G4TestAlg.SimTestTools += [CfgGetter.getPrivateTool(
        "PileupPixelHitsTestTool", checkType=True)]
if DetFlags.SCT_on():
    job.G4TestAlg.SimTestTools += [
        CfgGetter.getPrivateTool("SCT_HitsTestTool", checkType=True)]
    job.G4TestAlg.SimTestTools['SCT_HitsTestTool'].EvtStore = newSG
    job.G4TestAlg.SimTestTools += [CfgGetter.getPrivateTool(
        "PileupSCT_HitsTestTool", checkType=True)]
if DetFlags.TRT_on():
    job.G4TestAlg.SimTestTools += [
        CfgGetter.getPrivateTool("TrtHitsTestTool", checkType=True)]
    job.G4TestAlg.SimTestTools['TrtHitsTestTool'].EvtStore = newSG
    job.G4TestAlg.SimTestTools += [CfgGetter.getPrivateTool(
        "PileupTrtHitsTestTool", checkType=True)]
if DetFlags.LAr_on():
    job.G4TestAlg.SimTestTools += [
        CfgGetter.getPrivateTool("EMBHitsTestTool", checkType=True)]
    job.G4TestAlg.SimTestTools['EMBHitsTestTool'].EvtStore = newSG
    job.G4TestAlg.SimTestTools += [
        CfgGetter.getPrivateTool("EMECHitsTestTool", checkType=True)]
    job.G4TestAlg.SimTestTools['EMECHitsTestTool'].EvtStore = newSG
    job.G4TestAlg.SimTestTools += [
        CfgGetter.getPrivateTool("FCALHitsTestTool", checkType=True)]
    job.G4TestAlg.SimTestTools['FCALHitsTestTool'].EvtStore = newSG
    job.G4TestAlg.SimTestTools += [
        CfgGetter.getPrivateTool("HECHitsTestTool", checkType=True)]
    job.G4TestAlg.SimTestTools['HECHitsTestTool'].EvtStore = newSG
    if DetFlags.Truth_on():
        job.G4TestAlg.SimTestTools += [CfgGetter.getPrivateTool(
            "CaloEntryLayerTestTool", checkType=True)]
if DetFlags.Tile_on():
    job.G4TestAlg.SimTestTools += [
        CfgGetter.getPrivateTool("TileHitsTestTool", checkType=True)]
    job.G4TestAlg.SimTestTools['TileHitsTestTool'].EvtStore = newSG
if simFlags.CalibrationRun.get_Value() == 'LAr+Tile':
    job.G4TestAlg.SimTestTools += [CfgGetter.getPrivateTool(
        "LArActiveCaloCalibHitsTestTool", checkType=True)]
    job.G4TestAlg.SimTestTools += [CfgGetter.getPrivateTool(
        "LArInactiveCaloCalibHitsTestTool", checkType=True)]
    job.G4TestAlg.SimTestTools += [CfgGetter.getPrivateTool(
        "TileActiveCellCaloCalibHitsTestTool", checkType=True)]
    job.G4TestAlg.SimTestTools += [CfgGetter.getPrivateTool(
        "TileInactiveCellCaloCalibHitsTestTool", checkType=True)]
if DetFlags.Muon_on():
    job.G4TestAlg.SimTestTools += [
       CfgGetter.getPrivateTool("MDTHitsTestTool", checkType=True)]
    job.G4TestAlg.SimTestTools['MDTHitsTestTool'].EvtStore = newSG
    job.G4TestAlg.SimTestTools += [
       CfgGetter.getPrivateTool("RPCHitsTestTool", checkType=True)]
    job.G4TestAlg.SimTestTools['RPCHitsTestTool'].EvtStore = newSG
    job.G4TestAlg.SimTestTools += [
        CfgGetter.getPrivateTool("CSCHitsTestTool", checkType=True)]
    job.G4TestAlg.SimTestTools['CSCHitsTestTool'].EvtStore = newSG
    job.G4TestAlg.SimTestTools += [
        CfgGetter.getPrivateTool("TGCHitsTestTool", checkType=True)]
    job.G4TestAlg.SimTestTools['TGCHitsTestTool'].EvtStore = newSG
    if DetFlags.Truth_on():
        job.G4TestAlg.SimTestTools += [CfgGetter.getPrivateTool(
            "MuonEntryLayerTestTool", checkType=True)]
        job.G4TestAlg.SimTestTools += [CfgGetter.getPrivateTool(
            "MuonExitLayerTestTool", checkType=True)]
if DetFlags.Lucid_on():
    job.G4TestAlg.SimTestTools += [
        CfgGetter.getPrivateTool("LucidHitsTestTool", checkType=True)]
if DetFlags.ALFA_on():
    job.G4TestAlg.SimTestTools += [CfgGetter.getPrivateTool(
        "ALFA_SimHitsTestTool", checkType=True)]
if DetFlags.ZDC_on():
    job.G4TestAlg.SimTestTools += [
        CfgGetter.getPrivateTool("ZDCHitsTestTool", checkType=True)]
