# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from AthenaCommon import CfgMgr
from G4AtlasApps.SimFlags import SimFlags


def getG4EMProcessesPhysicsTool(name="G4EMProcessesPhysicsTool", **kwargs):
    kwargs.setdefault("ParticleList", SimFlags.G4EMProcessesParticleList.get_Value())
    return CfgMgr.G4EMProcessesPhysicsTool(name, **kwargs)
