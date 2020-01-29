# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

#Wrapper to ease the transition to GaudiConfig2
#This is the version for GaudiConfig2

#extend Semantics for ATLAS-specific cases:
import AthenaConfiguration.AtlasSemantics # noqa: F401

#Get Configurable database from Gaudi:
from GaudiConfig2 import Configurables as _cfgs
class _compFactory():
    def __getattr__(self,cfgName):
        if not cfgName.startswith("__"):
            return getattr(_cfgs,cfgName)

    def getComps(self, *manyNames):
        return [_cfgs.getByType(cfgName) for cfgName in manyNames]
           
CompFactory=_compFactory()
del _compFactory
