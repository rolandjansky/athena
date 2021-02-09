# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from AthenaCommon import CfgMgr
from ExtraParticles import PDGParser

from G4AtlasApps.SimFlags import SimFlags


def getExtraParticlesPhysicsTool(name="ExtraParticlesPhysicsTool", **kwargs):
    parser = PDGParser.PDGParser(SimFlags.ExtraParticlesPDGTABLE.get_Value(),
                                 SimFlags.ExtraParticlesRanges.get_Value())

    kwargs.setdefault("ExtraParticlesConfig", parser.createList())

    return CfgMgr.ExtraParticlesPhysicsTool(name, **kwargs)
