# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon import CfgMgr
from G4AtlasServices import G4AtlasServicesConfig

def getVerboseSelectorTool(name="G4UA::VerboseSelectorTool", **kwargs):
    from G4DebuggingTools.G4DebuggingToolsConf import G4UA__VerboseSelectorTool
    return G4UA__VerboseSelectorTool(name, **kwargs)

def addVerboseSelectorTool(name="G4UA::VerboseSelectorTool",system=False):
    G4AtlasServicesConfig.addAction(name,['BeginOfEvent','Step','BeginOfTracking','EndOfTracking'],system)

def getG4AtlantisDumperTool(name="G4UA::G4AtlantisDumperTool", **kwargs):
    from G4DebuggingTools.G4DebuggingToolsConf import G4UA__G4AtlantisDumperTool
    return G4UA__G4AtlantisDumperTool(name, **kwargs)

def addG4AtlantisDumperTool(name="G4UA::G4AtlantisDumperTool",system=False):
    G4AtlasServicesConfig.addAction(name,['EndOfEvent','Step','BeginOfEvent'],system)


def getEnergyConservationTestTool(name="G4UA::EnergyConservationTestTool", **kwargs):
    from G4DebuggingTools.G4DebuggingToolsConf import G4UA__EnergyConservationTestTool
    return G4UA__EnergyConservationTestTool(name, **kwargs)

def addEnergyConservationTestTool(name="G4UA::EnergyConservationTestTool",system=False):
    G4AtlasServicesConfig.addAction(name,['EndOfEvent','Step','BeginOfTracking','EndOfTracking'],system)

def getHyperspaceCatcherTool(name="G4UA::HyperspaceCatcherTool", **kwargs):
    from G4AtlasApps.SimFlags import simFlags
    # example custom configuration
    if name in simFlags.UserActionConfig.get_Value().keys():
        for prop,value in simFlags.UserActionConfig.get_Value()[name].iteritems():
            kwargs.setdefault(prop,value)

    from G4DebuggingTools.G4DebuggingToolsConf import G4UA__HyperspaceCatcherTool
    return G4UA__HyperspaceCatcherTool(name, **kwargs)

def addHyperspaceCatcherTool(name="G4UA::HyperspaceCatcherTool",system=False):
    G4AtlasServicesConfig.addAction(name,['BeginOfRun','Step'],system)
