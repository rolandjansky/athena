# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
from __future__ import print_function

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory


def FastSimulationToolListCfg(ConfigFlags):
    result = ComponentAccumulator()
    tools = []
    if ConfigFlags.Detector.GeometryBpipe:
        #if hasattr(simFlags, 'ForwardDetectors') and simFlags.ForwardDetectors.statusOn and simFlags.ForwardDetectors() == 2:
        #    FastSimulationList += ['ForwardTransportModel']
        if ConfigFlags.Sim.BeamPipeSimMode != "Normal":
            from G4FastSimulation.G4FastSimulationConfigNew import SimpleFastKillerCfg
            tools += [ result.popToolsAndMerge(SimpleFastKillerCfg(ConfigFlags)) ]
    if ConfigFlags.Detector.GeometryLAr:
        if ConfigFlags.Sim.LArParameterization > 0:
            # FIXME If we're only using Frozen Showers in the FCAL do
            # we really need to set up the EMB and EMEC as well?
            from LArG4FastSimulation.LArG4FastSimulationConfigNew import EMBFastShowerCfg, EMECFastShowerCfg, FCALFastShowerCfg, FCAL2FastShowerCfg
            tools += [ result.popToolsAndMerge(EMBFastShowerCfg(ConfigFlags)) ]
            tools += [ result.popToolsAndMerge(EMECFastShowerCfg(ConfigFlags)) ]
            tools += [ result.popToolsAndMerge(FCALFastShowerCfg(ConfigFlags)) ]
            tools += [ result.popToolsAndMerge(FCAL2FastShowerCfg(ConfigFlags)) ]
            if ConfigFlags.Sim.LArParameterization > 1:
                from G4FastSimulation.G4FastSimulationConfigNew import DeadMaterialShowerCfg
                tools += [ result.popToolsAndMerge(DeadMaterialShowerCfg(ConfigFlags)) ]
        elif ConfigFlags.Sim.LArParameterization == 0:
            print( "getFastSimulationMasterTool INFO No Frozen Showers" )
    if ConfigFlags.Detector.GeometryMuon:
        if ConfigFlags.Sim.CavernBG not in ['Off', 'Read']:
            # and not (hasattr(simFlags, 'RecordFlux') and simFlags.RecordFlux.statusOn and simFlags.RecordFlux()):
            from TrackWriteFastSim.TrackWriteFastSimConfigNew import NeutronFastSimCfg
            tools += [ result.popToolsAndMerge(NeutronFastSimCfg(ConfigFlags)) ]
    result.setPrivateTools(tools)
    return result


def FastSimulationMasterToolCfg(ConfigFlags, **kwargs):
    result = ComponentAccumulator()
    kwargs.setdefault("FastSimulations", result.popToolsAndMerge(FastSimulationToolListCfg(ConfigFlags)))
    FastSimulationMasterTool = CompFactory.FastSimulationMasterTool
    result.setPrivateTools(FastSimulationMasterTool(name="FastSimulationMasterTool", **kwargs))
    return result


def EmptyFastSimulationMasterToolCfg(ConfigFlags, **kwargs):
    result = ComponentAccumulator()
    FastSimulationMasterTool = CompFactory.FastSimulationMasterTool
    tool = result.popToolsAndMerge(FastSimulationMasterTool(name="EmptyFastSimulationMasterTool", **kwargs))
    result.setPrivateTools(tool)
    return result


def FwdSensitiveDetectorListCfg(ConfigFlags):
    # TODO: migrate to CA
    result = ComponentAccumulator()
    tools = []
    if ConfigFlags.Detector.EnableForward:
        print ('G4AtlasToolsConfigNew.FwdSensitiveDetectorListCfg ERROR Forward Detector SD configuration has not been migrated to CA yet!')
    if ConfigFlags.Detector.EnableLucid:
        tools += [ 'LUCID_SensitiveDetector' ]
    if ConfigFlags.Detector.EnableForward:
        if ConfigFlags.Detector.EnableZDC:
            tools += [ 'ZDC_PixelSD', 'ZDC_StripSD' ]
        if ConfigFlags.Detector.EnableALFA:
            tools += [ 'ALFA_SensitiveDetector' ]
        if ConfigFlags.Detector.EnableAFP:
            tools += [ 'AFP_SensitiveDetector' ]
            #tools += [ 'AFP_SiDSensitiveDetector', 'AFP_TDSensitiveDetector' ]
    result.setPrivateTools(tools)
    return result


def TrackFastSimSensitiveDetectorListCfg(ConfigFlags):
    result = ComponentAccumulator()
    tools = []
    if (ConfigFlags.Detector.EnableMuon and ConfigFlags.Sim.CavernBG in ['Write', 'WriteWorld']) or ConfigFlags.Sim.StoppedParticleFile:
        from TrackWriteFastSim.TrackWriteFastSimConfigNew import TrackFastSimSDCfg
        tools += [ result.popToolsAndMerge(TrackFastSimSDCfg(ConfigFlags)) ]
    result.setPrivateTools(tools)
    return result


def ITkSensitiveDetectorListCfg(ConfigFlags):
    result = ComponentAccumulator()
    tools = []

    if ConfigFlags.Detector.EnableITkPixel:
        from PixelG4_SD.PixelG4_SDToolConfig import ITkPixelSensorSDCfg
        tools += [ result.popToolsAndMerge(ITkPixelSensorSDCfg(ConfigFlags)) ]
        pass
    if ConfigFlags.Detector.EnableITkStrip:
        from SCT_G4_SD.SCT_G4_SDToolConfig import ITkStripSensorSDCfg
        tools += [ result.popToolsAndMerge(ITkStripSensorSDCfg(ConfigFlags)) ]
    if ConfigFlags.Detector.EnablePLR:
        from PixelG4_SD.PixelG4_SDToolConfig import PLRSensorSDCfg
        tools += [ result.popToolsAndMerge(PLRSensorSDCfg(ConfigFlags)) ]
    
    result.setPrivateTools(tools)
    return result


def HGTDSensitiveDetectorListCfg(ConfigFlags):
    result = ComponentAccumulator()
    tools = []

    if ConfigFlags.Detector.EnableHGTD:
        from HGTD_G4_SD.HGTD_G4_SDToolConfig import HgtdSensorSDCfg
        tools += [ result.popToolsAndMerge(HgtdSensorSDCfg(ConfigFlags)) ]
        pass

    result.setPrivateTools(tools)
    return result


def InDetSensitiveDetectorListCfg(ConfigFlags):
    result = ComponentAccumulator()
    tools = []

    if ConfigFlags.Detector.EnablePixel:
        from PixelG4_SD.PixelG4_SDToolConfig import PixelSensorSDCfg
        tools += [ result.popToolsAndMerge(PixelSensorSDCfg(ConfigFlags)) ]
    if ConfigFlags.Detector.EnableSCT:
        from SCT_G4_SD.SCT_G4_SDToolConfig import SctSensorSDCfg
        tools += [ result.popToolsAndMerge(SctSensorSDCfg(ConfigFlags)) ]
    if ConfigFlags.Detector.EnableTRT:
        from TRT_G4_SD.TRT_G4_SDToolConfig import TRTSensitiveDetectorCfg
        tools += [ result.popToolsAndMerge(TRTSensitiveDetectorCfg(ConfigFlags)) ]
    if ConfigFlags.Detector.EnableBCM:
        from BCM_G4_SD.BCM_G4_SDToolConfig import BCMSensorSDCfg
        tools += [ result.popToolsAndMerge(BCMSensorSDCfg(ConfigFlags)) ]
        from BLM_G4_SD.BLM_G4_SDToolConfig import BLMSensorSDCfg
        tools += [ result.popToolsAndMerge(BLMSensorSDCfg(ConfigFlags)) ]

    result.setPrivateTools(tools)
    return result


def CaloSensitiveDetectorListCfg(ConfigFlags):
    result = ComponentAccumulator()
    tools = []

    if ConfigFlags.Detector.EnableLAr:
        from LArG4SD.LArG4SDToolConfig import LArEMBSensitiveDetectorCfg, LArEMECSensitiveDetectorCfg, LArFCALSensitiveDetectorCfg, LArHECSensitiveDetectorCfg
        tools += [ result.popToolsAndMerge(LArEMBSensitiveDetectorCfg(ConfigFlags)) ]
        tools += [ result.popToolsAndMerge(LArEMECSensitiveDetectorCfg(ConfigFlags)) ]
        tools += [ result.popToolsAndMerge(LArFCALSensitiveDetectorCfg(ConfigFlags)) ]
        tools += [ result.popToolsAndMerge(LArHECSensitiveDetectorCfg(ConfigFlags)) ]
        if False:  # disabled for now
            from LArG4SD.LArG4SDToolConfig import LArMiniFCALSensitiveDetectorToolCfg
            tools += [ result.popToolsAndMerge(LArMiniFCALSensitiveDetectorToolCfg(ConfigFlags)) ]

        if ConfigFlags.Detector.EnableMBTS:
            from MinBiasScintillator.MinBiasScintillatorToolConfig import MinBiasScintillatorSDCfg
            tools += [ result.popToolsAndMerge(MinBiasScintillatorSDCfg(ConfigFlags)) ]

        if ConfigFlags.Sim.CalibrationRun in ['LAr', 'LAr+Tile']:
            from LArG4SD.LArG4SDToolConfig import LArDeadSensitiveDetectorToolCfg, LArActiveSensitiveDetectorToolCfg, LArInactiveSensitiveDetectorToolCfg
            tools += [ result.popToolsAndMerge(LArDeadSensitiveDetectorToolCfg(ConfigFlags)) ]
            tools += [ result.popToolsAndMerge(LArInactiveSensitiveDetectorToolCfg(ConfigFlags)) ]
            tools += [ result.popToolsAndMerge(LArActiveSensitiveDetectorToolCfg(ConfigFlags)) ]
        elif ConfigFlags.Sim.CalibrationRun == 'DeadLAr':
            from LArG4SD.LArG4SDToolConfig import LArDeadSensitiveDetectorToolCfg
            tools += [ result.popToolsAndMerge(LArDeadSensitiveDetectorToolCfg(ConfigFlags)) ]

    if ConfigFlags.Detector.EnableTile:
        if ConfigFlags.Sim.CalibrationRun in ['Tile', 'LAr+Tile']:
            from TileGeoG4Calib.TileGeoG4CalibConfigNew import TileGeoG4CalibSDCfg
            tools += [ result.popToolsAndMerge(TileGeoG4CalibSDCfg(ConfigFlags)) ]  # mode 1 : With CaloCalibrationHits
        else:
            from TileGeoG4SD.TileGeoG4SDToolConfig import TileGeoG4SDCfg
            tools += [ result.popToolsAndMerge(TileGeoG4SDCfg(ConfigFlags)) ]       # mode 0 : No CaloCalibrationHits
    if ConfigFlags.Sim.RecordStepInfo:
        from ISF_FastCaloSimSD.ISF_FastCaloSimSDToolConfig import FCS_StepInfoSDToolCfg
        tools += [ result.popToolsAndMerge(FCS_StepInfoSDToolCfg(ConfigFlags)) ]

    result.setPrivateTools(tools)
    return result


def MuonSensitiveDetectorListCfg(ConfigFlags):
    result = ComponentAccumulator()
    tools = []

    if ConfigFlags.Beam.Type == 'cosmics':
        if ConfigFlags.Detector.EnableMDT:
            from MuonG4SD.MuonG4SDToolConfig import MDTSensitiveDetectorCosmicsCfg
            tools += [ result.popToolsAndMerge(MDTSensitiveDetectorCosmicsCfg(ConfigFlags)) ]
        if ConfigFlags.Detector.EnableRPC:
            from MuonG4SD.MuonG4SDToolConfig import RPCSensitiveDetectorCosmicsCfg
            tools += [ result.popToolsAndMerge(RPCSensitiveDetectorCosmicsCfg(ConfigFlags)) ]
        if ConfigFlags.Detector.EnableTGC:
            from MuonG4SD.MuonG4SDToolConfig import TGCSensitiveDetectorCosmicsCfg
            tools += [ result.popToolsAndMerge(TGCSensitiveDetectorCosmicsCfg(ConfigFlags)) ]
        if ConfigFlags.Detector.EnableCSC:
            from MuonG4SD.MuonG4SDToolConfig import CSCSensitiveDetectorCosmicsCfg
            tools += [ result.popToolsAndMerge(CSCSensitiveDetectorCosmicsCfg(ConfigFlags)) ]
    else:
        if ConfigFlags.Detector.EnableMDT:
            from MuonG4SD.MuonG4SDToolConfig import MDTSensitiveDetectorCfg
            tools += [ result.popToolsAndMerge(MDTSensitiveDetectorCfg(ConfigFlags)) ]
        if ConfigFlags.Detector.EnableRPC:
            from MuonG4SD.MuonG4SDToolConfig import RPCSensitiveDetectorCfg
            tools += [ result.popToolsAndMerge(RPCSensitiveDetectorCfg(ConfigFlags)) ]
        if ConfigFlags.Detector.EnableTGC:
            from MuonG4SD.MuonG4SDToolConfig import TGCSensitiveDetectorCfg
            tools += [ result.popToolsAndMerge(TGCSensitiveDetectorCfg(ConfigFlags)) ]
        if ConfigFlags.Detector.EnableCSC:
            from MuonG4SD.MuonG4SDToolConfig import CSCSensitiveDetectorCfg
            tools += [ result.popToolsAndMerge(CSCSensitiveDetectorCfg(ConfigFlags)) ]

    if ConfigFlags.Detector.EnablesTGC :
        from MuonG4SD.MuonG4SDToolConfig import sTGCSensitiveDetectorCfg
        tools += [ result.popToolsAndMerge(sTGCSensitiveDetectorCfg(ConfigFlags)) ]
    if ConfigFlags.Detector.EnableMM :
        from MuonG4SD.MuonG4SDToolConfig import MicromegasSensitiveDetectorCfg
        tools += [ result.popToolsAndMerge(MicromegasSensitiveDetectorCfg(ConfigFlags)) ]

    result.setPrivateTools(tools)
    return result


def EnvelopeSensitiveDetectorListCfg(ConfigFlags):
    result = ComponentAccumulator()
    tools = []
    if ConfigFlags.Beam.Type == 'cosmics' and not ConfigFlags.Sim.ReadTR:
        from TrackWriteFastSim.TrackWriteFastSimConfigNew import CosmicTRSDCfg
        tools += [ result.popToolsAndMerge(CosmicTRSDCfg(ConfigFlags)) ]
    result.setPrivateTools(tools)
    return result


def SensitiveDetectorListCfg(ConfigFlags):
    result = ComponentAccumulator()
    tools = []

    tools += result.popToolsAndMerge(EnvelopeSensitiveDetectorListCfg(ConfigFlags))
    tools += result.popToolsAndMerge(InDetSensitiveDetectorListCfg(ConfigFlags))
    tools += result.popToolsAndMerge(ITkSensitiveDetectorListCfg(ConfigFlags))
    tools += result.popToolsAndMerge(HGTDSensitiveDetectorListCfg(ConfigFlags))
    tools += result.popToolsAndMerge(CaloSensitiveDetectorListCfg(ConfigFlags))
    tools += result.popToolsAndMerge(MuonSensitiveDetectorListCfg(ConfigFlags))
    tools += result.popToolsAndMerge(TrackFastSimSensitiveDetectorListCfg(ConfigFlags))
    tools += result.popToolsAndMerge(FwdSensitiveDetectorListCfg(ConfigFlags))

    result.setPrivateTools(tools)
    return result


def TestBeamSensitiveDetectorListCfg(ConfigFlags):
    result = ComponentAccumulator()
    tools = []

    if "tb_Tile2000_2003" in ConfigFlags.GeoModel.AtlasVersion:
        if ConfigFlags.Detector.EnableTile:
            if ConfigFlags.Sim.CalibrationRun in ['Tile', 'LAr+Tile']:
                from TileGeoG4Calib.TileGeoG4CalibConfigNew import TileCTBGeoG4CalibSDCfg
                tools += [ result.popToolsAndMerge(TileCTBGeoG4CalibSDCfg(ConfigFlags)) ] # mode 1 : With CaloCalibrationHits
            else:
                from TileGeoG4SD.TileGeoG4SDToolConfig import TileCTBGeoG4SDCfg
                tools += [ result.popToolsAndMerge(TileCTBGeoG4SDCfg(ConfigFlags)) ]      # mode 0 : No CaloCalibrationHits
                tools += [ 'MuonWallSD' ]
        result.setPrivateTools(tools)
        return result

    if ConfigFlags.Detector.EnablePixel:
        from PixelG4_SD.PixelG4_SDToolConfig import PixelSensor_CTBCfg
        tools += [ result.popToolsAndMerge(PixelSensor_CTBCfg(ConfigFlags)) ]
    if ConfigFlags.Detector.EnableSCT:
        from SCT_G4_SD.SCT_G4_SDToolConfig import SctSensor_CTBCfg
        tools += [ result.popToolsAndMerge(SctSensor_CTBCfg(ConfigFlags)) ]
    if ConfigFlags.Detector.EnableTRT:
        from TRT_G4_SD.TRT_G4_SDToolConfig import TRTSensitiveDetector_CTBCfg
        tools += [ result.popToolsAndMerge(TRTSensitiveDetector_CTBCfg(ConfigFlags)) ]
    if ConfigFlags.Detector.EnableLAr:
        from LArG4SD.LArG4SDToolConfig import LArEMBSensitiveDetectorCfg
        tools += [ result.popToolsAndMerge(LArEMBSensitiveDetectorCfg(ConfigFlags)) ]
        if 'LAr' in ConfigFlags.Sim.CalibrationRun:
            tools += [ 'LArH8CalibSensitiveDetector' ] # mode 1 : With CaloCalibrationHits
    if ConfigFlags.Detector.EnableTile:
        if ConfigFlags.Sim.CalibrationRun in ['Tile', 'LAr+Tile']:
            from TileGeoG4Calib.TileGeoG4CalibConfigNew import TileCTBGeoG4CalibSDCfg
            tools += [ result.popToolsAndMerge(TileCTBGeoG4CalibSDCfg(ConfigFlags)) ] # mode 1 : With CaloCalibrationHits
        else:
            from TileGeoG4SD.TileGeoG4SDToolConfig import TileCTBGeoG4SDCfg
            tools += [ result.popToolsAndMerge(TileCTBGeoG4SDCfg(ConfigFlags)) ]      # mode 0 : No CaloCalibrationHits
            tools += [ 'MuonWallSD' ]
    if ConfigFlags.Detector.EnableMuon:
        tools += [ 'MuonEntryRecord' ]
    tools += result.popToolsAndMerge(MuonSensitiveDetectorListCfg(ConfigFlags))

    result.setPrivateTools(tools)
    return result


def SensitiveDetectorMasterToolCfg(ConfigFlags, name="SensitiveDetectorMasterTool", **kwargs):
    result = ComponentAccumulator()
    if "ATLAS" in ConfigFlags.GeoModel.AtlasVersion:
        kwargs.setdefault("SensitiveDetectors", result.popToolsAndMerge(SensitiveDetectorListCfg(ConfigFlags)))
    elif "tb_Tile2000_2003" in ConfigFlags.GeoModel.AtlasVersion:
        kwargs.setdefault("SensitiveDetectors", result.popToolsAndMerge(TestBeamSensitiveDetectorListCfg(ConfigFlags)))
    elif "tb_LArH6" in ConfigFlags.GeoModel.AtlasVersion:
        pass
    elif "ctbh8" in ConfigFlags.GeoModel.AtlasVersion:
        kwargs.setdefault("SensitiveDetectors", result.popToolsAndMerge(TestBeamSensitiveDetectorListCfg(ConfigFlags)))

    result.setPrivateTools(CompFactory.SensitiveDetectorMasterTool(name, **kwargs))
    return result


def EmptySensitiveDetectorMasterToolCfg(name="EmptySensitiveDetectorMasterTool", **kwargs):
    result = ComponentAccumulator()
    tool = result.popToolsAndMerge(CompFactory.SensitiveDetectorMasterTool(name, **kwargs))
    result.setPrivateTools(tool)
    return result
