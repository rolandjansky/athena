# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from AthenaCommon import CfgMgr
from ExtraParticles import PDGHelpers

from G4AtlasApps.SimFlags import simFlags


def getExtraParticlesPhysicsTool(name="ExtraParticlesPhysicsTool", **kwargs):
    parser = PDGHelpers.PDGParser(simFlags.ExtraParticlesPDGTABLE.get_Value(),
                                  simFlags.ExtraParticlesRanges.get_Value())

    kwargs.setdefault("ExtraParticlesConfig", parser.createList())

    return CfgMgr.ExtraParticlesPhysicsTool(name, **kwargs)
