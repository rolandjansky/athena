# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

# ComponentAccumulator based configuration
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
GlobalFieldManagerTool, DetectorFieldManagerTool=CompFactory.getComps("GlobalFieldManagerTool","DetectorFieldManagerTool",)
from G4AtlasServices.G4AtlasFieldServices import StandardFieldSvcCfg, Q1FwdG4FieldSvcCfg, Q2FwdG4FieldSvcCfg, Q3FwdG4FieldSvcCfg, D1FwdG4FieldSvcCfg, D2FwdG4FieldSvcCfg, Q4FwdG4FieldSvcCfg, Q5FwdG4FieldSvcCfg, Q6FwdG4FieldSvcCfg, Q7FwdG4FieldSvcCfg, Q1HKickFwdG4FieldSvcCfg, Q1VKickFwdG4FieldSvcCfg, Q2HKickFwdG4FieldSvcCfg, Q2VKickFwdG4FieldSvcCfg, Q3HKickFwdG4FieldSvcCfg, Q3VKickFwdG4FieldSvcCfg, Q4VKickAFwdG4FieldSvcCfg, Q4HKickFwdG4FieldSvcCfg, Q4VKickBFwdG4FieldSvcCfg, Q5HKickFwdG4FieldSvcCfg, Q6VKickFwdG4FieldSvcCfg

# Field Managers
def ATLASFieldManagerToolCfg(ConfigFlags, name='ATLASFieldManager', **kwargs):
    result = ComponentAccumulator()
    kwargs.setdefault("IntegratorStepper", ConfigFlags.Sim.G4Stepper)
    kwargs.setdefault("FieldSvc", result.getPrimaryAndMerge(StandardFieldSvcCfg(ConfigFlags)).name)
    kwargs.setdefault("UseTightMuonStepping", False)
    if len(ConfigFlags.Sim.G4EquationOfMotion)>0:
        kwargs.setdefault("EquationOfMotion", ConfigFlags.Sim.G4EquationOfMotion )
    result.setPrivateTools(GlobalFieldManagerTool(name, **kwargs))
    return result

def TightMuonsATLASFieldManagerToolCfg(ConfigFlags, name='TightMuonsATLASFieldManager', **kwargs):
    result = ComponentAccumulator()
    kwargs.setdefault("IntegratorStepper", ConfigFlags.Sim.G4Stepper)
    kwargs.setdefault("FieldSvc", result.getPrimaryAndMerge(StandardFieldSvcCfg(ConfigFlags)).name)
    kwargs.setdefault("UseTightMuonStepping",True)
    if len(ConfigFlags.Sim.G4EquationOfMotion)>0:
        kwargs.setdefault("EquationOfMotion", ConfigFlags.Sim.G4EquationOfMotion )
    result.setPrivateTools(GlobalFieldManagerTool(name, **kwargs))
    return result

#not used in G4AtlasServicesConfigNew?
def ClassicFieldManagerToolCfg(ConfigFlags, name='ClassicFieldManager', **kwargs):
    kwargs.setdefault("IntegratorStepper", "ClassicalRK4")
    return ATLASFieldManagerToolCfg(ConfigFlags, name, **kwargs)

def BasicDetectorFieldManagerToolCfg(ConfigFlags, name='BasicDetectorFieldManager', **kwargs):
    result = ComponentAccumulator()
    if 'FieldSvc' not in kwargs: # don't create the StandardFieldSvc if it is not required by this tool.
        kwargs.setdefault("FieldSvc", result.getPrimaryAndMerge(StandardFieldSvcCfg(ConfigFlags)).name)
    kwargs.setdefault("IntegratorStepper", ConfigFlags.Sim.G4Stepper)
    kwargs.setdefault('MuonOnlyField',     False)
    if len(ConfigFlags.Sim.G4EquationOfMotion)>0:
        kwargs.setdefault("EquationOfMotion", ConfigFlags.Sim.G4EquationOfMotion )
    result.setPrivateTools(DetectorFieldManagerTool(name, **kwargs))
    return result

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

def ITkFieldManagerToolCfg(ConfigFlags, name='ITkFieldManager', **kwargs):
    kwargs.setdefault("LogicalVolumes", ['ITK::ITK'])
    #kwargs.setdefault('DeltaChord',         0.00001)
    kwargs.setdefault('DeltaIntersection',  0.00001)
    kwargs.setdefault('DeltaOneStep',       0.0001)
    kwargs.setdefault('MaximumEpsilonStep', 0.001)
    kwargs.setdefault('MinimumEpsilonStep', 0.00001)
    return BasicDetectorFieldManagerToolCfg(ConfigFlags, name, **kwargs)

def MuonsOnlyInCaloFieldManagerToolCfg(ConfigFlags, name='MuonsOnlyInCaloFieldManager', **kwargs):
    kwargs.setdefault("PhysicalVolumes", ['LArBarrel'])
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

#not used in G4AtlasServicesConfigNew?
def BasicFwdFieldManagerToolCfg(ConfigFlags, name='FwdFieldManagerTool', **kwargs):
    #kwargs.setdefault('DeltaChord',         0.00000002)
    kwargs.setdefault('DeltaIntersection',  1e-9)
    kwargs.setdefault('DeltaOneStep',       1e-8)
    kwargs.setdefault('MaximumEpsilonStep', 1e-8)
    kwargs.setdefault('MinimumEpsilonStep', 1e-9)
    #from G4AtlasApps.SimFlags import simFlags
    #if simFlags.FwdStepLimitation.statusOn:
    #    kwargs.setdefault("MaximumStep", simFlags.FwdStepLimitation())
    if False:
        kwargs.setdefault("MaximumStep", 1000.)
    return BasicDetectorFieldManagerToolCfg(ConfigFlags, name, **kwargs)

def Q1FwdFieldManagerToolCfg(ConfigFlags, name='Q1FwdFieldManager', **kwargs):
    result = ComponentAccumulator()
    kwargs.setdefault("FieldSvc", result.getPrimaryAndMerge(Q1FwdG4FieldSvcCfg(ConfigFlags)).name)
    kwargs.setdefault("LogicalVolumes", ['FwdRegion::LQXAA.1R1MagQ1'])
    acc=BasicFwdFieldManagerToolCfg(ConfigFlags, name, **kwargs)
    tool = result.popToolsAndMerge(acc)
    result.setPrivateTools(tool)
    return result
def Q2FwdFieldManagerToolCfg(ConfigFlags, name='Q2FwdFieldManager', **kwargs):
    result = ComponentAccumulator()
    kwargs.setdefault("FieldSvc", result.getPrimaryAndMerge(Q2FwdG4FieldSvcCfg(ConfigFlags)).name)
    kwargs.setdefault("LogicalVolumes", ['FwdRegion::LQXBA.2R1MagQ2a', 'FwdRegion::LQXBA.2R1MagQ2b'])
    acc=BasicFwdFieldManagerToolCfg(ConfigFlags, name, **kwargs)
    tool = result.popToolsAndMerge(acc)
    result.setPrivateTools(tool)
    return result
def Q3FwdFieldManagerToolCfg(ConfigFlags, name='Q3FwdFieldManager', **kwargs):
    result = ComponentAccumulator()
    kwargs.setdefault("FieldSvc", result.getPrimaryAndMerge(Q3FwdG4FieldSvcCfg(ConfigFlags)).name)
    kwargs.setdefault("LogicalVolumes", ['FwdRegion::LQXAG.3R1MagQ3'])
    acc=BasicFwdFieldManagerToolCfg(ConfigFlags, name, **kwargs)
    tool = result.popToolsAndMerge(acc)
    result.setPrivateTools(tool)
    return result
def D1FwdFieldManagerToolCfg(ConfigFlags, name='D1FwdFieldManager', **kwargs):
    result = ComponentAccumulator()
    kwargs.setdefault("FieldSvc", result.getPrimaryAndMerge(D1FwdG4FieldSvcCfg(ConfigFlags)).name)
    kwargs.setdefault("LogicalVolumes", ['FwdRegion::MBXW.A4R1MagD1a', 'FwdRegion::MBXW.B4R1MagD1b',
                                         'FwdRegion::MBXW.C4R1MagD1c', 'FwdRegion::MBXW.D4R1MagD1d',
                                         'FwdRegion::MBXW.E4R1MagD1e', 'FwdRegion::MBXW.F4R1MagD1f'])
    acc=BasicFwdFieldManagerToolCfg(ConfigFlags, name, **kwargs)
    tool = result.popToolsAndMerge(acc)
    result.setPrivateTools(tool)
    return result
def D2FwdFieldManagerToolCfg(ConfigFlags, name='D2FwdFieldManager', **kwargs):
    result = ComponentAccumulator()
    kwargs.setdefault("FieldSvc", result.getPrimaryAndMerge(D2FwdG4FieldSvcCfg(ConfigFlags)).name)
    kwargs.setdefault("LogicalVolumes", ['FwdRegion::LBRCD.4R1MagD2'])
    acc=BasicFwdFieldManagerToolCfg(ConfigFlags, name, **kwargs)
    tool = result.popToolsAndMerge(acc)
    result.setPrivateTools(tool)
    return result
def Q4FwdFieldManagerToolCfg(ConfigFlags, name='Q4FwdFieldManager', **kwargs):
    result = ComponentAccumulator()
    kwargs.setdefault("FieldSvc", result.getPrimaryAndMerge(Q4FwdG4FieldSvcCfg(ConfigFlags)).name)
    kwargs.setdefault("LogicalVolumes", ['FwdRegion::LQYCH.4R1MagQ4'])
    acc=BasicFwdFieldManagerToolCfg(ConfigFlags, name, **kwargs)
    tool = result.popToolsAndMerge(acc)
    result.setPrivateTools(tool)
    return result
def Q5FwdFieldManagerToolCfg(ConfigFlags, name='Q5FwdFieldManager', **kwargs):
    result = ComponentAccumulator()
    kwargs.setdefault("FieldSvc", result.getPrimaryAndMerge(Q5FwdG4FieldSvcCfg(ConfigFlags)).name)
    kwargs.setdefault("LogicalVolumes", ['FwdRegion::LQNDC.5R1MagQ5'])
    acc=BasicFwdFieldManagerToolCfg(ConfigFlags, name, **kwargs)
    tool = result.popToolsAndMerge(acc)
    result.setPrivateTools(tool)
    return result
def Q6FwdFieldManagerToolCfg(ConfigFlags, name='Q6FwdFieldManager', **kwargs):
    result = ComponentAccumulator()
    kwargs.setdefault("FieldSvc", result.getPrimaryAndMerge(Q6FwdG4FieldSvcCfg(ConfigFlags)).name)
    kwargs.setdefault("LogicalVolumes", ['FwdRegion::LQNDD.6R1MagQ6'])
    acc=BasicFwdFieldManagerToolCfg(ConfigFlags, name, **kwargs)
    tool = result.popToolsAndMerge(acc)
    result.setPrivateTools(tool)
    return result
def Q7FwdFieldManagerToolCfg(ConfigFlags, name='Q7FwdFieldManager', **kwargs):
    result = ComponentAccumulator()
    kwargs.setdefault("FieldSvc", result.getPrimaryAndMerge(Q7FwdG4FieldSvcCfg(ConfigFlags)).name)
    kwargs.setdefault("LogicalVolumes", ['FwdRegion::LQNFD.7R1MagQ7a', 'FwdRegion::LQNFD.7R1MagQ7b'])
    acc=BasicFwdFieldManagerToolCfg(ConfigFlags, name, **kwargs)
    tool = result.popToolsAndMerge(acc)
    result.setPrivateTools(tool)
    return result
def Q1HKickFwdFieldManagerToolCfg(ConfigFlags, name='Q1HKickFwdFieldManager', **kwargs):
    result = ComponentAccumulator()
    kwargs.setdefault("FieldSvc", result.getPrimaryAndMerge(Q1HKickFwdG4FieldSvcCfg(ConfigFlags)).name)
    kwargs.setdefault("LogicalVolumes", ['FwdRegion::LQXAA.1R1MagQ1HKick'])
    acc=BasicFwdFieldManagerToolCfg(ConfigFlags, name, **kwargs)
    tool = result.popToolsAndMerge(acc)
    result.setPrivateTools(tool)
    return result
def Q1VKickFwdFieldManagerToolCfg(ConfigFlags, name='Q1VKickFwdFieldManager', **kwargs):
    result = ComponentAccumulator()
    kwargs.setdefault("FieldSvc", result.getPrimaryAndMerge(Q1VKickFwdG4FieldSvcCfg(ConfigFlags)).name)
    kwargs.setdefault("LogicalVolumes", ['FwdRegion::LQXAA.1R1MagQ1VKick'])
    acc=BasicFwdFieldManagerToolCfg(ConfigFlags, name, **kwargs)
    tool = result.popToolsAndMerge(acc)
    result.setPrivateTools(tool)
    return result
def Q2HKickFwdFieldManagerToolCfg(ConfigFlags, name='Q2HKickFwdFieldManager', **kwargs):
    result = ComponentAccumulator()
    kwargs.setdefault("FieldSvc", result.getPrimaryAndMerge(Q2HKickFwdG4FieldSvcCfg(ConfigFlags)).name)
    kwargs.setdefault("LogicalVolumes", ['FwdRegion::LQXBA.2R1MagQ2HKick'])
    acc=BasicFwdFieldManagerToolCfg(ConfigFlags, name, **kwargs)
    tool = result.popToolsAndMerge(acc)
    result.setPrivateTools(tool)
    return result
def Q2VKickFwdFieldManagerToolCfg(ConfigFlags, name='Q2VKickFwdFieldManager', **kwargs):
    result = ComponentAccumulator()
    kwargs.setdefault("FieldSvc", result.getPrimaryAndMerge(Q2VKickFwdG4FieldSvcCfg(ConfigFlags)).name)
    kwargs.setdefault("LogicalVolumes", ['FwdRegion::LQXBA.2R1MagQ2VKick'])
    acc=BasicFwdFieldManagerToolCfg(ConfigFlags, name, **kwargs)
    tool = result.popToolsAndMerge(acc)
    result.setPrivateTools(tool)
    return result
def Q3HKickFwdFieldManagerToolCfg(ConfigFlags, name='Q3HKickFwdFieldManager', **kwargs):
    result = ComponentAccumulator()
    kwargs.setdefault("FieldSvc", result.getPrimaryAndMerge(Q3HKickFwdG4FieldSvcCfg(ConfigFlags)).name)
    kwargs.setdefault("LogicalVolumes", ['FwdRegion::LQXAG.3R1MagQ3HKick'])
    acc=BasicFwdFieldManagerToolCfg(ConfigFlags, name, **kwargs)
    tool = result.popToolsAndMerge(acc)
    result.setPrivateTools(tool)
    return result
def Q3VKickFwdFieldManagerToolCfg(ConfigFlags, name='Q3VKickFwdFieldManager', **kwargs):
    result = ComponentAccumulator()
    kwargs.setdefault("FieldSvc", result.getPrimaryAndMerge(Q3VKickFwdG4FieldSvcCfg(ConfigFlags)).name)
    kwargs.setdefault("LogicalVolumes", ['FwdRegion::LQXAG.3R1MagQ3VKick'])
    acc=BasicFwdFieldManagerToolCfg(ConfigFlags, name, **kwargs)
    tool = result.popToolsAndMerge(acc)
    result.setPrivateTools(tool)
    return result
def Q4VKickAFwdFieldManagerToolCfg(ConfigFlags, name='Q4VKickAFwdFieldManager', **kwargs):
    result = ComponentAccumulator()
    kwargs.setdefault("FieldSvc", result.getPrimaryAndMerge(Q4VKickAFwdG4FieldSvcCfg(ConfigFlags)).name)
    kwargs.setdefault("LogicalVolumes", ['FwdRegion::LQYCH.4R1MagQ4VKickA'])
    acc=BasicFwdFieldManagerToolCfg(ConfigFlags, name, **kwargs)
    tool = result.popToolsAndMerge(acc)
    result.setPrivateTools(tool)
    return result
def Q4HKickFwdFieldManagerToolCfg(ConfigFlags, name='Q4HKickFwdFieldManager', **kwargs):
    result = ComponentAccumulator()
    kwargs.setdefault("FieldSvc", result.getPrimaryAndMerge(Q4HKickFwdG4FieldSvcCfg(ConfigFlags)).name)
    kwargs.setdefault("LogicalVolumes", ['FwdRegion::LQYCH.4R1MagQ4HKick'])
    acc=BasicFwdFieldManagerToolCfg(ConfigFlags, name, **kwargs)
    tool = result.popToolsAndMerge(acc)
    result.setPrivateTools(tool)
    return result
def Q4VKickBFwdFieldManagerToolCfg(ConfigFlags, name='Q4VKickBFwdFieldManager', **kwargs):
    result = ComponentAccumulator()
    kwargs.setdefault("FieldSvc", result.getPrimaryAndMerge(Q4VKickBFwdG4FieldSvcCfg(ConfigFlags)).name)
    kwargs.setdefault("LogicalVolumes", ['FwdRegion::LQYCH.4R1MagQ4VKickB'])
    acc=BasicFwdFieldManagerToolCfg(ConfigFlags, name, **kwargs)
    tool = result.popToolsAndMerge(acc)
    result.setPrivateTools(tool)
    return result
def Q5HKickFwdFieldManagerToolCfg(ConfigFlags, name='Q5HKickFwdFieldManager', **kwargs):
    result = ComponentAccumulator()
    kwargs.setdefault("FieldSvc", result.getPrimaryAndMerge(Q5HKickFwdG4FieldSvcCfg(ConfigFlags)).name)
    kwargs.setdefault("LogicalVolumes", ['FwdRegion::LQNDC.5R1MagQ5HKick'])
    acc=BasicFwdFieldManagerToolCfg(ConfigFlags, name, **kwargs)
    tool = result.popToolsAndMerge(acc)
    result.setPrivateTools(tool)
    return result
def Q6VKickFwdFieldManagerToolCfg(ConfigFlags, name='Q6VKickFwdFieldManager', **kwargs):
    result = ComponentAccumulator()
    kwargs.setdefault("FieldSvc", result.getPrimaryAndMerge(Q6VKickFwdG4FieldSvcCfg(ConfigFlags)).name)
    kwargs.setdefault("LogicalVolumes", ['FwdRegion::LQNDD.6R1MagQ6VKick'])
    acc=BasicFwdFieldManagerToolCfg(ConfigFlags, name, **kwargs)
    tool = result.popToolsAndMerge(acc)
    result.setPrivateTools(tool)
    return result

def FwdRegionFieldManagerToolCfg(ConfigFlags, name='FwdRegionFieldManager', **kwargs):
    kwargs.setdefault("LogicalVolumes", ['FwdRegion::ForwardRegionGeoModel'])
    #from G4AtlasApps.SimFlags import simFlags
    #if simFlags.FwdStepLimitation.statusOn:
    #    kwargs.setdefault("MaximumStep", simFlags.FwdStepLimitation())
    if False:
        kwargs.setdefault("MaximumStep", 1000.)
    return BasicDetectorFieldManagerToolCfg(ConfigFlags, name, **kwargs)
