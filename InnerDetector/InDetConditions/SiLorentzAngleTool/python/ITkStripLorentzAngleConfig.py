"""Define methods to configure ITkStripLorentzAngleTool

Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
"""
from AthenaConfiguration.ComponentFactory import CompFactory
SiLorentzAngleTool=CompFactory.SiLorentzAngleTool
StripSiLorentzAngleCondAlg=CompFactory.SCTSiLorentzAngleCondAlg
from StripGeoModelXml.ITkStripGeoModelConfig import ITkStripGeometryCfg
from SCT_ConditionsTools.SCT_SiliconConditionsConfig import SCT_SiliconConditionsCfg

from MagFieldServices.MagFieldServicesConfig import MagneticFieldSvcCfg

def ITkStripLorentzAngleCfg(flags, name="ITkStripSiLorentzAngleCondAlg",
                        forceUseGeoModel=False, **kwargs):
    """Return configured ComponentAccumulator and tool for SCT_LorentzAngle

    SiConditionsTool and/or DCSConditionsTool may be provided in kwargs
    """
    # Condition algorithm
    # construct with field services
    acc = MagneticFieldSvcCfg(flags)
    # For SCT_ID and SCT_DetectorElementCollection used
    # in SCTSiLorentzAngleCondAlg and SiLorentzAngleTool
    acc.merge(ITkStripGeometryCfg(flags))
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
    #Specify correct DetElCollection for ITkStrip
    algkwargs["SCTDetEleCollKey"] = "ITkStripDetectorElementCollection"
    acc.addCondAlgo(StripSiLorentzAngleCondAlg(name, **algkwargs))

    # Condition tool
    toolkwargs = {}
    #Update ti ITkStrip eventually - tool assumes Pixel or SCT for the moment
    toolkwargs["DetectorName"] = "SCT"
    toolkwargs["DetEleCollKey"] = "ITkStripDetectorElementCollection"
    toolkwargs["SiLorentzAngleCondData"] = "SCTSiLorentzAngleCondData"
    acc.setPrivateTools(SiLorentzAngleTool(name="ITkStripLorentzAngleTool", **toolkwargs))
    return acc
