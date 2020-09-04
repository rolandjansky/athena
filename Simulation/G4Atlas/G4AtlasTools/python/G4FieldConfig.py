# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from AthenaCommon import CfgMgr

# Field Managers
def getATLASFieldManagerTool(name='ATLASFieldManager', **kwargs):
    from G4AtlasApps.SimFlags import simFlags
    kwargs.setdefault("IntegratorStepper", simFlags.G4Stepper.get_Value())
    kwargs.setdefault("FieldSvc", "StandardField")
    kwargs.setdefault("UseTightMuonStepping", False)
    if simFlags.EquationOfMotion.statusOn:
        kwargs.setdefault("EquationOfMotion", simFlags.EquationOfMotion.get_Value() )
    return CfgMgr.GlobalFieldManagerTool(name, **kwargs)

def getTightMuonsATLASFieldManagerTool(name='TightMuonsATLASFieldManager', **kwargs):
    from G4AtlasApps.SimFlags import simFlags
    kwargs.setdefault("IntegratorStepper", simFlags.G4Stepper.get_Value())
    kwargs.setdefault("FieldSvc", "StandardField")
    kwargs.setdefault("UseTightMuonStepping",True)
    if simFlags.EquationOfMotion.statusOn:
        kwargs.setdefault("EquationOfMotion", simFlags.EquationOfMotion.get_Value() )
    return CfgMgr.GlobalFieldManagerTool(name, **kwargs)

def getClassicFieldManagerTool(name='ClassicFieldManager', **kwargs):
    kwargs.setdefault("IntegratorStepper", "ClassicalRK4")
    return getATLASFieldManagerTool(name, **kwargs)

def getBasicDetectorFieldManagerTool(name='BasicDetectorFieldManager', **kwargs):
    from G4AtlasApps.SimFlags import simFlags
    kwargs.setdefault("IntegratorStepper", simFlags.G4Stepper.get_Value())
    kwargs.setdefault("FieldSvc",          "StandardField")
    kwargs.setdefault('MuonOnlyField',     False)
    if simFlags.EquationOfMotion.statusOn:
        kwargs.setdefault("EquationOfMotion", simFlags.EquationOfMotion.get_Value() )
    return CfgMgr.DetectorFieldManagerTool(name, **kwargs)

def getBeamPipeFieldManagerTool(name='BeamPipeFieldManager', **kwargs):
    kwargs.setdefault("LogicalVolumes", ['BeamPipe::BeamPipe'])
    #kwargs.setdefault('DeltaChord',         0.00001)
    kwargs.setdefault('DeltaIntersection',  0.00001)
    kwargs.setdefault('DeltaOneStep',       0.0001)
    kwargs.setdefault('MaximumEpsilonStep', 0.001)
    kwargs.setdefault('MinimumEpsilonStep', 0.00001)
    return getBasicDetectorFieldManagerTool(name, **kwargs)

def getInDetFieldManagerTool(name='InDetFieldManager', **kwargs):
    kwargs.setdefault("LogicalVolumes", ['IDET::IDET'])
    #kwargs.setdefault('DeltaChord',         0.00001)
    kwargs.setdefault('DeltaIntersection',  0.00001)
    kwargs.setdefault('DeltaOneStep',       0.0001)
    kwargs.setdefault('MaximumEpsilonStep', 0.001)
    kwargs.setdefault('MinimumEpsilonStep', 0.00001)
    return getBasicDetectorFieldManagerTool(name, **kwargs)

def getMuonsOnlyInCaloFieldManagerTool(name='MuonsOnlyInCaloFieldManager', **kwargs):
    kwargs.setdefault("LogicalVolumes", ['CALO::CALO'])
    #kwargs.setdefault('DeltaChord',         0.00000002)
    kwargs.setdefault('DeltaIntersection',  0.00000002)
    kwargs.setdefault('DeltaOneStep',       0.000001)
    kwargs.setdefault('MaximumEpsilonStep', 0.0000009)
    kwargs.setdefault('MinimumEpsilonStep', 0.000001)
    kwargs.setdefault('MuonOnlyField',      True)
    return getBasicDetectorFieldManagerTool(name, **kwargs)

def getMuonFieldManagerTool(name='MuonFieldManager', **kwargs):
    kwargs.setdefault("LogicalVolumes", ['MUONQ02::MUONQ02'])
    #kwargs.setdefault('DeltaChord',         0.00000002)
    kwargs.setdefault('DeltaIntersection',  0.00000002)
    kwargs.setdefault('DeltaOneStep',       0.000001)
    kwargs.setdefault('MaximumEpsilonStep', 0.0000009)
    kwargs.setdefault('MinimumEpsilonStep', 0.000001)
    return getBasicDetectorFieldManagerTool(name, **kwargs)


def getBasicFwdFieldMangerTool(name='FwdFieldManagerTool', **kwargs):
    #kwargs.setdefault('DeltaChord',         0.00000002)
    kwargs.setdefault('DeltaIntersection',  1e-9)
    kwargs.setdefault('DeltaOneStep',       1e-8)
    kwargs.setdefault('MaximumEpsilonStep', 1e-8)
    kwargs.setdefault('MinimumEpsilonStep', 1e-9)
    from G4AtlasApps.SimFlags import simFlags
    if simFlags.FwdStepLimitation.statusOn:
        kwargs.setdefault("MaximumStep", simFlags.FwdStepLimitation())
    return getBasicDetectorFieldManagerTool(name, **kwargs)

def getQ1FwdFieldMangerTool(name='Q1FwdFieldManager', **kwargs):
    kwargs.setdefault("FieldSvc",           "Q1FwdG4FieldSvc")
    kwargs.setdefault("LogicalVolumes", ['FwdRegion::LQXAA.1R1MagQ1'])
    return getBasicFwdFieldMangerTool(name, **kwargs)
def getQ2FwdFieldMangerTool(name='Q2FwdFieldManager', **kwargs):
    kwargs.setdefault("FieldSvc",           "Q2FwdG4FieldSvc")
    kwargs.setdefault("LogicalVolumes", ['FwdRegion::LQXBA.2R1MagQ2a', 'FwdRegion::LQXBA.2R1MagQ2b'])
    return getBasicFwdFieldMangerTool(name, **kwargs)
def getQ3FwdFieldMangerTool(name='Q3FwdFieldManager', **kwargs):
    kwargs.setdefault("FieldSvc",           "Q3FwdG4FieldSvc")
    kwargs.setdefault("LogicalVolumes", ['FwdRegion::LQXAG.3R1MagQ3'])
    return getBasicFwdFieldMangerTool(name, **kwargs)
def getD1FwdFieldMangerTool(name='D1FwdFieldManager', **kwargs):
    kwargs.setdefault("FieldSvc",           "D1FwdG4FieldSvc")
    kwargs.setdefault("LogicalVolumes", ['FwdRegion::MBXW.A4R1MagD1a', 'FwdRegion::MBXW.B4R1MagD1b',
                                         'FwdRegion::MBXW.C4R1MagD1c', 'FwdRegion::MBXW.D4R1MagD1d',
                                         'FwdRegion::MBXW.E4R1MagD1e', 'FwdRegion::MBXW.F4R1MagD1f'])
    return getBasicFwdFieldMangerTool(name, **kwargs)
def getD2FwdFieldMangerTool(name='D2FwdFieldManager', **kwargs):
    kwargs.setdefault("FieldSvc",           "D2FwdG4FieldSvc")
    kwargs.setdefault("LogicalVolumes", ['FwdRegion::LBRCD.4R1MagD2'])
    return getBasicFwdFieldMangerTool(name, **kwargs)
def getQ4FwdFieldMangerTool(name='Q4FwdFieldManager', **kwargs):
    kwargs.setdefault("FieldSvc",           "Q4FwdG4FieldSvc")
    kwargs.setdefault("LogicalVolumes", ['FwdRegion::LQYCH.4R1MagQ4'])
    return getBasicFwdFieldMangerTool(name, **kwargs)
def getQ5FwdFieldMangerTool(name='Q5FwdFieldManager', **kwargs):
    kwargs.setdefault("FieldSvc",           "Q5FwdG4FieldSvc")
    kwargs.setdefault("LogicalVolumes", ['FwdRegion::LQNDC.5R1MagQ5'])
    return getBasicFwdFieldMangerTool(name, **kwargs)
def getQ6FwdFieldMangerTool(name='Q6FwdFieldManager', **kwargs):
    kwargs.setdefault("FieldSvc",           "Q6FwdG4FieldSvc")
    kwargs.setdefault("LogicalVolumes", ['FwdRegion::LQNDD.6R1MagQ6'])
    return getBasicFwdFieldMangerTool(name, **kwargs)
def getQ7FwdFieldMangerTool(name='Q7FwdFieldManager', **kwargs):
    kwargs.setdefault("FieldSvc",           "Q7FwdG4FieldSvc")
    kwargs.setdefault("LogicalVolumes", ['FwdRegion::LQNFD.7R1MagQ7a', 'FwdRegion::LQNFD.7R1MagQ7b'])
    return getBasicFwdFieldMangerTool(name, **kwargs)
def getQ1HKickFwdFieldMangerTool(name='Q1HKickFwdFieldManager', **kwargs):
    kwargs.setdefault("FieldSvc",           "Q1HKickFwdG4FieldSvc")
    kwargs.setdefault("LogicalVolumes", ['FwdRegion::LQXAA.1R1MagQ1HKick'])
    return getBasicFwdFieldMangerTool(name, **kwargs)
def getQ1VKickFwdFieldMangerTool(name='Q1VKickFwdFieldManager', **kwargs):
    kwargs.setdefault("FieldSvc",           "Q1VKickFwdG4FieldSvc")
    kwargs.setdefault("LogicalVolumes", ['FwdRegion::LQXAA.1R1MagQ1VKick'])
    return getBasicFwdFieldMangerTool(name, **kwargs)
def getQ2HKickFwdFieldMangerTool(name='Q2HKickFwdFieldManager', **kwargs):
    kwargs.setdefault("FieldSvc",           "Q2HKickFwdG4FieldSvc")
    kwargs.setdefault("LogicalVolumes", ['FwdRegion::LQXBA.2R1MagQ2HKick'])
    return getBasicFwdFieldMangerTool(name, **kwargs)
def getQ2VKickFwdFieldMangerTool(name='Q2VKickFwdFieldManager', **kwargs):
    kwargs.setdefault("FieldSvc",           "Q2VKickFwdG4FieldSvc")
    kwargs.setdefault("LogicalVolumes", ['FwdRegion::LQXBA.2R1MagQ2VKick'])
    return getBasicFwdFieldMangerTool(name, **kwargs)
def getQ3HKickFwdFieldMangerTool(name='Q3HKickFwdFieldManager', **kwargs):
    kwargs.setdefault("FieldSvc",           "Q3HKickFwdG4FieldSvc")
    kwargs.setdefault("LogicalVolumes", ['FwdRegion::LQXAG.3R1MagQ3HKick'])
    return getBasicFwdFieldMangerTool(name, **kwargs)
def getQ3VKickFwdFieldMangerTool(name='Q3VKickFwdFieldManager', **kwargs):
    kwargs.setdefault("FieldSvc",           "Q3VKickFwdG4FieldSvc")
    kwargs.setdefault("LogicalVolumes", ['FwdRegion::LQXAG.3R1MagQ3VKick'])
    return getBasicFwdFieldMangerTool(name, **kwargs)
def getQ4VKickAFwdFieldMangerTool(name='Q4VKickAFwdFieldManager', **kwargs):
    kwargs.setdefault("FieldSvc",           "Q4VKickAFwdG4FieldSvc")
    kwargs.setdefault("LogicalVolumes", ['FwdRegion::LQYCH.4R1MagQ4VKickA'])
    return getBasicFwdFieldMangerTool(name, **kwargs)
def getQ4HKickFwdFieldMangerTool(name='Q4HKickFwdFieldManager', **kwargs):
    kwargs.setdefault("FieldSvc",           "Q4HKickFwdG4FieldSvc")
    kwargs.setdefault("LogicalVolumes", ['FwdRegion::LQYCH.4R1MagQ4HKick'])
    return getBasicFwdFieldMangerTool(name, **kwargs)
def getQ4VKickBFwdFieldMangerTool(name='Q4VKickBFwdFieldManager', **kwargs):
    kwargs.setdefault("FieldSvc",           "Q4VKickBFwdG4FieldSvc")
    kwargs.setdefault("LogicalVolumes", ['FwdRegion::LQYCH.4R1MagQ4VKickB'])
    return getBasicFwdFieldMangerTool(name, **kwargs)
def getQ5HKickFwdFieldMangerTool(name='Q5HKickFwdFieldManager', **kwargs):
    kwargs.setdefault("FieldSvc",           "Q5HKickFwdG4FieldSvc")
    kwargs.setdefault("LogicalVolumes", ['FwdRegion::LQNDC.5R1MagQ5HKick'])
    return getBasicFwdFieldMangerTool(name, **kwargs)
def getQ6VKickFwdFieldMangerTool(name='Q6VKickFwdFieldManager', **kwargs):
    kwargs.setdefault("FieldSvc",           "Q6VKickFwdG4FieldSvc")
    kwargs.setdefault("LogicalVolumes", ['FwdRegion::LQNDD.6R1MagQ6VKick'])
    return getBasicFwdFieldMangerTool(name, **kwargs)

def getFwdRegionFieldMangerTool(name='FwdRegionFieldManager', **kwargs):
    kwargs.setdefault("LogicalVolumes", ['FwdRegion::ForwardRegionGeoModel'])
    from G4AtlasApps.SimFlags import simFlags
    if simFlags.FwdStepLimitation.statusOn:
        kwargs.setdefault("MaximumStep", simFlags.FwdStepLimitation())
    return getBasicDetectorFieldManagerTool(name, **kwargs)

