# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
from __future__ import print_function
from AthenaConfiguration.ComponentFactory import CompFactory
LArFastShowerTool = CompFactory.LArFastShowerTool
LArG4ShowerLibSvc = CompFactory.LArG4ShowerLibSvc

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator

def LArG4ShowerLibSvcCfg(ConfigFlags, **kwargs):
    result = ComponentAccumulator()
    fileNameList = []
    #TODO make this configurable based on ConfigFlags?
    # FCAL1 frozen shower libraries
    fileNameList += [ "LArG4ShowerLibData/MC21_v2/LArG4ShowerLib.FCAL1.11.root",
                      "LArG4ShowerLibData/MC21_v2/LArG4ShowerLib.FCAL1.22.root",
                      "LArG4ShowerLibData/MC21_v2/LArG4ShowerLib.FCAL1.2112.root",
                      "LArG4ShowerLibData/MC21_v2/LArG4ShowerLib.FCAL1.211.root" ]
    # FCAL2 frozen shower libraries
    fileNameList += [ "LArG4ShowerLibData/MC21_v2/LArG4ShowerLib.FCAL2.11.root",
                      "LArG4ShowerLibData/MC21_v2/LArG4ShowerLib.FCAL2.22.root",
                      "LArG4ShowerLibData/MC21_v2/LArG4ShowerLib.FCAL2.2112.root",
                      "LArG4ShowerLibData/MC21_v2/LArG4ShowerLib.FCAL2.211.root" ]
    kwargs.setdefault("FileNameList", fileNameList)
    result.addService(LArG4ShowerLibSvc(name="LArG4ShowerLibSvc", **kwargs))
    return result


def EMBFastShowerCfg(ConfigFlags, **kwargs):
    result = ComponentAccumulator()
    result.merge(LArG4ShowerLibSvcCfg(ConfigFlags))
    kwargs.setdefault("RegionNames",        ["EMB"])
    kwargs.setdefault("EFlagToShowerLib",   False)
    kwargs.setdefault("GFlagToShowerLib",   False)
    kwargs.setdefault("NeutFlagToShowerLib",False)
    kwargs.setdefault("PionFlagToShowerLib",False)
    kwargs.setdefault("ContainLow",         True)
    kwargs.setdefault("AbsLowEta",          0.3)
    kwargs.setdefault("ContainHigh",        True)
    kwargs.setdefault("AbsHighEta",         1.1)
    kwargs.setdefault("ContainCrack",       True)
    kwargs.setdefault("AbsCrackEta1",       0.5)
    kwargs.setdefault("AbsCrackEta2",       1.1)
    kwargs.setdefault("DetectorTag",        100000)
    kwargs.setdefault("SensitiveDetector",  "BarrelFastSimDedicatedSD")
    kwargs.setdefault("EMinEneShowerLib",   0.51)
    result.setPrivateTools( LArFastShowerTool(name="EMBFastShower", **kwargs) )
    return result


def EMECFastShowerCfg(ConfigFlags, **kwargs):
    result = ComponentAccumulator()
    result.merge(LArG4ShowerLibSvcCfg(ConfigFlags))
    kwargs.setdefault("RegionNames",        ["EMECPara"])
    kwargs.setdefault("EFlagToShowerLib",   False)
    kwargs.setdefault("GFlagToShowerLib",   False)
    kwargs.setdefault("NeutFlagToShowerLib",False)
    kwargs.setdefault("PionFlagToShowerLib",False)
    kwargs.setdefault("ContainLow",         True)
    kwargs.setdefault("AbsLowEta",          1.8)
    kwargs.setdefault("ContainHigh",        True)
    kwargs.setdefault("AbsHighEta",         2.9)
    kwargs.setdefault("ContainCrack",       True)
    kwargs.setdefault("AbsCrackEta1",       2.2)
    kwargs.setdefault("AbsCrackEta2",       2.8)
    kwargs.setdefault("DetectorTag",        200000)
    kwargs.setdefault("SensitiveDetector", "EndcapFastSimDedicatedSD")
    kwargs.setdefault("EMinEneShowerLib",   0.51)
    result.setPrivateTools( LArFastShowerTool(name="EMECFastShower", **kwargs) )
    return result


def FCALFastShowerCfg(ConfigFlags, **kwargs):
    result = ComponentAccumulator()
    result.merge(LArG4ShowerLibSvcCfg(ConfigFlags))
    kwargs.setdefault("RegionNames",        ["FCALPara"])
    kwargs.setdefault("EFlagToShowerLib",   True)
    kwargs.setdefault("GFlagToShowerLib",   True)
    kwargs.setdefault("NeutFlagToShowerLib",True)
    kwargs.setdefault("PionFlagToShowerLib",False)
    kwargs.setdefault("ContainLow",         False)
    kwargs.setdefault("AbsLowEta",          4.0)
    kwargs.setdefault("ContainHigh",        False)
    kwargs.setdefault("AbsHighEta",         4.6)
    kwargs.setdefault("DetectorTag",        300000)
    kwargs.setdefault("SensitiveDetector", "FCALFastSimDedicatedSD")
    kwargs.setdefault("EMinEneShowerLib",   3.0)
    result.setPrivateTools( LArFastShowerTool(name="FCALFastShower", **kwargs) )
    return result


def FCAL2FastShowerCfg(ConfigFlags, **kwargs):
    result = ComponentAccumulator()
    result.merge(LArG4ShowerLibSvcCfg(ConfigFlags))
    kwargs.setdefault("RegionNames",        ["FCAL2Para"])
    kwargs.setdefault("EFlagToShowerLib",   True)
    kwargs.setdefault("GFlagToShowerLib",   True)
    kwargs.setdefault("NeutFlagToShowerLib",True)
    kwargs.setdefault("PionFlagToShowerLib",False)
    kwargs.setdefault("ContainLow",         True)
    kwargs.setdefault("AbsLowEta",          3.8)
    kwargs.setdefault("ContainHigh",        True)
    kwargs.setdefault("AbsHighEta",         4.4)
    kwargs.setdefault("DetectorTag",        400000)
    kwargs.setdefault("SensitiveDetector", "FCALFastSimDedicatedSD")
    kwargs.setdefault("EMinEneShowerLib",   1.0)
    result.setPrivateTools( LArFastShowerTool(name="FCAL2FastShower", **kwargs) )
    return result
