"""Define methods to configure SCTLorentzAngleTool

Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
"""
from AthenaConfiguration.ComponentFactory import CompFactory
from MagFieldServices.MagFieldServicesConfig import AtlasFieldCacheCondAlgCfg
from SCT_ConditionsTools.SCT_ConditionsToolsConfig import SCT_SiliconConditionsCfg
from SCT_GeoModel.SCT_GeoModelConfig import SCT_ReadoutGeometryCfg

def SCT_LorentzAngleToolCfg(flags, name="SCTLorentzAngleTool", **kwargs):
    """Return a SiLorentzAngleTool configured for SCT"""
    acc = SCT_LorentzAngleCondAlgCfg(flags)
    kwargs.setdefault("DetectorName", "SCT")
    kwargs.setdefault("SiLorentzAngleCondData", "SCTSiLorentzAngleCondData")
    kwargs.setdefault("DetEleCollKey", "SCT_DetectorElementCollection")
    acc.setPrivateTools(CompFactory.SiLorentzAngleTool(name, **kwargs))
    return acc

def SCT_LorentzAngleCondAlgCfg(flags, name="SCTSiLorentzAngleCondAlg",
                               forceUseGeoModel=False, **kwargs):
    # Condition algorithm
    # construct with field services
    acc = AtlasFieldCacheCondAlgCfg(flags)
    # For SCT_ID and SCT_DetectorElementCollection used
    # in SCTSiLorentzAngleCondAlg and SiLorentzAngleTool
    acc.merge(SCT_ReadoutGeometryCfg(flags))
    # set up SCTSiLorentzAngleCondAlg
    algkwargs = {}
    algkwargs["UseMagFieldCache"] = kwargs.get("UseMagFieldCache", True)
    algkwargs["UseMagFieldDcs"] = kwargs.get("UseMagFieldDcs", not flags.Common.isOnline)
    algkwargs["UseGeoModel"] = forceUseGeoModel
    algkwargs["useSctDefaults"] = kwargs.get("useSctDefaults", False)
    if not algkwargs["useSctDefaults"]:
        if kwargs.get("SiConditionsTool"):
            algkwargs["SiConditionsTool"] = kwargs.get("SiConditionsTool")
        else:
            sikwargs = {}
            if kwargs.get("DCSConditionsTool"):
                sikwargs["DCSConditionsTool"] = kwargs.get("DCSConditionsTool")
            sikwargs["ForceUseGeoModel"] = forceUseGeoModel
            algkwargs["SiConditionsTool"] = acc.popToolsAndMerge(SCT_SiliconConditionsCfg(flags, **sikwargs))
    acc.addCondAlgo(CompFactory.SCTSiLorentzAngleCondAlg(name, **algkwargs))
    return acc
