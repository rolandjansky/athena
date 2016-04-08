# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon import CfgMgr

def getSimpleFastKiller(name="SimpleFastKiller", **kwargs):
    kwargs.setdefault("RegionNames" , ["BeampipeFwdCut"] )
    return CfgMgr.SimpleFastKillerTool(name, **kwargs)

def getDeadMaterialShower(name="DeadMaterialShower", **kwargs):
    kwargs.setdefault("RegionNames",        ["DeadMaterial"])
    return CfgMgr.DeadMaterialShowerTool(name, **kwargs)

