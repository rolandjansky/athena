# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.Logging import logging
from AthenaConfiguration.ComponentFactory import CompFactory
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from G4AtlasTests.G4AtlasTestsConfigNew import (
    SiHitsTestToolCfg, TrtHitsTestToolCfg, LArHitsTestToolCfg,
    TileHitsTestToolCfg, CaloCalibrationHitsTestToolCfg, MDTHitsTestToolCfg,
    RPCHitsTestToolCfg, CSCHitsTestToolCfg, TGCHitsTestToolCfg,
    LucidHitsTestToolCfg, ALFASimHitsTestToolCfg, ZDCHitsTestToolCfg,
    LayerTestToolCfg, TruthTestToolCfg
)


def g4TestAlgCfg(toolList, configFlags, name="g4TestAlg", **kwargs):
    mlog = logging.getLogger(name)
    mlog.debug('Start configuration')
    result = ComponentAccumulator()

    kwargs.setdefault("SimTestTools", toolList)
    G4TestAlg = CompFactory.G4TestAlg('G4TestAlg')
    result.addEventAlgo(G4TestAlg)
    return result


def FCpileupCfg(configFlags, name='FCpileup',  **kwargs):

    mlog = logging.getLogger('FCpileupCfg')
    mlog.debug('Start configuration')
    result = ComponentAccumulator()

    histSvc = CompFactory.THistSvc(Output=["truth DATAFILE='truth.root', OPT='RECREATE'"])

    toolList = []
    TruthTestTool = result.popToolsAndMerge(TruthTestToolCfg("TruthEvent", name="TruthTestTool", **kwargs))
    toolList.append(TruthTestTool)
    EvgenTruthTestTool = result.popToolsAndMerge(TruthTestToolCfg("GEN_EVENT", name="EvgenTruthTestTool", **kwargs))
    toolList.append(EvgenTruthTestTool)
    PileupTruthTestTool = result.popToolsAndMerge(TruthTestToolCfg("TruthEvent_PU", name="PileupTruthTestTool", **kwargs))
    toolList.append(PileupTruthTestTool)
    PileupEvgenTruthTestTool = result.popToolsAndMerge(TruthTestToolCfg("GEN_EVENT_PU", name="PileupEvgenTruthTestTool", **kwargs))
    toolList.append(PileupEvgenTruthTestTool)
    if configFlags.Detector.EnablePixel:
        PixelHitsTestTool = result.popToolsAndMerge(SiHitsTestToolCfg("PixelHits", name, **kwargs))
        toolList.append(PixelHitsTestTool)
        PileupPixelHitsTestTool = result.popToolsAndMerge(SiHitsTestToolCfg("PileupPixelHits", name, **kwargs))
        toolList.append(PileupPixelHitsTestTool)
    if configFlags.Detector.EnableSCT:
        SCT_HitsTestTool = result.popToolsAndMerge(SiHitsTestToolCfg("SCT_Hits", name, **kwargs))
        toolList.append(SCT_HitsTestTool)
        PileupSCT_HitsTestTool = result.popToolsAndMerge(SiHitsTestToolCfg("PileupSCT_HitsTestTool", name, **kwargs))
        toolList.append(PileupSCT_HitsTestTool)
    if configFlags.Detector.EnableTRT:
        TrtHitsTestTool = result.popToolsAndMerge(TrtHitsTestToolCfg("TRTUncompressedHits", name, **kwargs))
        toolList.append(TrtHitsTestTool)
        PileupTrtHitsTestTool = result.popToolsAndMerge(TrtHitsTestToolCfg("PileupTRTUncompressedHits", name, **kwargs))
        toolList.append(PileupTrtHitsTestTool)
    if configFlags.Detector.EnableTile:
        TileHitsTestTool = result.popToolsAndMerge(TileHitsTestToolCfg(configFlags.Sim.UsingGeant4, name, **kwargs))
        toolList.append(TileHitsTestTool)
    if configFlags.Detector.EnableLAr:
        LArHitsTestToolEMB = result.popToolsAndMerge(LArHitsTestToolCfg("EMB", name, **kwargs))
        toolList.append(LArHitsTestToolEMB)
        LArHitsTestToolEMEC = result.popToolsAndMerge(LArHitsTestToolCfg("EMEC", name, **kwargs))
        toolList.append(LArHitsTestToolEMEC)
        LArHitsTestToolHEC = result.popToolsAndMerge(LArHitsTestToolCfg("HEC", name, **kwargs))
        toolList.append(LArHitsTestToolHEC)
        LArHitsTestToolFCAL = result.popToolsAndMerge(LArHitsTestToolCfg("FCAL", name, **kwargs))
        toolList.append(LArHitsTestToolFCAL)
        if configFlags.Detector.EnableTile:
            CaloCalibHitsTestTool_LArAct = result.popToolsAndMerge(CaloCalibrationHitsTestToolCfg("LArActiveCaloCalibHitsTestTool", name, **kwargs))
            toolList.append(CaloCalibHitsTestTool_LArAct)
            CaloCalibHitsTestTool_LArInac = result.popToolsAndMerge(CaloCalibrationHitsTestToolCfg("LArInactiveCaloCalibHitsTestTool", name, **kwargs))
            toolList.append(CaloCalibHitsTestTool_LArInac)
            # CaloCalibHitsTestTool_LArDeadMat = result.popToolsAndMerge(CaloCalibrationHitsTestToolCfg("LArDeadMaterialCaloCalibHitsTestTool", name, **kwargs))
            # toolList.append(CaloCalibHitsTestTool_LArDeadMat)
            CaloCalibHitsTestTool_TileAct = result.popToolsAndMerge(CaloCalibrationHitsTestToolCfg("TileActiveCellCaloCalibHitsTestTool", name, **kwargs))
            toolList.append(CaloCalibHitsTestTool_TileAct)
            CaloCalibHitsTestTool_TileInac = result.popToolsAndMerge(CaloCalibrationHitsTestToolCfg("TileInactiveCellCaloCalibHitsTestTool", name, **kwargs))
            toolList.append(CaloCalibHitsTestTool_TileInac)
            # CaloCalibHitsTestTool_TileDeadMat = result.popToolsAndMerge(CaloCalibrationHitsTestToolCfg("TileDeadMaterialCaloCalibHitsTestTool", name, **kwargs))
            # toolList.append(CaloCalibHitsTestTool_TileDeadMat)
        CaloEntryLayerTestTool = result.popToolsAndMerge(LayerTestToolCfg("CaloEntry", name="CaloEntry", **kwargs))
        toolList.append(CaloEntryLayerTestTool)

    if configFlags.Detector.EnableMuon:
        MDTHitsTestTool = result.popToolsAndMerge(MDTHitsTestToolCfg("MDT", name, **kwargs))
        toolList.append(MDTHitsTestTool)
        RPCHitsTestTool = result.popToolsAndMerge(RPCHitsTestToolCfg("RPC", name, **kwargs))
        toolList.append(RPCHitsTestTool)
        CSCHitsTestTool = result.popToolsAndMerge(CSCHitsTestToolCfg("CSC", name, **kwargs))
        toolList.append(CSCHitsTestTool)
        TGCHitsTestTool = result.popToolsAndMerge(TGCHitsTestToolCfg("TGC", name, **kwargs))
        toolList.append(TGCHitsTestTool)
        MuonEntryLayerTestTool = result.popToolsAndMerge(LayerTestToolCfg("MuonEntry", name="MuonEntry", **kwargs))
        toolList.append(MuonEntryLayerTestTool)
        MuonExitLayerTestTool = result.popToolsAndMerge(LayerTestToolCfg("MuonExit", name="MuonExit", **kwargs))
        toolList.append(MuonExitLayerTestTool)
    if configFlags.Detector.EnableLucid:
        LucidHitsTestTool = result.popToolsAndMerge(LucidHitsTestToolCfg(name, **kwargs))
        toolList.append(LucidHitsTestTool)
    if configFlags.Detector.EnableALFA:
        ALFASimHitsTestTool = result.popToolsAndMerge(ALFASimHitsTestToolCfg(name, **kwargs))
        toolList.append(ALFASimHitsTestTool)
    if configFlags.Detector.EnableZDC:
        ZDCHitsTestTool = result.popToolsAndMerge(ZDCHitsTestToolCfg(name, **kwargs))
        toolList.append(ZDCHitsTestTool)

    result.merge(g4TestAlgCfg(toolList, configFlags, name, **kwargs))
    result.addService(histSvc)
    return result


if __name__ == "__main__":

    from AthenaConfiguration.AllConfigFlags import ConfigFlags
    from AthenaCommon.Logging import log
    from AthenaCommon.Constants import DEBUG
    from AthenaCommon.Configurable import Configurable
    from AthenaConfiguration.TestDefaults import defaultTestFiles
    Configurable.configurableRun3Behavior = 1

    log.setLevel(DEBUG)

    ConfigFlags.Input.isMC = True
    ConfigFlags.Input.Files = defaultTestFiles.HITS_RUN2
    ConfigFlags.Exec.MaxEvents = 3
    ConfigFlags.fillFromArgs()
    ConfigFlags.lock()

    from AthenaConfiguration.MainServicesConfig import MainServicesCfg
    acc = MainServicesCfg(ConfigFlags)
    acc.popToolsAndMerge(FCpileupCfg(ConfigFlags))

    print("INFO_FCpileupCfg: Dumping config flags")
    ConfigFlags.dump()
    print("INFO_FCpileupCfg: Print config details")
    acc.printConfig(withDetails=True, summariseProps=True)
    acc.store(open('fcpileup.pkl', 'wb'))
