# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon import CfgMgr
def getMonopolePhysicsTool(name="MonopolePhysicsTool", **kwargs):
    return CfgMgr.MonopolePhysicsTool(name, **kwargs)

def getG4mplEqMagElectricFieldTool(name="G4mplEqMagElectricField", **kwargs):
    return CfgMgr.G4mplEqMagElectricFieldTool(name, **kwargs)
