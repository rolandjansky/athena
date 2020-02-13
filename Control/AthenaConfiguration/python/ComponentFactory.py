# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

#Wrapper to ease the transition to GaudiConfig2
#Current version imitate CfgMgr to get configurables
from AthenaCommon.ConfigurableDb import getConfigurable

class _compFactory():
    def __getattr__(self,cfgName):
        if not cfgName.startswith("__"):
            return getConfigurable(cfgName,assumeCxxClass=False)

    def getComps(self, *manyNames):
        return [getConfigurable(cfgName,assumeCxxClass=False) for cfgName in manyNames]
            
CompFactory=_compFactory()
del _compFactory
