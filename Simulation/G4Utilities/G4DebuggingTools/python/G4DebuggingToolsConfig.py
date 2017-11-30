# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon import CfgMgr, Logging
from G4AtlasServices import G4AtlasServicesConfig

def getVerboseSelectorTool(name="G4UA::VerboseSelectorTool", **kwargs):
    from G4AtlasApps.SimFlags import simFlags
    # example custom configuration
    if name in simFlags.UserActionConfig.get_Value().keys():
        for prop,value in simFlags.UserActionConfig.get_Value()[name].iteritems():
            kwargs.setdefault(prop,value)
    return CfgMgr.G4UA__VerboseSelectorTool(name, **kwargs)


def addVerboseSelectorTool(name="G4UA::VerboseSelectorTool",system=False):
    G4AtlasServicesConfig.addAction(name,['Event','Step','Tracking'],system)


def getG4AtlantisDumperTool(name="G4UA::G4AtlantisDumperTool", **kwargs):
    return CfgMgr.G4UA__G4AtlantisDumperTool(name, **kwargs)


def addG4AtlantisDumperTool(name="G4UA::G4AtlantisDumperTool",system=False):
    G4AtlasServicesConfig.addAction(name,['Event','Step','Event'],system)


def getEnergyConservationTestTool(name="G4UA::EnergyConservationTestTool", **kwargs):
    return CfgMgr.G4UA__EnergyConservationTestTool(name, **kwargs)


def addEnergyConservationTestTool(name="G4UA::EnergyConservationTestTool",system=False):
    G4AtlasServicesConfig.addAction(name,['Event','Step','Tracking'],system)


def getHyperspaceCatcherTool(name="G4UA::HyperspaceCatcherTool", **kwargs):
    from G4AtlasApps.SimFlags import simFlags
    # example custom configuration
    if name in simFlags.UserActionConfig.get_Value().keys():
        for prop,value in simFlags.UserActionConfig.get_Value()[name].iteritems():
            kwargs.setdefault(prop,value)
    return CfgMgr.G4UA__HyperspaceCatcherTool(name, **kwargs)


def addHyperspaceCatcherTool(name="G4UA::HyperspaceCatcherTool",system=False):
    G4AtlasServicesConfig.addAction(name,['Run','Step'],system)


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
        for prop,value in simFlags.UserActionConfig.get_Value()[name].iteritems():
            kwargs.setdefault(prop,value)
    return CfgMGr.G4UA__VolumeDebuggerTool(name, **kwargs)


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
        for prop,value in simFlags.UserActionConfig.get_Value()[name].iteritems():
            kwargs.setdefault(prop,value)
    # Set up the user action
    return CfgMgr.G4UA__Geant4SetupCheckerTool(name, **kwargs)

