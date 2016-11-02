# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

def getMaterialStepRecorderTool(name="G4UA::MaterialStepRecorderTool", **kwargs):
    from G4AtlasApps.SimFlags import simFlags
    # example custom configuration
    if name in simFlags.UserActionConfig.get_Value().keys():
        for prop,value in simFlags.UserActionConfig.get_Value()[name].iteritems():
            kwargs.setdefault(prop,value)
    from TrkG4UserActions.TrkG4UserActionsConf import G4UA__MaterialStepRecorderTool
    return G4UA__MaterialStepRecorderTool(name, **kwargs)
from AthenaCommon import CfgMgr

#-------------------------------------------------------------------
def getMaterialStepRecorder(name="MaterialStepRecorder", **kwargs):
    return CfgMgr.MaterialStepRecorder(name, **kwargs)
#-------------------------------------------------------------------
def getPositionMomentumWriter(name="PosMomWriter", **kwargs):
    return CfgMgr.Trk__PositionMomentumWriter(name, **kwargs)

def getEnergyLossRecorder(name="EnergyLossRecorder", **kwargs):
    kwargs.setdefault("PositionMomentumWriter", "PosMomWriter")
    return CfgMgr.EnergyLossRecorder(name, **kwargs)
#-------------------------------------------------------------------
def getGeantFollowerHelper(name="GeantFollowerHelper", **kwargs):
    from AthenaCommon.AppMgr import ToolSvc
    kwargs.setdefault("Extrapolator", ToolSvc.TestExtrapolator) ##FIXME Assumes that this already exists..
    kwargs.setdefault("ExtrapolateDirectly", True)
    kwargs.setdefault("ExtrapolateIncrementally", True)
    return CfgMgr.Trk__GeantFollowerHelper(name, **kwargs)

def getGeantFollower(name="GeantFollower", **kwargs):
    return CfgMgrGeantFollower.(name, **kwargs)
#-------------------------------------------------------------------
def getGeantFollowerMSHelper(name="GeantFollowerMSHelper", **kwargs):
    from AthenaCommon.AppMgr import ToolSvc
    kwargs.setdefault("Extrapolator", ToolSvc.TestExtrapolator) ##FIXME Assumes that this already exists..
    kwargs.setdefault("ExtrapolateDirectly", False) # for geantinos ExtrapolateDirectly = True no Eloss is calculated
    kwargs.setdefault("ExtrapolateIncrementally", False)
    # SpeedUp False takes more CPU because it will stop at each G4 Step in the Muon Spectrometer
    kwargs.setdefault("SpeedUp", True)
    return CfgMgr.Trk__GeantFollowerMSHelper(name, **kwargs)

def getGeantFollowerMS(name="GeantFollowerMS", **kwargs):
    kwargs.setdefault("HelperTool", "GeantFollowerMSHelper")
    return CfgMgr.GeantFollowerMS(name, **kwargs)
#-------------------------------------------------------------------
