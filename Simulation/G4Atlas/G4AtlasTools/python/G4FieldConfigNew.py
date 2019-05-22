# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from G4AtlasTools.G4AtlasToolsConf import GlobalFieldManagerTool, DetectorFieldManagerTool

# Field Managers
def ATLASFieldManagerToolCfg(ConfigFlags, name='ATLASFieldManager', **kwargs):
    from G4AtlasApps.SimFlags import simFlags
    kwargs.setdefault("IntegratorStepper", simFlags.G4Stepper.get_Value())
    kwargs.setdefault("FieldSvc", "StandardField")
    kwargs.setdefault("UseTightMuonStepping", False)
    if simFlags.EquationOfMotion.statusOn:
        kwargs.setdefault("EquationOfMotion", simFlags.EquationOfMotion.get_Value() )
    return GlobalFieldManagerTool(name, **kwargs)

def TightMuonsATLASFieldManagerToolCfg(ConfigFlags, name='TightMuonsATLASFieldManager', **kwargs):
    from G4AtlasApps.SimFlags import simFlags
    kwargs.setdefault("IntegratorStepper", simFlags.G4Stepper.get_Value())
    kwargs.setdefault("FieldSvc", "StandardField")
    kwargs.setdefault("UseTightMuonStepping",True)
    if simFlags.EquationOfMotion.statusOn:
        kwargs.setdefault("EquationOfMotion", simFlags.EquationOfMotion.get_Value() )
    return GlobalFieldManagerTool(name, **kwargs)

def ClassicFieldManagerToolCfg(ConfigFlags, name='ClassicFieldManager', **kwargs):
    kwargs.setdefault("IntegratorStepper", "ClassicalRK4")
    return ATLASFieldManagerToolCfg(ConfigFlags, name, **kwargs)

def BasicDetectorFieldManagerToolCfg(ConfigFlags, name='BasicDetectorFieldManager', **kwargs):
    from G4AtlasApps.SimFlags import simFlags
    kwargs.setdefault("IntegratorStepper", simFlags.G4Stepper.get_Value())
    kwargs.setdefault("FieldSvc",          "StandardField")
    kwargs.setdefault('MuonOnlyField',     False)
    if simFlags.EquationOfMotion.statusOn:
        kwargs.setdefault("EquationOfMotion", simFlags.EquationOfMotion.get_Value() )
    return DetectorFieldManagerTool(name, **kwargs)

def BeamPipeFieldManagerToolCfg(ConfigFlags, name='BeamPipeFieldManager', **kwargs):
    kwargs.setdefault("LogicalVolumes", ['BeamPipe::BeamPipe'])
    #kwargs.setdefault('DeltaChord',         0.00001)
    kwargs.setdefault('DeltaIntersection',  0.00001)
    kwargs.setdefault('DeltaOneStep',       0.0001)
    kwargs.setdefault('MaximumEpsilonStep', 0.001)
    kwargs.setdefault('MinimumEpsilonStep', 0.00001)
    return BasicDetectorFieldManagerToolCfg(ConfigFlags, name, **kwargs)

def InDetFieldManagerToolCfg(ConfigFlags, name='InDetFieldManager', **kwargs):
    kwargs.setdefault("LogicalVolumes", ['IDET::IDET'])
    #kwargs.setdefault('DeltaChord',         0.00001)
    kwargs.setdefault('DeltaIntersection',  0.00001)
    kwargs.setdefault('DeltaOneStep',       0.0001)
    kwargs.setdefault('MaximumEpsilonStep', 0.001)
    kwargs.setdefault('MinimumEpsilonStep', 0.00001)
    return BasicDetectorFieldManagerToolCfg(ConfigFlags, name, **kwargs)

def MuonsOnlyInCaloFieldManagerToolCfg(ConfigFlags, name='MuonsOnlyInCaloFieldManager', **kwargs):
    kwargs.setdefault("LogicalVolumes", ['CALO::CALO'])
    #kwargs.setdefault('DeltaChord',         0.00000002)
    kwargs.setdefault('DeltaIntersection',  0.00000002)
    kwargs.setdefault('DeltaOneStep',       0.000001)
    kwargs.setdefault('MaximumEpsilonStep', 0.0000009)
    kwargs.setdefault('MinimumEpsilonStep', 0.000001)
    kwargs.setdefault('MuonOnlyField',      True)
    return BasicDetectorFieldManagerToolCfg(ConfigFlags, name, **kwargs)

def MuonFieldManagerToolCfg(ConfigFlags, name='MuonFieldManager', **kwargs):
    kwargs.setdefault("LogicalVolumes", ['MUONQ02::MUONQ02'])
    #kwargs.setdefault('DeltaChord',         0.00000002)
    kwargs.setdefault('DeltaIntersection',  0.00000002)
    kwargs.setdefault('DeltaOneStep',       0.000001)
    kwargs.setdefault('MaximumEpsilonStep', 0.0000009)
    kwargs.setdefault('MinimumEpsilonStep', 0.000001)
    return BasicDetectorFieldManagerToolCfg(ConfigFlags, name, **kwargs)


def BasicFwdFieldMangerToolCfg(ConfigFlags, name='FwdFieldManagerTool', **kwargs):
    #kwargs.setdefault('DeltaChord',         0.00000002)
    kwargs.setdefault('DeltaIntersection',  1e-9)
    kwargs.setdefault('DeltaOneStep',       1e-8)
    kwargs.setdefault('MaximumEpsilonStep', 1e-8)
    kwargs.setdefault('MinimumEpsilonStep', 1e-9)
    kwargs.setdefault("FieldSvc",           "ForwardField")
    from G4AtlasApps.SimFlags import simFlags
    if simFlags.FwdStepLimitation.statusOn:
        kwargs.setdefault("MaximumStep", simFlags.FwdStepLimitation())
    return BasicDetectorFieldManagerToolCfg(ConfigFlags, name, **kwargs)

def Q1FwdFieldMangerToolCfg(ConfigFlags, name='Q1FwdFieldManager', **kwargs):
    kwargs.setdefault("FieldSvc",           "Q1FwdG4FieldSvc")
    kwargs.setdefault("LogicalVolumes", ['FwdRegion::LQXAA.1R1MagQ1'])
    return BasicFwdFieldMangerToolCfg(ConfigFlags, name, **kwargs)
def Q2FwdFieldMangerToolCfg(ConfigFlags, name='Q2FwdFieldManager', **kwargs):
    kwargs.setdefault("FieldSvc",           "Q2FwdG4FieldSvc")
    kwargs.setdefault("LogicalVolumes", ['FwdRegion::LQXBA.2R1MagQ2a', 'FwdRegion::LQXBA.2R1MagQ2b'])
    return BasicFwdFieldMangerToolCfg(ConfigFlags, name, **kwargs)
def Q3FwdFieldMangerToolCfg(ConfigFlags, name='Q3FwdFieldManager', **kwargs):
    kwargs.setdefault("FieldSvc",           "Q3FwdG4FieldSvc")
    kwargs.setdefault("LogicalVolumes", ['FwdRegion::LQXAG.3R1MagQ3'])
    return BasicFwdFieldMangerToolCfg(ConfigFlags, name, **kwargs)
def D1FwdFieldMangerToolCfg(ConfigFlags, name='D1FwdFieldManager', **kwargs):
    kwargs.setdefault("FieldSvc",           "D1FwdG4FieldSvc")
    kwargs.setdefault("LogicalVolumes", ['FwdRegion::MBXW.A4R1MagD1a', 'FwdRegion::MBXW.B4R1MagD1b',
                                         'FwdRegion::MBXW.C4R1MagD1c', 'FwdRegion::MBXW.D4R1MagD1d',
                                         'FwdRegion::MBXW.E4R1MagD1e', 'FwdRegion::MBXW.F4R1MagD1f'])
    return BasicFwdFieldMangerToolCfg(ConfigFlags, name, **kwargs)
def D2FwdFieldMangerToolCfg(ConfigFlags, name='D2FwdFieldManager', **kwargs):
    kwargs.setdefault("FieldSvc",           "D2FwdG4FieldSvc")
    kwargs.setdefault("LogicalVolumes", ['FwdRegion::LBRCD.4R1MagD2'])
    return BasicFwdFieldMangerToolCfg(ConfigFlags, name, **kwargs)
def Q4FwdFieldMangerToolCfg(ConfigFlags, name='Q4FwdFieldManager', **kwargs):
    kwargs.setdefault("FieldSvc",           "Q4FwdG4FieldSvc")
    kwargs.setdefault("LogicalVolumes", ['FwdRegion::LQYCH.4R1MagQ4'])
    return BasicFwdFieldMangerToolCfg(ConfigFlags, name, **kwargs)
def Q5FwdFieldMangerToolCfg(ConfigFlags, name='Q5FwdFieldManager', **kwargs):
    kwargs.setdefault("FieldSvc",           "Q5FwdG4FieldSvc")
    kwargs.setdefault("LogicalVolumes", ['FwdRegion::LQNDC.5R1MagQ5'])
    return BasicFwdFieldMangerToolCfg(ConfigFlags, name, **kwargs)
def Q6FwdFieldMangerToolCfg(ConfigFlags, name='Q6FwdFieldManager', **kwargs):
    kwargs.setdefault("FieldSvc",           "Q6FwdG4FieldSvc")
    kwargs.setdefault("LogicalVolumes", ['FwdRegion::LQNDD.6R1MagQ6'])
    return BasicFwdFieldMangerToolCfg(ConfigFlags, name, **kwargs)
def Q7FwdFieldMangerToolCfg(ConfigFlags, name='Q7FwdFieldManager', **kwargs):
    kwargs.setdefault("FieldSvc",           "Q7FwdG4FieldSvc")
    kwargs.setdefault("LogicalVolumes", ['FwdRegion::LQNFD.7R1MagQ7a', 'FwdRegion::LQNFD.7R1MagQ7b'])
    return BasicFwdFieldMangerToolCfg(ConfigFlags, name, **kwargs)
def Q1HKickFwdFieldMangerToolCfg(ConfigFlags, name='Q1HKickFwdFieldManager', **kwargs):
    kwargs.setdefault("FieldSvc",           "Q1HKickFwdG4FieldSvc")
    kwargs.setdefault("LogicalVolumes", ['FwdRegion::LQXAA.1R1MagQ1HKick'])
    return BasicFwdFieldMangerToolCfg(ConfigFlags, name, **kwargs)
def Q1VKickFwdFieldMangerToolCfg(ConfigFlags, name='Q1VKickFwdFieldManager', **kwargs):
    kwargs.setdefault("FieldSvc",           "Q1VKickFwdG4FieldSvc")
    kwargs.setdefault("LogicalVolumes", ['FwdRegion::LQXAA.1R1MagQ1VKick'])
    return BasicFwdFieldMangerToolCfg(ConfigFlags, name, **kwargs)
def Q2HKickFwdFieldMangerToolCfg(ConfigFlags, name='Q2HKickFwdFieldManager', **kwargs):
    kwargs.setdefault("FieldSvc",           "Q2HKickFwdG4FieldSvc")
    kwargs.setdefault("LogicalVolumes", ['FwdRegion::LQXBA.2R1MagQ2HKick'])
    return BasicFwdFieldMangerToolCfg(ConfigFlags, name, **kwargs)
def Q2VKickFwdFieldMangerToolCfg(ConfigFlags, name='Q2VKickFwdFieldManager', **kwargs):
    kwargs.setdefault("FieldSvc",           "Q2VKickFwdG4FieldSvc")
    kwargs.setdefault("LogicalVolumes", ['FwdRegion::LQXBA.2R1MagQ2VKick'])
    return BasicFwdFieldMangerToolCfg(ConfigFlags, name, **kwargs)
def Q3HKickFwdFieldMangerToolCfg(ConfigFlags, name='Q3HKickFwdFieldManager', **kwargs):
    kwargs.setdefault("FieldSvc",           "Q3HKickFwdG4FieldSvc")
    kwargs.setdefault("LogicalVolumes", ['FwdRegion::LQXAG.3R1MagQ3HKick'])
    return BasicFwdFieldMangerToolCfg(ConfigFlags, name, **kwargs)
def Q3VKickFwdFieldMangerToolCfg(ConfigFlags, name='Q3VKickFwdFieldManager', **kwargs):
    kwargs.setdefault("FieldSvc",           "Q3VKickFwdG4FieldSvc")
    kwargs.setdefault("LogicalVolumes", ['FwdRegion::LQXAG.3R1MagQ3VKick'])
    return BasicFwdFieldMangerToolCfg(ConfigFlags, name, **kwargs)
def Q4VKickAFwdFieldMangerToolCfg(ConfigFlags, name='Q4VKickAFwdFieldManager', **kwargs):
    kwargs.setdefault("FieldSvc",           "Q4VKickAFwdG4FieldSvc")
    kwargs.setdefault("LogicalVolumes", ['FwdRegion::LQYCH.4R1MagQ4VKickA'])
    return BasicFwdFieldMangerToolCfg(ConfigFlags, name, **kwargs)
def Q4HKickFwdFieldMangerToolCfg(ConfigFlags, name='Q4HKickFwdFieldManager', **kwargs):
    kwargs.setdefault("FieldSvc",           "Q4HKickFwdG4FieldSvc")
    kwargs.setdefault("LogicalVolumes", ['FwdRegion::LQYCH.4R1MagQ4HKick'])
    return BasicFwdFieldMangerToolCfg(ConfigFlags, name, **kwargs)
def Q4VKickBFwdFieldMangerToolCfg(ConfigFlags, name='Q4VKickBFwdFieldManager', **kwargs):
    kwargs.setdefault("FieldSvc",           "Q4VKickBFwdG4FieldSvc")
    kwargs.setdefault("LogicalVolumes", ['FwdRegion::LQYCH.4R1MagQ4VKickB'])
    return BasicFwdFieldMangerToolCfg(ConfigFlags, name, **kwargs)
def Q5HKickFwdFieldMangerToolCfg(ConfigFlags, name='Q5HKickFwdFieldManager', **kwargs):
    kwargs.setdefault("FieldSvc",           "Q5HKickFwdG4FieldSvc")
    kwargs.setdefault("LogicalVolumes", ['FwdRegion::LQNDC.5R1MagQ5HKick'])
    return BasicFwdFieldMangerToolCfg(ConfigFlags, name, **kwargs)
def Q6VKickFwdFieldMangerToolCfg(ConfigFlags, name='Q6VKickFwdFieldManager', **kwargs):
    kwargs.setdefault("FieldSvc",           "Q6VKickFwdG4FieldSvc")
    kwargs.setdefault("LogicalVolumes", ['FwdRegion::LQNDD.6R1MagQ6VKick'])
    return BasicFwdFieldMangerToolCfg(ConfigFlags, name, **kwargs)

def FwdRegionFieldMangerToolCfg(ConfigFlags, name='FwdRegionFieldManager', **kwargs):
    kwargs.setdefault("LogicalVolumes", ['FwdRegion::ForwardRegionGeoModel'])
    from G4AtlasApps.SimFlags import simFlags
    if simFlags.FwdStepLimitation.statusOn:
        kwargs.setdefault("MaximumStep", simFlags.FwdStepLimitation())
    return BasicDetectorFieldManagerToolCfg(ConfigFlags, name, **kwargs)



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
  cfg.addPublicTool(ATLASFieldManagerToolCfg(ConfigFlags))
  cfg.addPublicTool(TightMuonsATLASFieldManagerToolCfg  (ConfigFlags))
  cfg.addPublicTool(ClassicFieldManagerToolCfg(ConfigFlags))
  cfg.addPublicTool(BasicDetectorFieldManagerToolCfg(ConfigFlags))
  #cfg.addPublicTool(Q1FwdFieldMangerToolCfg(ConfigFlags))


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

