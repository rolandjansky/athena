# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon import CfgMgr
from G4AtlasServices import G4AtlasServicesConfig

def getG4HitFilterTool(name="G4UA::G4HitFilterTool", **kwargs):
    from G4HitFilter.G4HitFilterConf import G4UA__G4HitFilterTool
    return G4UA__G4HitFilterTool(name, **kwargs)

def addG4HitFilterTool(name="G4UA::G4HitFilterTool",system=False):
    G4AtlasServicesConfig.addAction(name,['Run','Event'],system)
