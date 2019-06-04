"""ComponentAccumulator configuration utilities for LArRecUtils

Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
"""
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from LArCabling.LArCablingConfig import LArOnOffIdMappingCfg
from LArRecUtils.LArFebConfigCondAlgConfig import LArFEBConfigCondAlgCfg
from LArRecUtils.LArRecUtilsConf import LArADC2MeVCondAlg
from LArRecUtils.LArRecUtilsConf import LArMCSymCondAlg
from LArRecUtils.LArRecUtilsConf import LArAutoCorrNoiseCondAlg

def LArADC2MeVCondAlgCfg(flags, **kwargs):
    """Return ComponentAccumulator with configured LArADC2MeVCondAlg"""
    acc = LArOnOffIdMappingCfg(flags)
    if flags.Input.isMC:
        if flags.LAr.HasMphys:
            LArMphysOverMcalKey = "LArMphysOverMcalSym"
        else:
            LArMphysOverMcalKey = ""
        kwargs.setdefault("LArMphysOverMcalKey", LArMphysOverMcalKey)
        if not flags.LAr.HasHVCorr:
            kwargs.setdefault("LArHVScaleCorrKey", "")
        kwargs.setdefault("LAruA2MeVKey", "LAruA2MeVSym")
        kwargs.setdefault("LArDAC2uAKey", "LArDAC2uASym")
        kwargs.setdefault("LArRampKey", "LArRampSym")
        kwargs.setdefault("UseFEBGainTresholds", False)
    else:
        acc.merge(LArFEBConfigCondAlgCfg(flags))
    acc.addCondAlgo(LArADC2MeVCondAlg(**kwargs))
    return acc

def LArMCSymCondAlgCfg(flags, name="LArMCSymCondAlg", **kwargs):
    """Return ComponentAccumulator with configured LArMCSymCondAlg"""
    acc = ComponentAccumulator()
    kwargs.setdefault("ReadKey", "LArOnOffIdMap")
    acc.addCondAlgo(LArMCSymCondAlg(name, **kwargs))
    return acc

def LArAutoCorrNoiseCondAlgCfg(flags, name="LArAutoCorrNoiseCondAlg", **kwargs):
    """Return ComponentAccumulator with configured LArAutoCorrNoiseCondAlg"""
    acc = LArOnOffIdMappingCfg(flags)
    acc.merge(LArMCSymCondAlgCfg(flags))
    kwargs.setdefault("nSampl", flags.LAr.ROD.nSamples)
    acc.addCondAlgo(LArAutoCorrNoiseCondAlg(name, **kwargs))
    return acc

