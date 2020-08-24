# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from AthenaCommon import CfgMgr, Logging
from G4AtlasServices import G4AtlasServicesConfig

def getVerboseSelectorTool(name="G4UA::VerboseSelectorTool", **kwargs):
    from G4AtlasApps.SimFlags import simFlags
    # example custom configuration
    if name in simFlags.UserActionConfig.get_Value().keys():
        for prop,value in simFlags.UserActionConfig.get_Value()[name].items():
            kwargs.setdefault(prop,value)
    return CfgMgr.G4UA__VerboseSelectorTool(name, **kwargs)

def getG4AtlantisDumperTool(name="G4UA::G4AtlantisDumperTool", **kwargs):
    return CfgMgr.G4UA__G4AtlantisDumperTool(name, **kwargs)

def getEnergyConservationTestTool(name="G4UA::EnergyConservationTestTool", **kwargs):
    return CfgMgr.G4UA__EnergyConservationTestTool(name, **kwargs)

def getHyperspaceCatcherTool(name="G4UA::HyperspaceCatcherTool", **kwargs):
    from G4AtlasApps.SimFlags import simFlags
    # example custom configuration
    if name in simFlags.UserActionConfig.get_Value().keys():
        for prop,value in simFlags.UserActionConfig.get_Value()[name].items():
            kwargs.setdefault(prop,value)
    return CfgMgr.G4UA__HyperspaceCatcherTool(name, **kwargs)

def getStepNtupleTool(name="G4UA::StepNtupleTool", **kwargs):
    from AthenaCommon.ConcurrencyFlags import jobproperties as concurrencyProps
    if concurrencyProps.ConcurrencyFlags.NumThreads() >1:
        log=Logging.logging.getLogger(name)
        log.fatal('Attempt to run '+name+' with more than one thread, which is not supported')
        #from AthenaCommon.AppMgr import theApp
        #theApp.exit(1)
        return False
    return CfgMgr.G4UA__StepNtupleTool(name, **kwargs)

def getVolumeDebuggerTool(name="G4UA::VolumeDebuggerTool", **kwargs):
    from AthenaCommon.ConcurrencyFlags import jobproperties as concurrencyProps

    from G4AtlasApps.SimFlags import simFlags
    # example custom configuration
    if name in simFlags.UserActionConfig.get_Value().keys():
        for prop,value in simFlags.UserActionConfig.get_Value()[name].items():
            kwargs.setdefault(prop,value)
    return CfgMgr.G4UA__VolumeDebuggerTool(name, **kwargs)

def getGeant4SetupCheckerTool(name="G4UA::Geant4SetupCheckerTool", **kwargs):
    # Set reference based on geometry
    from G4AtlasApps.SimFlags import simFlags
    default_file = '/afs/cern.ch/atlas/groups/Simulation/G4config_reference_files/default_reference.txt'
    test_file = '/afs/cern.ch/atlas/groups/Simulation/G4config_reference_files/'
    test_file+=simFlags.SimLayout().replace('_VALIDATION','')+'_reference.txt'
    import os
    if os.access(test_file,os.R_OK): default_file = test_file
    kwargs.setdefault('ReferenceFile',default_file)
    # Grab the properties that were already set
    if name in simFlags.UserActionConfig.get_Value().keys():
        for prop,value in simFlags.UserActionConfig.get_Value()[name].items():
            kwargs.setdefault(prop,value)
    # Set up the user action
    return CfgMgr.G4UA__Geant4SetupCheckerTool(name, **kwargs)

def getStepHistogramTool(name="G4UA::StepHistogramTool", **kwargs):
    from AthenaCommon.ConcurrencyFlags import jobproperties as concurrencyProps
    if concurrencyProps.ConcurrencyFlags.NumThreads() >1:
        log=Logging.logging.getLogger(name)
        log.fatal('Attempt to run '+name+' with more than one thread, which is not supported')
        return False
    from G4AtlasApps.SimFlags import simFlags
    if name in simFlags.UserActionConfig.get_Value().keys():
        for prop,value in simFlags.UserActionConfig.get_Value()[name].items():
            kwargs.setdefault(prop,value)
    return CfgMgr.G4UA__StepHistogramTool(name, **kwargs)

