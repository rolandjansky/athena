# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
from __future__ import print_function
from AthenaConfiguration.ComponentFactory import CompFactory


from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
SensitiveDetectorMasterTool=CompFactory.SensitiveDetectorMasterTool
FastSimulationMasterTool=CompFactory.FastSimulationMasterTool

from BCM_G4_SD.BCM_G4_SDToolConfig import BCMSensorSDCfg
from BLM_G4_SD.BLM_G4_SDToolConfig import BLMSensorSDCfg

from PixelG4_SD.PixelG4_SDToolConfig import PixelSensorSDCfg
from PixelG4_SD.PixelG4_SDToolConfig import PixelSensor_CTBCfg

from SCT_G4_SD.SCT_G4_SDToolConfig import SctSensorSDCfg
from SCT_G4_SD.SCT_G4_SDToolConfig import SLHC_SctSensorSDCfg
from SCT_G4_SD.SCT_G4_SDToolConfig import SctSensor_CTBCfg

from TRT_G4_SD.TRT_G4_SDToolConfig import TRTSensitiveDetectorCfg
from TRT_G4_SD.TRT_G4_SDToolConfig import TRTSensitiveDetector_CTBCfg


from LArG4SD.LArG4SDToolConfig import LArEMBSensitiveDetectorCfg, LArEMECSensitiveDetectorCfg, LArFCALSensitiveDetectorCfg, LArHECSensitiveDetectorCfg, LArMiniFCALSensitiveDetectorToolCfg, LArDeadSensitiveDetectorToolCfg, LArActiveSensitiveDetectorToolCfg, LArInactiveSensitiveDetectorToolCfg

from TileGeoG4SD.TileGeoG4SDToolConfig import TileGeoG4SDCfg, TileCTBGeoG4SDCfg

from TileGeoG4Calib.TileGeoG4CalibConfigNew import TileGeoG4CalibSDCfg, TileCTBGeoG4CalibSDCfg
from MuonG4SD.MuonG4SDToolConfig import CSCSensitiveDetectorCfg, MDTSensitiveDetectorCfg, RPCSensitiveDetectorCfg, MDTSensitiveDetectorCosmicsCfg, RPCSensitiveDetectorCosmicsCfg, TGCSensitiveDetectorCosmicsCfg, sTGCSensitiveDetectorCfg, MicromegasSensitiveDetectorCfg, CSCSensitiveDetectorCosmicsCfg, TGCSensitiveDetectorCfg
from MinBiasScintillator.MinBiasScintillatorToolConfig import MinBiasScintillatorSDCfg
from LArG4FastSimulation.LArG4FastSimulationConfigNew import EMBFastShowerCfg, EMECFastShowerCfg, FCALFastShowerCfg, FCAL2FastShowerCfg
from G4FastSimulation.G4FastSimulationConfigNew import SimpleFastKillerCfg, DeadMaterialShowerCfg

def generateFastSimulationListCfg(ConfigFlags):
    result = ComponentAccumulator()
    FastSimulationList = []
    if ConfigFlags.Detector.GeometryBpipe:
        #if hasattr(simFlags, 'ForwardDetectors') and simFlags.ForwardDetectors.statusOn and simFlags.ForwardDetectors() == 2:
        #    FastSimulationList += ['ForwardTransportModel']
        if ConfigFlags.Sim.BeamPipeSimMode != "Normal":
            FastSimulationList += [ result.popToolsAndMerge(SimpleFastKillerCfg(ConfigFlags)) ]
    if ConfigFlags.Detector.GeometryLAr:
        if ConfigFlags.Sim.LArParameterization > 0:
            # FIXME If we're only using Frozen Showers in the FCAL do
            # we really need to set up the EMB and EMEC as well?
            FastSimulationList += [result.popToolsAndMerge(EMBFastShowerCfg(ConfigFlags))]
            FastSimulationList += [result.popToolsAndMerge(EMECFastShowerCfg(ConfigFlags))]
            FastSimulationList += [result.popToolsAndMerge(FCALFastShowerCfg(ConfigFlags))]
            FastSimulationList += [result.popToolsAndMerge(FCAL2FastShowerCfg(ConfigFlags))]
            if ConfigFlags.Sim.LArParameterization > 1:
                 FastSimulationList += [ result.popToolsAndMerge(DeadMaterialShowerCfg(ConfigFlags)) ]
        elif ConfigFlags.Sim.LArParameterization == 0:
            print( "getFastSimulationMasterTool INFO No Frozen Showers" )
    #if ConfigFlags.Detector.GeometryMuon:
    #    if hasattr(simFlags, 'CavernBG') and simFlags.CavernBG.statusOn and simFlags.CavernBG.get_Value() != 'Read' and\
    #            not (hasattr(simFlags, 'RecordFlux') and simFlags.RecordFlux.statusOn and simFlags.RecordFlux()):
    #        FastSimulationList += ['NeutronFastSim']
    result.setPrivateTools(FastSimulationList)
    return result


def FastSimulationMasterToolCfg(ConfigFlags, **kwargs):
    result = ComponentAccumulator()
    FastSimulationList = result.popToolsAndMerge(generateFastSimulationListCfg(ConfigFlags))
    kwargs.setdefault("FastSimulations", FastSimulationList)
    result.addPublicTool(FastSimulationMasterTool(name="FastSimulationMasterTool", **kwargs))
    return result


def EmptyFastSimulationMasterToolCfg(ConfigFlags, **kwargs):
    result = ComponentAccumulator()
    result.addPublicTool(FastSimulationMasterTool(name="EmptyFastSimulationMasterTool", **kwargs))
    return result


def generateFwdSensitiveDetectorList(ConfigFlags):
    SensitiveDetectorList=[]
    if ConfigFlags.Detector.SimulateLucid:
        SensitiveDetectorList += [ 'LUCID_SensitiveDetector' ]
    if ConfigFlags.Detector.SimulateForward:
        if ConfigFlags.Detector.SimulateZDC:
            SensitiveDetectorList += [ 'ZDC_PixelSD', 'ZDC_StripSD' ]
        if ConfigFlags.Detector.SimulateALFA:
            SensitiveDetectorList += [ 'ALFA_SensitiveDetector' ]
        if ConfigFlags.Detector.SimulateAFP:
            SensitiveDetectorList += [ 'AFP_SensitiveDetector' ]
            #SensitiveDetectorList += [ 'AFP_SiDSensitiveDetector', 'AFP_TDSensitiveDetector' ]

    return SensitiveDetectorList


def generateTrackFastSimSensitiveDetectorList(ConfigFlags):
    SensitiveDetectorList=[]
    #if (ConfigFlags.Detector.Muon_on() and simFlags.CavernBG.statusOn and simFlags.CavernBG.get_Value() != 'Read' and 'Write' in simFlags.CavernBG.get_Value()) or (hasattr(simFlags, 'StoppedParticleFile') and simFlags.StoppedParticleFile.statusOn):
    #if ((ConfigFlags.Detector.SimulateMuon) and (ConfigFlags.Sim.CavernBG != 'Read') and ('Write' in ConfigFlags.Sim.CavernBG)) or ConfigFlags.Sim.StoppedParticleFile:
    if False: 
        SensitiveDetectorList += [ 'TrackFastSimSD' ]
    return SensitiveDetectorList


def generateInDetSensitiveDetectorList(ConfigFlags):

    result = ComponentAccumulator()
    SensitiveDetectorList=[]

    isUpgrade = ConfigFlags.GeoModel.Run=="RUN4"
    #EDIT THIS LATER(geoflags...)...!!
    isRUN2 = (ConfigFlags.GeoModel.Run in ["RUN2", "RUN3"]) or (ConfigFlags.GeoModel.Run=="UNDEFINED" )#and geoFlags.isIBL()) #isIBL may cause issues later....
    isRUN1 = not (isRUN2 or isUpgrade)

    if ConfigFlags.Detector.SimulatePixel:
        if isRUN1 or isRUN2:
            if ConfigFlags.Detector.SimulateBCM:
                accBCM, toolBCM = BCMSensorSDCfg(ConfigFlags)
                SensitiveDetectorList += [ toolBCM ]
                result.merge(accBCM)
                accBLM, toolBLM = BLMSensorSDCfg(ConfigFlags)
                SensitiveDetectorList += [ toolBLM ]
                result.merge(accBLM)
        accPixel, toolPixel = PixelSensorSDCfg(ConfigFlags)
        SensitiveDetectorList += [ toolPixel ]
        result.merge(accPixel)
    if ConfigFlags.Detector.SimulateSCT:
        if isUpgrade:
            accSCT, toolSCT = SLHC_SctSensorSDCfg(ConfigFlags)
            SensitiveDetectorList += [ toolSCT ]
            result.merge(accSCT)
        else:
            accSCT,toolSCT = SctSensorSDCfg(ConfigFlags)
            SensitiveDetectorList += [ toolSCT ]
            result.merge(accSCT)
    if ConfigFlags.Detector.SimulateTRT and not isUpgrade:
        accTRT, toolTRT = TRTSensitiveDetectorCfg(ConfigFlags)
        SensitiveDetectorList += [ toolTRT ]
        result.merge(accTRT)
    return result, SensitiveDetectorList #List of tools here now! (CALL IT TOOL LIST?)


def generateCaloSensitiveDetectorList(ConfigFlags):
    result = ComponentAccumulator()
    SensitiveDetectorList=[]

    if ConfigFlags.Detector.SimulateLAr:
        accLArEMB = LArEMBSensitiveDetectorCfg(ConfigFlags)
        accLArEMEC = LArEMECSensitiveDetectorCfg(ConfigFlags)
        accLArFCAL = LArFCALSensitiveDetectorCfg(ConfigFlags)
        accLArHEC = LArHECSensitiveDetectorCfg(ConfigFlags)
        accLArMiniFCAL = LArMiniFCALSensitiveDetectorToolCfg(ConfigFlags)

        SensitiveDetectorList += [  result.popToolsAndMerge(accLArEMB),
                                    result.popToolsAndMerge(accLArEMEC),
                                    result.popToolsAndMerge(accLArFCAL),
                                    result.popToolsAndMerge(accLArHEC),
                                    result.popToolsAndMerge(accLArMiniFCAL)
                                   ]
        #todo migrate below>>>
        #if hasattr(DetFlags.simulate, 'HGTD_on') and DetFlags.simulate.HGTD_on():
        #if ConfigFlags.Detector.SimulateHGTD:
        if False:
            SensitiveDetectorList += [ 'HGTDSensorSD' ]
        else:
            accMinBiasScintillator = MinBiasScintillatorSDCfg(ConfigFlags)
            SensitiveDetectorList += [ result.popToolsAndMerge(accMinBiasScintillator) ]

        if ConfigFlags.Sim.CalibrationRun in ['LAr', 'LAr+Tile']:
            SensitiveDetectorList += [ result.popToolsAndMerge(LArDeadSensitiveDetectorToolCfg(ConfigFlags)) ]
            SensitiveDetectorList += [ result.popToolsAndMerge(LArActiveSensitiveDetectorToolCfg(ConfigFlags)) ]
            SensitiveDetectorList += [ result.popToolsAndMerge(LArInactiveSensitiveDetectorToolCfg(ConfigFlags)) ]
        elif ConfigFlags.Sim.CalibrationRun == 'DeadLAr':
            accDeadSensitiveDetector = LArDeadSensitiveDetectorToolCfg(ConfigFlags)
            SensitiveDetectorList += [ result.popToolsAndMerge(accDeadSensitiveDetector) ]

    if ConfigFlags.Detector.SimulateTile:
        if ConfigFlags.Sim.CalibrationRun in ['Tile', 'LAr+Tile']:
            accTile = TileGeoG4CalibSDCfg(ConfigFlags)
            SensitiveDetectorList += [ result.popToolsAndMerge(accTile) ] # mode 1 : With CaloCalibrationHits
        else:
            accTile = TileGeoG4SDCfg(ConfigFlags)
            SensitiveDetectorList += [ result.popToolsAndMerge(accTile) ]      # mode 0 : No CaloCalibrationHits

    if ConfigFlags.Sim.RecordStepInfo:
        SensitiveDetectorList += [ 'FCS_StepInfoSensitiveDetector' ]

    result.setPrivateTools(SensitiveDetectorList)
    return result


def generateMuonSensitiveDetectorList(ConfigFlags):
    result = ComponentAccumulator()
    SensitiveDetectorList=[]
    
    if ConfigFlags.Detector.SimulateMuon:
        if ConfigFlags.Beam.Type == 'cosmics':
            if ConfigFlags.Detector.SimulateMDT : 
                accMDT = MDTSensitiveDetectorCosmicsCfg(ConfigFlags)
                SensitiveDetectorList += [ result.popToolsAndMerge(accMDT) ]
            if ConfigFlags.Detector.SimulateRPC : 
                accRPC = RPCSensitiveDetectorCosmicsCfg(ConfigFlags)
                SensitiveDetectorList += [ result.popToolsAndMerge(accRPC) ]
            if ConfigFlags.Detector.SimulateTGC : 
                accTGC = TGCSensitiveDetectorCosmicsCfg(ConfigFlags)
                SensitiveDetectorList += [ result.popToolsAndMerge(accTGC) ]
        else:
            if ConfigFlags.Detector.SimulateMDT :
                accMDT =  MDTSensitiveDetectorCfg(ConfigFlags)
                SensitiveDetectorList += [ result.popToolsAndMerge(accMDT) ]
            if ConfigFlags.Detector.SimulateRPC : 
                accRPC = RPCSensitiveDetectorCfg(ConfigFlags) 
                SensitiveDetectorList += [ result.popToolsAndMerge(accRPC) ]
            if ConfigFlags.Detector.SimulateTGC :
                accTGC = TGCSensitiveDetectorCfg(ConfigFlags)
                SensitiveDetectorList += [ result.popToolsAndMerge(accTGC) ]
        

        #if ( hasattr(simFlags, 'SimulateNewSmallWheel') and simFlags.SimulateNewSmallWheel() ) or ConfigFlags.GeoModel.Run =="RUN3" :
        if False:
        #if ConfigFlags.SimulateNewSmallWheel or ConfigFlags.GeoModel.Run =="RUN3" :
            if ConfigFlags.Detector.Simulate.sTGC :
                acc_sTGC = sTGCSensitiveDetectorCfg(ConfigFlags)
                SensitiveDetectorList += [ result.popToolsAndMerge(acc_sTGC) ]
            if ConfigFlags.Detector.Simulate.Micromegas :
                accMicromegas = MicromegasSensitiveDetectorCfg(ConfigFlags)
                SensitiveDetectorList += [ result.popToolsAndMerge(accMicromegas) ]
        elif ConfigFlags.Detector.SimulateCSC:
            # CSCs built instead of NSW
            if ConfigFlags.Beam.Type == 'cosmics':
                accCSC = CSCSensitiveDetectorCosmicsCfg(ConfigFlags)
                SensitiveDetectorList += [ result.popToolsAndMerge(accCSC) ]
            else:
                accCSC = CSCSensitiveDetectorCfg(ConfigFlags)
                SensitiveDetectorList += [ result.popToolsAndMerge(accCSC) ]

    result.setPrivateTools(SensitiveDetectorList)
    return result


def generateEnvelopeSensitiveDetectorList(ConfigFlags):
    SensitiveDetectorList=[]
    if ConfigFlags.Beam.Type == 'cosmics' and not ConfigFlags.Sim.ReadTR:
        SensitiveDetectorList+=['CosmicRecord']
    return SensitiveDetectorList


def generateSensitiveDetectorList(ConfigFlags):
    result = ComponentAccumulator()
    SensitiveDetectorList=[]
    SensitiveDetectorList += generateEnvelopeSensitiveDetectorList(ConfigFlags) # to update

    acc_InDetSensitiveDetector, InDetSensitiveDetectorList = generateInDetSensitiveDetectorList(ConfigFlags)
    SensitiveDetectorList += InDetSensitiveDetectorList


    acc_CaloSensitiveDetector = generateCaloSensitiveDetectorList(ConfigFlags)
    SensitiveDetectorList+=result.popToolsAndMerge(acc_CaloSensitiveDetector)

    accMuon = generateMuonSensitiveDetectorList(ConfigFlags)
    SensitiveDetectorList += result.popToolsAndMerge(accMuon)


    SensitiveDetectorList += generateTrackFastSimSensitiveDetectorList(ConfigFlags)
    SensitiveDetectorList += generateFwdSensitiveDetectorList(ConfigFlags)

    result.merge(acc_InDetSensitiveDetector)

    result.setPrivateTools(SensitiveDetectorList)
    return result


def generateTestBeamSensitiveDetectorList(ConfigFlags):
    result = ComponentAccumulator()
    SensitiveDetectorList=[]

    if "tb_Tile2000_2003" in ConfigFlags.GeoModel.AtlasVersion:
        if ConfigFlags.Detector.SimulateTile:
            if ConfigFlags.Sim.CalibrationRun in ['Tile', 'LAr+Tile']:
                accTileCTB = TileCTBGeoG4CalibSDCfg(ConfigFlags)
                SensitiveDetectorList += [ result.popToolsAndMerge( accTileCTB) ] # mode 1 : With CaloCalibrationHits
            else:
                accTileCTB =  TileCTBGeoG4SDCfg(ConfigFlags)
                SensitiveDetectorList += [ result.popToolsAndMerge( accTileCTB ) ]      # mode 0 : No CaloCalibrationHits
                if ConfigFlags.Detector.SimulateCalo:
                    SensitiveDetectorList += [ 'MuonWallSD' ]
        return result, SensitiveDetectorList

    if ConfigFlags.Detector.SimulatePixel:
        toolPixel = PixelSensor_CTBCfg()
        SensitiveDetectorList += [ toolPixel ]
    if ConfigFlags.Detector.SimulateSCT:
        toolSCT = SctSensor_CTBCfg()
        SensitiveDetectorList += [ toolSCT ]
    if ConfigFlags.Detector.SimulateTRT:
        toolTRT = TRTSensitiveDetector_CTBCfg()
        SensitiveDetectorList += [ toolTRT ]
    if ConfigFlags.Detector.SimulateLAr:
        SensitiveDetectorList += [ 'LArEMBSensitiveDetector' ]
        if 'LAr' in ConfigFlags.Sim.CalibrationRun:
            SensitiveDetectorList += [ 'LArH8CalibSensitiveDetector' ] # mode 1 : With CaloCalibrationHits
    if ConfigFlags.Detector.SimulateTile:
        if ConfigFlags.Sim.CalibrationRun in ['Tile', 'LAr+Tile']:
            accTileCTB = TileCTBGeoG4CalibSDCfg(ConfigFlags)
            SensitiveDetectorList += [ result.popToolsAndMerge( accTileCTB ) ] # mode 1 : With CaloCalibrationHits
        else:
            accTileCTB =  TileCTBGeoG4SDCfg(ConfigFlags)
            SensitiveDetectorList += [ result.popToolsAndMerge( accTileCTB ) ]      # mode 0 : No CaloCalibrationHits
            SensitiveDetectorList += [ 'MuonWallSD' ]
    #if ConfigFlags.Detector.geometry.Muon_on():
    if ConfigFlags.Detector.SimulateMuon:
        SensitiveDetectorList += [ 'MuonEntryRecord' ]
    SensitiveDetectorList += generateMuonSensitiveDetectorList(ConfigFlags)

    result.setPrivateTools(SensitiveDetectorList)
    return result


def SensitiveDetectorMasterToolCfg(ConfigFlags, name="SensitiveDetectorMasterTool", **kwargs):
    result = ComponentAccumulator()
    if "ATLAS" in ConfigFlags.GeoModel.AtlasVersion:
        accSensitiveDetector = generateSensitiveDetectorList(ConfigFlags)
        kwargs.setdefault("SensitiveDetectors", result.popToolsAndMerge(accSensitiveDetector)) #list of tools
    elif "tb_Tile2000_2003" in ConfigFlags.GeoModel.AtlasVersion:

        accSensitiveDetector = generateTestBeamSensitiveDetectorList(ConfigFlags)
        kwargs.setdefault("SensitiveDetectors", result.popToolsAndMerge(accSensitiveDetector) ) #list of tools here!
    elif "tb_LArH6" in ConfigFlags.GeoModel.AtlasVersion:
        pass
    elif "ctbh8" in ConfigFlags.GeoModel.AtlasVersion:

        accSensitiveDetector = generateTestBeamSensitiveDetectorList(ConfigFlags)
        kwargs.setdefault("SensitiveDetectors", result.popToolsAndMerge(accSensitiveDetector) ) #list of tools here!

    result.addPublicTool(SensitiveDetectorMasterTool(name, **kwargs)) #note -this is still a public tool
    return result


def getEmptySensitiveDetectorMasterTool(name="EmptySensitiveDetectorMasterTool", **kwargs):
    result = ComponentAccumulator()
    result.addPublicTool(SensitiveDetectorMasterTool(name, **kwargs))
    return result
