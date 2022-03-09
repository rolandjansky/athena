# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
from AthenaConfiguration.ComponentFactory import CompFactory
from AthenaConfiguration.Enums import LHCPeriod
from G4AtlasApps.SimEnums import BeamPipeSimMode

RegionCreator=CompFactory.RegionCreator

# Beampipe Regions
def BeampipeFwdCutPhysicsRegionToolCfg(ConfigFlags, name='BeampipeFwdCutPhysicsRegionTool', **kwargs):
    kwargs.setdefault("RegionName", 'BeampipeFwdCut')
    volumeList = []
    if ConfigFlags.GeoModel.Run is LHCPeriod.Run1:
        volumeList = ['BeamPipe::SectionF47', 'BeamPipe::SectionF48', 'BeamPipe::SectionF61']
    else:
        volumeList = ['BeamPipe::SectionF198', 'BeamPipe::SectionF199', 'BeamPipe::SectionF200']
        if ConfigFlags.GeoModel.Run > LHCPeriod.Run4:
            print('BeampipeFwdCutPhysicsRegionToolCfg: WARNING check that RUN2 beampipe volume names are correct for this geometry tag')
    kwargs.setdefault("VolumeList",  volumeList)

    if ConfigFlags.Sim.BeamPipeSimMode is BeamPipeSimMode.FastSim:
        kwargs.setdefault("ElectronCut", 10.)
        kwargs.setdefault("PositronCut", 10.)
        kwargs.setdefault("GammaCut", 10.)
        print('Adding fast sim model to the beampipe!')
    else:
        assert ConfigFlags.Sim.BeamPipeCut
        if ConfigFlags.Sim.BeamPipeCut < 1:
            msg = "Setting the forward beam pipe range cuts to %e mm " % ConfigFlags.Sim.BeamPipeCut
            msg += "-- cut is < 1 mm, I hope you know what you're doing!"
            print(msg)
        if ConfigFlags.Sim.BeamPipeSimMode is BeamPipeSimMode.EGammaRangeCuts:
            kwargs.setdefault("ElectronCut", ConfigFlags.Sim.BeamPipeCut)
            kwargs.setdefault("PositronCut", ConfigFlags.Sim.BeamPipeCut)
            kwargs.setdefault("GammaCut", ConfigFlags.Sim.BeamPipeCut)
        elif ConfigFlags.Sim.BeamPipeSimMode is BeamPipeSimMode.EGammaPRangeCuts:
            kwargs.setdefault("ElectronCut", ConfigFlags.Sim.BeamPipeCut)
            kwargs.setdefault("PositronCut", ConfigFlags.Sim.BeamPipeCut)
            kwargs.setdefault("GammaCut", ConfigFlags.Sim.BeamPipeCut)
            kwargs.setdefault("ProtonCut", ConfigFlags.Sim.BeamPipeCut)
    return RegionCreator(name, **kwargs)

def FWDBeamLinePhysicsRegionToolCfg(ConfigFlags, name='FWDBeamLinePhysicsRegionTool', **kwargs):
    kwargs.setdefault("RegionName", 'FWDBeamLine')
    if ConfigFlags.GeoModel.Run is LHCPeriod.Run1:
        volumeList = ['BeamPipe::SectionF46']
    else:
        volumeList = ['BeamPipe::SectionF197']
        if ConfigFlags.GeoModel.Run > LHCPeriod.Run4:
            print('FWDBeamLinePhysicsRegionToolCfg: WARNING check that RUN2 beampipe volume names are correct for this geometry tag')
    kwargs.setdefault("VolumeList",  volumeList)
    return RegionCreator(name, **kwargs)

# Forward Regions
def FwdRegionPhysicsRegionToolCfg(ConfigFlags, name='FwdRegionPhysicsRegionTool', **kwargs):
    kwargs.setdefault("RegionName", 'FwdRegion')
    volumeList = ['FwdRegion::ForwardRegionGeoModel']
    kwargs.setdefault("VolumeList",  volumeList)
    kwargs.setdefault("ElectronCut", 200.)
    kwargs.setdefault("PositronCut", 200.)
    kwargs.setdefault("GammaCut",    200.)
    return RegionCreator(name, **kwargs)

# Inner Detector Regions
def PixelPhysicsRegionToolCfg(ConfigFlags, name='PixelPhysicsRegionTool', **kwargs):
    kwargs.setdefault("RegionName", 'Pixel')
    volumeList = ['Pixel::siLog', 'Pixel::siBLayLog']
    if ConfigFlags.GeoModel.Run in [LHCPeriod.Run2, LHCPeriod.Run3]:
        # TODO: should we support old geometry tags with Run == "UNDEFINED" and ConfigFlags.GeoModel.IBLLayout not in ["noIBL", "UNDEFINED"]?
        volumeList += ['Pixel::dbmDiamondLog']
    kwargs.setdefault("VolumeList",  volumeList)
    # The range cuts used here are directly linked to the minimum energy of delta rays.
    # The minimum energy of delta rays in an input to the digitisation when using Bichsel charge deposition model.
    # The range cut is equated to an energy threshold in the simulation log file
    # If these change please update the digitisation cuts appropriately. 
    kwargs.setdefault("ElectronCut", 0.05)
    kwargs.setdefault("PositronCut", 0.05)
    kwargs.setdefault("GammaCut",    0.05)
    return RegionCreator(name, **kwargs)

def SCTPhysicsRegionToolCfg(ConfigFlags, name='SCTPhysicsRegionTool', **kwargs):
    kwargs.setdefault("RegionName", 'SCT')
    volumeList = ['SCT::BRLSensor', 'SCT::ECSensor0', 'SCT::ECSensor1', 
                  'SCT::ECSensor2','SCT::ECSensor3']
    kwargs.setdefault("VolumeList",  volumeList)
    kwargs.setdefault("ElectronCut", 0.05)
    kwargs.setdefault("PositronCut", 0.05)
    kwargs.setdefault("GammaCut",    0.05)
    return RegionCreator(name, **kwargs)

def ITkPixelPhysicsRegionToolCfg(ConfigFlags, name='ITkPixelPhysicsRegionTool', **kwargs):
    kwargs.setdefault("RegionName", 'ITkPixel')
    volumeList = ['ITkPixel::InnerBarrelSingleMod_Sensor',
                  'ITkPixel::InnerRingSingleMod_Sensor',
                  'ITkPixel::InnerQuadMod_Sensor',
                  'ITkPixel::OuterQuadMod_Sensor',
                  'ITkPixel::InclinedQuadMod_Sensor']
    kwargs.setdefault("VolumeList",  volumeList)
    kwargs.setdefault("ElectronCut", 0.05)
    kwargs.setdefault("PositronCut", 0.05)
    kwargs.setdefault("GammaCut",    0.05)
    return RegionCreator(name, **kwargs)

def ITkStripPhysicsRegionToolCfg(ConfigFlags, name='ITkStripPhysicsRegionTool', **kwargs):
    kwargs.setdefault("RegionName", 'ITkStrip')
    volumeList = ['ITkStrip::BRLSensorSS', 'ITkStrip::BRLSensorMS',
                  'ITkStrip::ECSensor0', 'ITkStrip::ECSensor1', 'ITkStrip::ECSensor2',
                  'ITkStrip::ECSensor3', 'ITkStrip::ECSensor4', 'ITkStrip::ECSensor5']
    kwargs.setdefault("VolumeList",  volumeList)
    kwargs.setdefault("ElectronCut", 0.05)
    kwargs.setdefault("PositronCut", 0.05)
    kwargs.setdefault("GammaCut",    0.05)
    return RegionCreator(name, **kwargs)

def HGTDPhysicsRegionToolCfg(ConfigFlags, name='HGTDPhysicsRegionTool', **kwargs):
    kwargs.setdefault("RegionName", 'HGTD')
    volumeList = ['HGTD::HGTDSiSensor0', 'HGTD::HGTDSiSensor1',
                  'HGTD::HGTDSiSensor2', 'HGTD::HGTDSiSensor3']
    kwargs.setdefault("VolumeList",  volumeList)
    kwargs.setdefault("ElectronCut", 0.05)
    kwargs.setdefault("PositronCut", 0.05)
    kwargs.setdefault("GammaCut",    0.05)
    return RegionCreator(name, **kwargs)

def TRTPhysicsRegionToolCfg(ConfigFlags, name='TRTPhysicsRegionTool', **kwargs):
    rangeCut = ConfigFlags.Sim.TRTRangeCut
    kwargs.setdefault("RegionName", 'TRT')
    volumeList = ['TRT::Gas', 'TRT::GasMA']
    kwargs.setdefault("VolumeList",  volumeList)
    kwargs.setdefault("ElectronCut", rangeCut)
    kwargs.setdefault("PositronCut", rangeCut)
    # The photon range cut is meant to stay small
    kwargs.setdefault("GammaCut",    0.05)
    return RegionCreator(name, **kwargs)

def TRT_ArPhysicsRegionToolCfg(ConfigFlags, name='TRT_ArPhysicsRegionTool', **kwargs):
    kwargs.setdefault("RegionName", 'TRT_Ar')
    volumeList = ['TRT::Gas_Ar', 'TRT::GasMA_Ar']
    kwargs.setdefault("VolumeList",  volumeList)
    kwargs.setdefault("ElectronCut", 30.0)
    kwargs.setdefault("PositronCut", 30.0)
    kwargs.setdefault("GammaCut",    0.05)
    return RegionCreator(name, **kwargs)

def TRT_KrPhysicsRegionToolCfg(ConfigFlags, name='TRT_KrPhysicsRegionTool', **kwargs):
    kwargs.setdefault("RegionName", 'TRT_Kr')
    volumeList = ['TRT::Gas_Kr', 'TRT::GasMA_Kr']
    kwargs.setdefault("VolumeList",  volumeList)
    kwargs.setdefault("ElectronCut", 15.0)
    kwargs.setdefault("PositronCut", 15.0)
    kwargs.setdefault("GammaCut",    0.05)
    return RegionCreator(name, **kwargs)

# Calo Regions
def EMBPhysicsRegionToolCfg(ConfigFlags, name='EMBPhysicsRegionTool', **kwargs):
    kwargs.setdefault("RegionName", 'EMB')
    volumeList = ['LArMgr::LAr::EMB::STAC']
    kwargs.setdefault("VolumeList",  volumeList)
    rangeEMB = 0.03
    if '_EMV' not in ConfigFlags.Sim.PhysicsList and '_EMX' not in ConfigFlags.Sim.PhysicsList:
        rangeEMB = 0.1
    kwargs.setdefault("ElectronCut", rangeEMB)
    kwargs.setdefault("PositronCut", rangeEMB)
    kwargs.setdefault("GammaCut",    rangeEMB)
    return RegionCreator(name, **kwargs)

def EMECPhysicsRegionToolCfg(ConfigFlags, name='EMECPhysicsRegionTool', **kwargs):
    kwargs.setdefault("RegionName", 'EMEC')
    volumeList = ['LArMgr::LAr::EMEC::Mother']
    kwargs.setdefault("VolumeList",  volumeList)
    rangeEMEC = 0.03
    if '_EMV' not in ConfigFlags.Sim.PhysicsList and '_EMX' not in ConfigFlags.Sim.PhysicsList:
        rangeEMEC = 0.1
    kwargs.setdefault("ElectronCut", rangeEMEC)
    kwargs.setdefault("PositronCut", rangeEMEC)
    kwargs.setdefault("GammaCut",    rangeEMEC)
    return RegionCreator(name, **kwargs)

def HECPhysicsRegionToolCfg(ConfigFlags, name='HECPhysicsRegionTool', **kwargs):
    kwargs.setdefault("RegionName", 'HEC')
    volumeList = ['LArMgr::LAr::HEC::LiquidArgon']
    kwargs.setdefault("VolumeList",  volumeList)
    rangeHEC = 0.03
    if '_EMV' not in ConfigFlags.Sim.PhysicsList and '_EMX' not in ConfigFlags.Sim.PhysicsList:
        rangeHEC = 1.0
    kwargs.setdefault("ElectronCut", rangeHEC)
    kwargs.setdefault("PositronCut", rangeHEC)
    kwargs.setdefault("GammaCut",    rangeHEC)
    return RegionCreator(name, **kwargs)

def FCALPhysicsRegionToolCfg(ConfigFlags, name='FCALPhysicsRegionTool', **kwargs):
    kwargs.setdefault("RegionName", 'FCAL')
    volumeList = ['LArMgr::LAr::FCAL::LiquidArgonC']
    kwargs.setdefault("VolumeList",  volumeList)
    kwargs.setdefault("ElectronCut", 0.03)
    kwargs.setdefault("PositronCut", 0.03)
    kwargs.setdefault("GammaCut",    0.03)
    return RegionCreator(name, **kwargs)

def EMECParaPhysicsRegionToolCfg(ConfigFlags, name='EMECParaPhysicsRegionTool', **kwargs):
    kwargs.setdefault("RegionName", 'EMECPara')
    volumeList = ['LArMgr::LAr::EMEC::Pos::InnerWheel', 'LArMgr::LAr::EMEC::Pos::OuterWheel',
                  'LArMgr::LAr::EMEC::Neg::InnerWheel', 'LArMgr::LAr::EMEC::Neg::OuterWheel']
    kwargs.setdefault("VolumeList",  volumeList)
    rangeEMEC = 0.03
    if '_EMV' not in ConfigFlags.Sim.PhysicsList and '_EMX' not in ConfigFlags.Sim.PhysicsList:
        rangeEMEC = 0.1
    kwargs.setdefault("ElectronCut", rangeEMEC)
    kwargs.setdefault("PositronCut", rangeEMEC)
    kwargs.setdefault("GammaCut",    rangeEMEC)
    return RegionCreator(name, **kwargs)

def FCALParaPhysicsRegionToolCfg(ConfigFlags, name='FCALParaPhysicsRegionTool', **kwargs):
    kwargs.setdefault("RegionName", 'FCALPara')
    volumeList = ['LArMgr::LAr::FCAL::Module1::Absorber']
    kwargs.setdefault("VolumeList",  volumeList)
    kwargs.setdefault("ElectronCut", 0.03)
    kwargs.setdefault("PositronCut", 0.03)
    kwargs.setdefault("GammaCut",    0.03)
    return RegionCreator(name, **kwargs)

def FCAL2ParaPhysicsRegionToolCfg(ConfigFlags, name='FCAL2ParaPhysicsRegionTool', **kwargs):
    kwargs.setdefault("RegionName", 'FCAL2Para')
    volumeList = ['LArMgr::LAr::FCAL::Module2::Absorber', 'LArMgr::LAr::FCAL::Module3::Absorber']
    kwargs.setdefault("VolumeList",  volumeList)
    kwargs.setdefault("ElectronCut", 0.03)
    kwargs.setdefault("PositronCut", 0.03)
    kwargs.setdefault("GammaCut",    0.03)
    return RegionCreator(name, **kwargs)

def PreSampLArPhysicsRegionToolCfg(ConfigFlags, name='PreSampLArPhysicsRegionTool', **kwargs):
    kwargs.setdefault("RegionName", 'PreSampLAr')
    volumeList = ['LArMgr::LAr::Endcap::Presampler::LiquidArgon']
    kwargs.setdefault("VolumeList",  volumeList)
    return RegionCreator(name, **kwargs)

def DeadMaterialPhysicsRegionToolCfg(ConfigFlags, name='DeadMaterialPhysicsRegionTool', **kwargs):
    kwargs.setdefault("RegionName", 'DeadMaterial')
    volumeList = []
    sectionList = []
    if ConfigFlags.GeoModel.Run is LHCPeriod.Run1:
        sectionList = list(range(16,49)) # does not include 49
        sectionList += [ 51, 52, 53, 54 ]
    else:
        sectionList = list(range(191,200)) # does not include 200
        if ConfigFlags.GeoModel.Run > LHCPeriod.Run4:
            print('DeadMaterialPhysicsRegionToolCfg: WARNING check that RUN2 beampipe volume names are correct for this geometry tag')
    for section in sectionList:
        volumeList += ['BeamPipe::SectionF'+str(section)]
    volumeList += ['LArMgr::LAr::Endcap::Cryostat::Cylinder',
                   'LArMgr::LAr::Endcap::Cryostat::Cylinder::Mixed',
                   'LArMgr::LAr::Endcap::Cryostat::Cone::Mixed',
                   'LArMgr::LAr::Endcap::Cryostat::Cone',
                   'DiskShieldingPlugs', 'ToroidShieldingInnerPlugs',
                   'ForwardShieldingMainCylinder']
    kwargs.setdefault("VolumeList",  volumeList)
    kwargs.setdefault("ElectronCut", 1.0)
    kwargs.setdefault("PositronCut", 1.0)
    kwargs.setdefault("GammaCut",    1.0)
    return RegionCreator(name, **kwargs)

#Muon Regions
def DriftWallPhysicsRegionToolCfg(ConfigFlags, name='DriftWallPhysicsRegionTool', **kwargs):
    kwargs.setdefault("RegionName", 'DriftWall')
    volumeList = ['Muon::MDTDriftWall']
    kwargs.setdefault("VolumeList",  volumeList)
    kwargs.setdefault("ElectronCut", 0.05)
    kwargs.setdefault("PositronCut", 0.05)
    return RegionCreator(name, **kwargs)

def DriftWall1PhysicsRegionToolCfg(ConfigFlags, name='DriftWall1PhysicsRegionTool', **kwargs):
    kwargs.setdefault("RegionName", 'DriftWall1')
    volumeList = ['Muon::Endplug']
    kwargs.setdefault("VolumeList",  volumeList)
    kwargs.setdefault("ElectronCut", 1.0)
    kwargs.setdefault("PositronCut", 1.0)
    return RegionCreator(name, **kwargs)

def DriftWall2PhysicsRegionToolCfg(ConfigFlags, name='DriftWall2PhysicsRegionTool', **kwargs):
    kwargs.setdefault("RegionName", 'DriftWall2')
    volumeList = ['Muon::SensitiveGas']
    kwargs.setdefault("VolumeList",  volumeList)
    kwargs.setdefault("ElectronCut", 1.0)
    kwargs.setdefault("PositronCut", 1.0)
    return RegionCreator(name, **kwargs)

def MuonSystemFastPhysicsRegionToolCfg(ConfigFlags, name='MuonSystemFastPhysicsRegionTool', **kwargs):
    kwargs.setdefault("RegionName", 'MuonSystemFastRegion')
    volumeList = []
    from G4AtlasApps.SimEnums import CavernBackground
    if ConfigFlags.Sim.CavernBackground in [CavernBackground.SignalWorld, CavernBackground.WriteWorld]:
        if ConfigFlags.GeoModel.Run < LHCPeriod.Run4:
            volumeList += ['BeamPipe::BeamPipe', 'IDET::IDET']
        else:
            volumeList += ['BeamPipe::BeamPipe', 'ITK::ITK']
    volumeList = ['Muon::MuonSys']
    kwargs.setdefault("VolumeList",  volumeList)
    kwargs.setdefault("ElectronCut", 1.0)
    kwargs.setdefault("PositronCut", 1.0)
    return RegionCreator(name, **kwargs)

def MuonPhysicsRegionToolCfg(ConfigFlags, name="MuonPhysicsRegionTool", **kwargs):
    kwargs.setdefault("RegionName", 'MuonSys')
    volumeList = ['Muon::MuonSys']
    kwargs.setdefault("VolumeList",  volumeList)
    kwargs.setdefault("ElectronCut", 0.1)
    kwargs.setdefault("PositronCut", 0.1)
    kwargs.setdefault("GammaCut",    0.1)
    return RegionCreator(name, **kwargs)

# Cavern Regions
def SX1PhysicsRegionToolCfg(ConfigFlags, name='SX1PhysicsRegionTool', **kwargs):
    kwargs.setdefault("RegionName", 'SX1')
    volumeList = ['CavernInfra::SX1Air']
    kwargs.setdefault("VolumeList",  volumeList)
    kwargs.setdefault("ElectronCut", 2000.)
    kwargs.setdefault("PositronCut", 2000.)
    kwargs.setdefault("GammaCut",    2000.)
    return RegionCreator(name, **kwargs)

def BedrockPhysicsRegionToolCfg(ConfigFlags, name='BedrockPhysicsRegionTool', **kwargs):
    kwargs.setdefault("RegionName", 'BEDROCK')
    volumeList = ['CavernInfra::BEDROCK']
    kwargs.setdefault("VolumeList",  volumeList)
    kwargs.setdefault("ElectronCut", 1000000.)
    kwargs.setdefault("PositronCut", 1000000.)
    kwargs.setdefault("GammaCut",    1000000.)
    return RegionCreator(name, **kwargs)

def CavernShaftsConcretePhysicsRegionToolCfg(ConfigFlags, name='CavernShaftsConcretePhysicsRegionTool', **kwargs):
    kwargs.setdefault("RegionName", 'CAV_SHAFTS_CONC')
    volumeList = ['CavernInfra::CAV_SHAFTS_CONC']
    kwargs.setdefault("VolumeList",  volumeList)
    kwargs.setdefault("ElectronCut", 150.)
    kwargs.setdefault("PositronCut", 150.)
    kwargs.setdefault("GammaCut",    150.)
    return RegionCreator(name, **kwargs)

##def getCavernShaftsAirPhysicsRegionTool(name='CavernShaftsAirPhysicsRegionTool', **kwargs):
##    kwargs.setdefault("RegionName", 'CAV_SHAFTS_AIR')
##    volumeList = ['CavernInfra::CAV_SHAFTS_AIR']
##    kwargs.setdefault("VolumeList",  volumeList)
##    kwargs.setdefault("ElectronCut", 2000.)
##    kwargs.setdefault("PositronCut", 2000.)
##    kwargs.setdefault("GammaCut",    2000.)
##    return CfgMgr.RegionCreator(name, **kwargs)

# CTB Regions

def SCTSiliconPhysicsRegionToolCfg(ConfigFlags, name='SCTSiliconPhysicsRegionTool', **kwargs):
    kwargs.setdefault("RegionName", 'SCTSiliconRegion')
    volumeList = ['SCT::ECSensor0']
    kwargs.setdefault("VolumeList",  volumeList)
    kwargs.setdefault("ElectronCut", 0.01)
    return RegionCreator(name, **kwargs)
