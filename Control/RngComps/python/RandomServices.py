"""Define functions to construct random number services

Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
"""
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from RngComps.RngCompsConf import (
    AtDSFMTGenSvc,
    AtRanluxGenSvc,
    AtRndmGenSvc,
    AthRNGSvc,
)

# Translate between names in AthRNGSvc and elsewhere
AthEngines = {
    "dSFMT" : "AtDSFMTGenSvc",
    "Ranlux64" : "AtRanluxGenSvc",
    "Ranecu" : "AtRndmGenSvc",
}

def dSFMT(seed, name="AtDSFMTGenSvc"):
    """Return a ComponentAccumulator containing an AtDSFMTGenSvc with seed"""
    acc = ComponentAccumulator(name)
    service = AtDSFMTGenSvc()
    service.Seeds.append(seed)
    acc.addService(service)
    return acc

def Ranlux64(seed, name="AtRanluxGenSvc"):
    """Return a ComponentAccumulator containing an AtRanluxGenSvc with seed"""
    acc = ComponentAccumulator()
    service = AtRanluxGenSvc(name)
    service.Seeds.append(seed)
    acc.addService(service)
    return acc

def Ranecu(seed, name="AtRndmGenSvc"):
    """Return a ComponentAccumulator containing an AtRndmGenSvc with seed"""
    acc = ComponentAccumulator()
    service = AtRndmGenSvc(name)
    service.Seeds.append(seed)
    acc.addService(service)
    return acc

def RNG(engine="dSFMT", name="AthRNGSvc"):
    """Return a ComponentAccumulator containing an AthRNGSvc"""
    acc = ComponentAccumulator()
    service = AthRNGSvc(name)
    service.EngineType = engine
    acc.addService(service)
    return acc

