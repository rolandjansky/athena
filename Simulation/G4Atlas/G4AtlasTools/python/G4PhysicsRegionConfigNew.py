# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from G4AtlasTools.G4AtlasToolsConf import RegionCreator

# Beampipe Regions
def getBeampipeFwdCutPhysicsRegionTool(name='BeampipeFwdCutPhysicsRegionTool', **kwargs):
    kwargs.setdefault("RegionName", 'BeampipeFwdCut')
    volumeList = ['BeamPipe::SectionF47', 'BeamPipe::SectionF48', 'BeamPipe::SectionF61']
    kwargs.setdefault("VolumeList",  volumeList)
    from G4AtlasApps.SimFlags import simFlags
    if simFlags.BeamPipeSimMode() == "FastSim":
        kwargs.setdefault("ElectronCut", 10.)
        kwargs.setdefault("PositronCut", 10.)
        kwargs.setdefault("GammaCut", 10.)
        print 'Adding fast sim model to the beampipe!'
    else:
        assert simFlags.BeamPipeCut.statusOn
        if simFlags.BeamPipeCut() < 1:
            msg = "Setting the forward beam pipe range cuts to %e mm " % simFlags.BeamPipeCut()
            msg += "-- cut is < 1 mm, I hope you know what you're doing!"
            print msg
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

def getFWDBeamLinePhysicsRegionTool(name='FWDBeamLinePhysicsRegionTool', **kwargs):
    kwargs.setdefault("RegionName", 'FWDBeamLine')
    volumeList = ['BeamPipe::SectionF46']
    kwargs.setdefault("VolumeList",  volumeList)
    return RegionCreator(name, **kwargs)

# Forward Regions
def getFwdRegionPhysicsRegionTool(name='FwdRegionPhysicsRegionTool', **kwargs):
    kwargs.setdefault("RegionName", 'FwdRegion')
    volumeList = ['FwdRegion::ForwardRegionGeoModel']
    kwargs.setdefault("VolumeList",  volumeList)
    kwargs.setdefault("ElectronCut", 200.)
    kwargs.setdefault("PositronCut", 200.)
    kwargs.setdefault("GammaCut",    200.)
    return RegionCreator(name, **kwargs)

# Inner Detector Regions
def getPixelPhysicsRegionTool(name='PixelPhysicsRegionTool', **kwargs):
    kwargs.setdefault("RegionName", 'Pixel')
    volumeList = ['Pixel::siLog', 'Pixel::siBLayLog']
    from AtlasGeoModel.CommonGMJobProperties import CommonGeometryFlags as commonGeoFlags
    from AtlasGeoModel.InDetGMJobProperties import InDetGeometryFlags as geoFlags
    if (commonGeoFlags.Run() in ["RUN2", "RUN3"]) or (commonGeoFlags.Run()=="UNDEFINED" and geoFlags.isIBL()):
        volumeList += ['Pixel::dbmDiamondLog']
    kwargs.setdefault("VolumeList",  volumeList)
    kwargs.setdefault("ElectronCut", 0.05)
    kwargs.setdefault("PositronCut", 0.05)
    kwargs.setdefault("GammaCut",    0.05)
    return RegionCreator(name, **kwargs)

def getSCTPhysicsRegionTool(name='SCTPhysicsRegionTool', **kwargs):
    kwargs.setdefault("RegionName", 'SCT')
    volumeList = ['SCT::BRLSensor', 'SCT::BRLSensorSS', 'SCT::BRLSensorMS',
                   'SCT::ECSensor0', 'SCT::ECSensor1', 'SCT::ECSensor2',
                   'SCT::ECSensor3', 'SCT::ECSensor4', 'SCT::ECSensor5']
    kwargs.setdefault("VolumeList",  volumeList)
    kwargs.setdefault("ElectronCut", 0.05)
    kwargs.setdefault("PositronCut", 0.05)
    kwargs.setdefault("GammaCut",    0.05)
    return RegionCreator(name, **kwargs)

def getTRTPhysicsRegionTool(name='TRTPhysicsRegionTool', **kwargs):
    from G4AtlasApps.SimFlags import simFlags
    rangeCut = simFlags.TRTRangeCut.get_Value()
    kwargs.setdefault("RegionName", 'TRT')
    volumeList = ['TRT::Gas', 'TRT::GasMA']
    kwargs.setdefault("VolumeList",  volumeList)
    kwargs.setdefault("ElectronCut", rangeCut)
    kwargs.setdefault("PositronCut", rangeCut)
    # The photon range cut is meant to stay small
    kwargs.setdefault("GammaCut",    0.05)
    return RegionCreator(name, **kwargs)

def getTRT_ArPhysicsRegionTool(name='TRT_ArPhysicsRegionTool', **kwargs):
    kwargs.setdefault("RegionName", 'TRT_Ar')
    volumeList = ['TRT::Gas_Ar', 'TRT::GasMA_Ar']
    kwargs.setdefault("VolumeList",  volumeList)
    kwargs.setdefault("ElectronCut", 30.0)
    kwargs.setdefault("PositronCut", 30.0)
    kwargs.setdefault("GammaCut",    0.05)
    return RegionCreator(name, **kwargs)

def getTRT_KrPhysicsRegionTool(name='TRT_KrPhysicsRegionTool', **kwargs):
    kwargs.setdefault("RegionName", 'TRT_Kr')
    volumeList = ['TRT::Gas_Kr', 'TRT::GasMA_Kr']
    kwargs.setdefault("VolumeList",  volumeList)
    kwargs.setdefault("ElectronCut", 15.0)
    kwargs.setdefault("PositronCut", 15.0)
    kwargs.setdefault("GammaCut",    0.05)
    return RegionCreator(name, **kwargs)

# Calo Regions
def getEMBPhysicsRegionTool(name='EMBPhysicsRegionTool', **kwargs):
    kwargs.setdefault("RegionName", 'EMB')
    volumeList = ['LArMgr::LAr::EMB::STAC']
    kwargs.setdefault("VolumeList",  volumeList)
    rangeEMB = 0.03
    from G4AtlasApps.SimFlags import simFlags
    if not '_EMV' in simFlags.PhysicsList() and not '_EMX' in simFlags.PhysicsList():
        rangeEMB = 0.1
    kwargs.setdefault("ElectronCut", rangeEMB)
    kwargs.setdefault("PositronCut", rangeEMB)
    kwargs.setdefault("GammaCut",    rangeEMB)
    return RegionCreator(name, **kwargs)

def getEMECPhysicsRegionTool(name='EMECPhysicsRegionTool', **kwargs):
    kwargs.setdefault("RegionName", 'EMEC')
    volumeList = ['LArMgr::LAr::EMEC::Mother']
    kwargs.setdefault("VolumeList",  volumeList)
    rangeEMEC = 0.03
    from G4AtlasApps.SimFlags import simFlags
    if not '_EMV' in simFlags.PhysicsList() and not '_EMX' in simFlags.PhysicsList():
        rangeEMEC = 0.1
    kwargs.setdefault("ElectronCut", rangeEMEC)
    kwargs.setdefault("PositronCut", rangeEMEC)
    kwargs.setdefault("GammaCut",    rangeEMEC)
    return RegionCreator(name, **kwargs)

def getHECPhysicsRegionTool(name='HECPhysicsRegionTool', **kwargs):
    kwargs.setdefault("RegionName", 'HEC')
    volumeList = ['LArMgr::LAr::HEC::LiquidArgon']
    kwargs.setdefault("VolumeList",  volumeList)
    rangeHEC = 0.03
    from G4AtlasApps.SimFlags import simFlags
    if not '_EMV' in simFlags.PhysicsList() and not '_EMX' in simFlags.PhysicsList():
        rangeHEC = 1.0
    kwargs.setdefault("ElectronCut", rangeHEC)
    kwargs.setdefault("PositronCut", rangeHEC)
    kwargs.setdefault("GammaCut",    rangeHEC)
    return RegionCreator(name, **kwargs)

def getFCALPhysicsRegionTool(name='FCALPhysicsRegionTool', **kwargs):
    kwargs.setdefault("RegionName", 'FCAL')
    volumeList = ['LArMgr::LAr::FCAL::LiquidArgonC']
    kwargs.setdefault("VolumeList",  volumeList)
    kwargs.setdefault("ElectronCut", 0.03)
    kwargs.setdefault("PositronCut", 0.03)
    kwargs.setdefault("GammaCut",    0.03)
    return RegionCreator(name, **kwargs)

def getEMECParaPhysicsRegionTool(name='EMECParaPhysicsRegionTool', **kwargs):
    kwargs.setdefault("RegionName", 'EMECPara')
    volumeList = ['LArMgr::LAr::EMEC::Pos::InnerWheel', 'LArMgr::LAr::EMEC::Pos::OuterWheel',
                  'LArMgr::LAr::EMEC::Neg::InnerWheel', 'LArMgr::LAr::EMEC::Neg::OuterWheel']
    kwargs.setdefault("VolumeList",  volumeList)
    rangeEMEC = 0.03
    from G4AtlasApps.SimFlags import simFlags
    if not '_EMV' in simFlags.PhysicsList() and not '_EMX' in simFlags.PhysicsList():
        rangeEMEC = 0.1
    kwargs.setdefault("ElectronCut", rangeEMEC)
    kwargs.setdefault("PositronCut", rangeEMEC)
    kwargs.setdefault("GammaCut",    rangeEMEC)
    return RegionCreator(name, **kwargs)

def getFCALParaPhysicsRegionTool(name='FCALParaPhysicsRegionTool', **kwargs):
    kwargs.setdefault("RegionName", 'FCALPara')
    volumeList = ['LArMgr::LAr::FCAL::Module1::Absorber']
    kwargs.setdefault("VolumeList",  volumeList)
    kwargs.setdefault("ElectronCut", 0.03)
    kwargs.setdefault("PositronCut", 0.03)
    kwargs.setdefault("GammaCut",    0.03)
    return RegionCreator(name, **kwargs)

def getFCAL2ParaPhysicsRegionTool(name='FCAL2ParaPhysicsRegionTool', **kwargs):
    kwargs.setdefault("RegionName", 'FCAL2Para')
    volumeList = ['LArMgr::LAr::FCAL::Module2::Absorber', 'LArMgr::LAr::FCAL::Module3::Absorber']
    kwargs.setdefault("VolumeList",  volumeList)
    kwargs.setdefault("ElectronCut", 0.03)
    kwargs.setdefault("PositronCut", 0.03)
    kwargs.setdefault("GammaCut",    0.03)
    return RegionCreator(name, **kwargs)

def getPreSampLArPhysicsRegionTool(name='PreSampLArPhysicsRegionTool', **kwargs):
    kwargs.setdefault("RegionName", 'PreSampLAr')
    volumeList = ['LArMgr::LAr::Endcap::Presampler::LiquidArgon']
    kwargs.setdefault("VolumeList",  volumeList)
    return RegionCreator(name, **kwargs)

def getDeadMaterialPhysicsRegionTool(name='DeadMaterialPhysicsRegionTool', **kwargs):
    kwargs.setdefault("RegionName", 'DeadMaterial')
    volumeList = []
    sectionList = range(16,49) # does not include 49
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
def getDriftWallPhysicsRegionTool(name='DriftWallPhysicsRegionTool', **kwargs):
    kwargs.setdefault("RegionName", 'DriftWall')
    volumeList = ['Muon::MDTDriftWall']
    kwargs.setdefault("VolumeList",  volumeList)
    kwargs.setdefault("ElectronCut", 0.05)
    kwargs.setdefault("PositronCut", 0.05)
    return RegionCreator(name, **kwargs)

def getDriftWall1PhysicsRegionTool(name='DriftWall1PhysicsRegionTool', **kwargs):
    kwargs.setdefault("RegionName", 'DriftWall1')
    volumeList = ['Muon::Endplug']
    kwargs.setdefault("VolumeList",  volumeList)
    kwargs.setdefault("ElectronCut", 1.0)
    kwargs.setdefault("PositronCut", 1.0)
    return RegionCreator(name, **kwargs)

def getDriftWall2PhysicsRegionTool(name='DriftWall2PhysicsRegionTool', **kwargs):
    kwargs.setdefault("RegionName", 'DriftWall2')
    volumeList = ['Muon::SensitiveGas']
    kwargs.setdefault("VolumeList",  volumeList)
    kwargs.setdefault("ElectronCut", 1.0)
    kwargs.setdefault("PositronCut", 1.0)
    return RegionCreator(name, **kwargs)

def getMuonSystemFastPhysicsRegionTool(name='MuonSystemFastPhysicsRegionTool', **kwargs):
    kwargs.setdefault("RegionName", 'MuonSystemFastRegion')
    volumeList = []
    from G4AtlasApps.SimFlags import simFlags
    if 'World' in simFlags.CavernBG.get_Value():
        volumeList += ['BeamPipe::BeamPipe', 'IDET::IDET']
    volumeList = ['Muon::MuonSys']
    kwargs.setdefault("VolumeList",  volumeList)
    kwargs.setdefault("ElectronCut", 1.0)
    kwargs.setdefault("PositronCut", 1.0)
    return RegionCreator(name, **kwargs)

def getMuonPhysicsRegionTool(name="MuonPhysicsRegionTool", **kwargs):
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

def getBedrockPhysicsRegionTool(name='BedrockPhysicsRegionTool', **kwargs):
    kwargs.setdefault("RegionName", 'BEDROCK')
    volumeList = ['CavernInfra::BEDROCK']
    kwargs.setdefault("VolumeList",  volumeList)
    kwargs.setdefault("ElectronCut", 1000000.)
    kwargs.setdefault("PositronCut", 1000000.)
    kwargs.setdefault("GammaCut",    1000000.)
    return RegionCreator(name, **kwargs)

def getCavernShaftsConcretePhysicsRegionTool(name='CavernShaftsConcretePhysicsRegionTool', **kwargs):
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

def getSCTSiliconPhysicsRegionTool(name='SCTSiliconPhysicsRegionTool', **kwargs):
    kwargs.setdefault("RegionName", 'SCTSiliconRegion')
    volumeList = ['SCT::ECSensor0']
    kwargs.setdefault("VolumeList",  volumeList)
    kwargs.setdefault("ElectronCut", 0.01)
    return RegionCreator(name, **kwargs)





if __name__ == '__main__':
  from AthenaConfiguration.MainServicesConfig import MainServicesSerialCfg
  import os

  # Set up logging and config behaviour
  from AthenaCommon.Logging import log
  from AthenaCommon.Constants import DEBUG
  from AthenaCommon.Configurable import Configurable
  log.setLevel(DEBUG)
  Configurable.configurableRun3Behavior = 1


  #import config flags
  from AthenaConfiguration.AllConfigFlags import ConfigFlags
  
  from AthenaConfiguration.TestDefaults import defaultTestFiles
  inputDir = defaultTestFiles.d
  ConfigFlags.Input.Files = defaultTestFiles.EVNT

  # Finalize 
  ConfigFlags.lock()

  ## Initialize a new component accumulator
  cfg = MainServicesSerialCfg()

  #add the algorithm
  tool1 = SX1PhysicsRegionToolCfg(ConfigFlags)
  cfg.addPublicTool(tool1)
  #cfg.merge(acc1)

  #cfg.addEventAlgo(Alg) #Event algo?
  #cfg.merge(acc)
  #cfg.addPublicTool(Alg)

  # Dump config
  #cfg.getService("StoreGateSvc").Dump = True
  #cfg.getService("ConditionStore").Dump = True
  cfg.printConfig(withDetails=True, summariseProps = True)
  ConfigFlags.dump()


  f=open("test.pkl","w")
  cfg.store(f) 
  f.close()



  print cfg._publicTools
  print "-----------------finished----------------------"
