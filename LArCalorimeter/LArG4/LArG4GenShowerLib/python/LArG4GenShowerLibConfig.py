# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon import CfgMgr


def getLArG4GenShowerLib(name="LArG4GenShowerLib", **kwargs):
    from G4AtlasApps.SimFlags import simFlags
    kwargs.setdefault("PhysicsList",simFlags.PhysicsList.get_Value())
    return CfgMgr.LArG4GenShowerLib(name, **kwargs)


def getTestActionShowerLibTool(name="G4UA::TestActionShowerLibTool", **kwargs):
    from LArG4GenShowerLib.LArG4GenShowerLibConf import G4UA__TestActionShowerLibTool
    return G4UA__TestActionShowerLibTool(name,**kwargs)