# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from AthenaCommon import CfgMgr
from Mesons.MesonsList import createMesonsList

def getMesonsPhysicsTool(name="MesonsPhysicsTool", **kwargs):
    kwargs.setdefault("OutputLevel", 1)
    kwargs.setdefault("MesonsConfig", createMesonsList())

    return CfgMgr.MesonsPhysicsTool(name, **kwargs)

