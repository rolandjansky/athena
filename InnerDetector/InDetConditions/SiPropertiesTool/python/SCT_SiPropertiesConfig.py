"""Define methods to configure SCT SiProperties

Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
"""
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from SiPropertiesTool.SiPropertiesToolConf import SiPropertiesTool
from SiPropertiesTool.SiPropertiesToolConf import SCTSiPropertiesCondAlg

def SCT_SiPropertiesToolCfg(flags, name="SCT_SiPropertiesTool", **kwargs):
    """Return a SiPropertiesTool configured for SCT"""
    kwargs.setdefault("DetectorName", "SCT")
    kwargs.setdefault("ReadKey", "SCTSiliconPropertiesVector")
    return SiPropertiesTool(name=name, **kwargs)

def SCT_SiPropertiesCfg(flags, name="SCTSiPropertiesCondAlg", **kwargs):
    """Return configured ComponentAccumulator and tool for SCT_SiProperties

    SiConditionsTool and/or SiPropertiesTool may be provided in kwargs
    """
    acc = ComponentAccumulator()
    tool = kwargs.get("SiPropertiesTool", SCT_SiPropertiesToolCfg(flags))
    alg = SCTSiPropertiesCondAlg(name, **kwargs)
    acc.addEventAlgo(alg)
    acc.setPrivateTools(tool)
    return acc

