# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from AthenaCommon import CfgMgr
from ExtraParticles.PDGHelpers import getPDGTABLE, PDGParser

from G4AtlasApps.SimFlags import simFlags


def getExtraParticlesPhysicsTool(name="ExtraParticlesPhysicsTool", **kwargs):
    if getPDGTABLE(simFlags.ExtraParticlesPDGTABLE.get_Value()):
        parser = PDGParser(simFlags.ExtraParticlesPDGTABLE.get_Value(),
                           simFlags.ExtraParticlesRanges.get_Value())
    else:
        print("ERROR Failed to find PDGTABLE.MEV file.")
    kwargs.setdefault("ExtraParticlesConfig", parser.createList())

    return CfgMgr.ExtraParticlesPhysicsTool(name, **kwargs)
