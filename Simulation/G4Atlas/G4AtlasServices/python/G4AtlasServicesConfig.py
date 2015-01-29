# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon import CfgMgr

def generateSensitiveDetectorList():
    SensitiveDetectorList=[]
    from AthenaCommon.DetFlags import DetFlags
    if DetFlags.simulate.SCT_on():
        SensitiveDetectorList += [ ] # Empty list for the time being - building as they are migrated
    return SensitiveDetectorList

def getSensitiveDetectorSvc(name="SensitiveDetectorSvc", **kwargs):
    kwargs.setdefault("SensitiveDetectors", generateSensitiveDetectorList())
    return CfgMgr.SensitiveDetectorSvc(name, **kwargs)

def getFastSimulationSvc(name="FastSimulationSvc", **kwargs):
    FastSimulationList=[]
    # Eventually we will be setting up fast simulations here
    kwargs.setdefault("FastSimulations", FastSimulationList)
    return CfgMgr.FastSimulationSvc(name, **kwargs)
