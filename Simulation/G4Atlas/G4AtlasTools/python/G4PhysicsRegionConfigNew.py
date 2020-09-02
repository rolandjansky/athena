# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
from __future__ import print_function
from AthenaConfiguration.ComponentFactory import CompFactory

RegionCreator=CompFactory.RegionCreator

# Beampipe Regions
def BeampipeFwdCutPhysicsRegionToolCfg(ConfigFlags, name='BeampipeFwdCutPhysicsRegionTool', **kwargs):
    kwargs.setdefault("RegionName", 'BeampipeFwdCut')
    volumeList = ['BeamPipe::SectionF47', 'BeamPipe::SectionF48', 'BeamPipe::SectionF61']
    kwargs.setdefault("VolumeList",  volumeList)
    from G4AtlasApps.SimFlags import simFlags

    #if simFlags.BeamPipeSimMode() == "FastSim":
    if True:
        kwargs.setdefault("ElectronCut", 10.)
        kwargs.setdefault("PositronCut", 10.)
        kwargs.setdefault("GammaCut", 10.)
        print('Adding fast sim model to the beampipe!')
    else:
        assert simFlags.BeamPipeCut.statusOn
        if simFlags.BeamPipeCut() < 1:
            msg = "Setting the forward beam pipe range cuts to %e mm " % simFlags.BeamPipeCut()
            msg += "-- cut is < 1 mm, I hope you know what you're doing!"
            print(msg)
        if simFlags.BeamPipeSimMode() == "EGammaRangeCuts":
            kwargs.setdefault("ElectronCut", simFlags.BeamPipeCut())
            kwargs.setdefault("PositronCut", simFlags.BeamPipeCut())
            kwargs.setdefault("GammaCut", simFlags.BeamPipeCut())
        elif simFlags.BeamPipeSimMode() == "EGammaPRangeCuts":
            kwargs.setdefault("ElectronCut", simFlags.BeamPipeCut())
            kwargs.setdefault("PositronCut", simFlags.BeamPipeCut())
            kwargs.setdefault("GammaCut", simFlags.BeamPipeCut())
            kwargs.setdefault("ProtonCut", simFlags.BeamPipeCut())
    return RegionCreator(name, **kwargs)

def FWDBeamLinePhysicsRegionToolCfg(ConfigFlags, name='FWDBeamLinePhysicsRegionTool', **kwargs):
    kwargs.setdefault("RegionName", 'FWDBeamLine')
    volumeList = ['BeamPipe::SectionF46']
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
    from AtlasGeoModel.CommonGMJobProperties import CommonGeometryFlags as commonGeoFlags
    from AtlasGeoModel.InDetGMJobProperties import InDetGeometryFlags as geoFlags
    if (ConfigFlags.GeoModel.Run in ["RUN2", "RUN3"]) or (commonGeoFlags.Run()=="UNDEFINED" and geoFlags.isIBL()):
        volumeList += ['Pixel::dbmDiamondLog']
    kwargs.setdefault("VolumeList",  volumeList)
    kwargs.setdefault("ElectronCut", 0.05)
    kwargs.setdefault("PositronCut", 0.05)
    kwargs.setdefault("GammaCut",    0.05)
    return RegionCreator(name, **kwargs)

def SCTPhysicsRegionToolCfg(ConfigFlags, name='SCTPhysicsRegionTool', **kwargs):
    kwargs.setdefault("RegionName", 'SCT')
    volumeList = ['SCT::BRLSensor', 'SCT::BRLSensorSS', 'SCT::BRLSensorMS',
                   'SCT::ECSensor0', 'SCT::ECSensor1', 'SCT::ECSensor2',
                   'SCT::ECSensor3', 'SCT::ECSensor4', 'SCT::ECSensor5']
    kwargs.setdefault("VolumeList",  volumeList)
    kwargs.setdefault("ElectronCut", 0.05)
    kwargs.setdefault("PositronCut", 0.05)
    kwargs.setdefault("GammaCut",    0.05)
    return RegionCreator(name, **kwargs)

def ITkPixelPhysicsRegionToolCfg(ConfigFlags, name='ITkPixelPhysicsRegionTool', **kwargs):
    kwargs.setdefault("RegionName", 'ITkPixel')
    volumeList = ['ITkPixel::siLog']
    kwargs.setdefault("VolumeList",  volumeList)
    kwargs.setdefault("ElectronCut", 0.05)
    kwargs.setdefault("PositronCut", 0.05)
    kwargs.setdefault("GammaCut",    0.05)
    return RegionCreator(name, **kwargs)

def ITkStripPhysicsRegionToolCfg(ConfigFlags, name='ITkStripPhysicsRegionTool', **kwargs):
    kwargs.setdefault("RegionName", 'ITkStrip')
    #Need to find what these should be set to for ITk...
    volumeList = ['SCT::BRLSensor', 'SCT::BRLSensorSS', 'SCT::BRLSensorMS',
                   'SCT::ECSensor0', 'SCT::ECSensor1', 'SCT::ECSensor2',
                   'SCT::ECSensor3', 'SCT::ECSensor4', 'SCT::ECSensor5']
    kwargs.setdefault("VolumeList",  volumeList)
    kwargs.setdefault("ElectronCut", 0.05)
    kwargs.setdefault("PositronCut", 0.05)
    kwargs.setdefault("GammaCut",    0.05)
    return RegionCreator(name, **kwargs)

def TRTPhysicsRegionToolCfg(ConfigFlags, name='TRTPhysicsRegionTool', **kwargs):
    #rangeCut = simFlags.TRTRangeCut._Value()
    rangeCut = 30.0
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
    from G4AtlasApps.SimFlags import simFlags
    if '_EMV' not in simFlags.PhysicsList() and '_EMX' not in simFlags.PhysicsList():
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
    from G4AtlasApps.SimFlags import simFlags
    if '_EMV' not in simFlags.PhysicsList() and '_EMX' not in simFlags.PhysicsList():
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
    from G4AtlasApps.SimFlags import simFlags
    if '_EMV' not in simFlags.PhysicsList() and '_EMX' not in simFlags.PhysicsList():
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
    from G4AtlasApps.SimFlags import simFlags
    if '_EMV' not in simFlags.PhysicsList() and '_EMX' not in simFlags.PhysicsList():
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
    sectionList = list(range(16,49)) # does not include 49
    sectionList += [ 51, 52, 53, 54 ]
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
    if ConfigFlags.Sim.CavernBG  == 'World':
        volumeList += ['BeamPipe::BeamPipe', 'IDET::IDET']
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
