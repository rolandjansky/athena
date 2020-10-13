"""Define methods to configure SCT SiProperties

Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
"""
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from SCT_ConditionsTools.SCT_DCSConditionsConfig import SCT_DCSConditionsCfg
from SCT_ConditionsTools.SCT_SiliconConditionsConfig import SCT_SiliconConditionsCfg
from SCT_GeoModel.SCT_GeoModelConfig import SCT_GeometryCfg
SiPropertiesTool=CompFactory.SiPropertiesTool
SCTSiPropertiesCondAlg=CompFactory.SCTSiPropertiesCondAlg

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

    acc.merge(SCT_GeometryCfg(flags)) # For SCT_DetectorElementCollection used in SCTSiPropertiesCondAlg

    DCSkwargs = {}
    # For HLT
    if flags.Common.isOnline and not flags.Input.isMC:
        dcs_folder = "/SCT/HLT/DCS"
        DCSkwargs["dbInstance"] = "SCT"
        DCSkwargs["hvFolder"] = dcs_folder + "/HV"
        DCSkwargs["tempFolder"] = dcs_folder + "/MODTEMP"
        DCSkwargs["stateFolder"] = dcs_folder + "/CHANSTAT"
    DCSAcc = SCT_DCSConditionsCfg(flags, **DCSkwargs)
    SCAcc = SCT_SiliconConditionsCfg(flags, DCSConditionsTool=DCSAcc.popPrivateTools())
    acc.merge(DCSAcc)
    acc.merge(SCAcc)

    tool = kwargs.get("SiPropertiesTool", SCT_SiPropertiesToolCfg(flags))
    alg = SCTSiPropertiesCondAlg(name, **kwargs)
    acc.addCondAlgo(alg)
    acc.setPrivateTools(tool)
    return acc

