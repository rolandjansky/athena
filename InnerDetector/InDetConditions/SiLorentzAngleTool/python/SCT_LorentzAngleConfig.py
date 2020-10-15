"""Define methods to configure SCTLorentzAngleTool

Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
"""
from AthenaConfiguration.ComponentFactory import CompFactory
SiLorentzAngleTool=CompFactory.SiLorentzAngleTool
SCTSiLorentzAngleCondAlg=CompFactory.SCTSiLorentzAngleCondAlg
from SCT_ConditionsTools.SCT_SiliconConditionsConfig import SCT_SiliconConditionsCfg
from SCT_GeoModel.SCT_GeoModelConfig import SCT_GeometryCfg
from MagFieldServices.MagFieldServicesConfig import MagneticFieldSvcCfg

def SCT_LorentzAngleCfg(flags, name="SCT_SiLorentzAngleCondAlg",
                        forceUseGeoModel=False, **kwargs):
    """Return configured ComponentAccumulator and tool for SCT_LorentzAngle

    SiConditionsTool and/or DCSConditionsTool may be provided in kwargs
    """
    # Condition algorithm
    # construct with field services
    acc = MagneticFieldSvcCfg(flags)
    # For SCT_ID and SCT_DetectorElementCollection used
    # in SCTSiLorentzAngleCondAlg and SiLorentzAngleTool
    acc.merge(SCT_GeometryCfg(flags))
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
    acc.addCondAlgo(SCTSiLorentzAngleCondAlg(name, **algkwargs))

    # Condition tool
    toolkwargs = {}
    toolkwargs["DetectorName"] = "SCT"
    toolkwargs["DetEleCollKey"] = "SCT_DetectorElementCollection"
    toolkwargs["SiLorentzAngleCondData"] = "SCTSiLorentzAngleCondData"
    acc.setPrivateTools(SiLorentzAngleTool(name, **toolkwargs))
    return acc
