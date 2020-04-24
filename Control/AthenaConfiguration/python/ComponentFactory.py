# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

import sys


def isRun3Cfg():
    """
    Returns true if the python fragment is run-3 style 
    (ComponentAccumulator, GaudiConfig2)
    Returns false if the python fragments is run 1/2 style
    (PackageConf.py, RecExCommon)
    """
    from AthenaCommon.Configurable import Configurable
    if "AthenaCommon.Include" not in sys.modules\
        or Configurable.configurableRun3Behavior:
        return True
    else:
        return False


#This version of CompFactory provides the RecExCommon-style configurables 
# used by athena.py. Internally works like CfgMgr

from AthenaCommon.ConfigurableDb import getConfigurable
class _compFactory1():
    def __getattr__(self,cfgName):
        if not cfgName.startswith("__"):
            return getConfigurable(cfgName.replace("::","__"),assumeCxxClass=False)

    def getComp(self, cfgName):
        return getConfigurable(cfgName.replace("::","__"),assumeCxxClass=False)

    def getComps(self, *manyNames):
        return [getConfigurable(cfgName.replace("::","__"),assumeCxxClass=False) for cfgName in manyNames]

#This version of the CompFactory provides GaudiConfig2-style Configurables
from GaudiConfig2 import Configurables as _cfgs
class _compFactory2():
    #Get Configurable database from Gaudi:
    def __getattr__(self,cfgName):
        if not cfgName.startswith("__"):
            return getattr(_cfgs,cfgName)

    def getComp(self, oneName):
        return _cfgs.getByType(oneName)

    def getComps(self, *manyNames):
        return [_cfgs.getByType(cfgName) for cfgName in manyNames]


#extend Semantics for ATLAS-specific cases:
import AthenaConfiguration.AtlasSemantics # noqa: F401

#Dynamically switch between the two versions 
class _compFactory():
    def _getFactory(self):

        if isRun3Cfg():
            return _compFactory2()
        else:
            return _compFactory1()

    def __getattr__(self,cfgName):
        return getattr(self._getFactory(),cfgName)


    def getComp(self, oneName):
        return self._getFactory().getComp(oneName)

    def getComps(self, *manyNames):
        return self._getFactory().getComps(*manyNames)


CompFactory=_compFactory()
