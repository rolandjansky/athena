"""Define methods to configure ITkStripLorentzAngleTool

Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
"""
from AthenaConfiguration.ComponentFactory import CompFactory
from MagFieldServices.MagFieldServicesConfig import MagneticFieldSvcCfg
from SCT_ConditionsTools.ITkStripConditionsToolsConfig import ITkStripSiliconConditionsCfg
from StripGeoModelXml.ITkStripGeoModelConfig import ITkStripReadoutGeometryCfg


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
    acc.merge(ITkStripReadoutGeometryCfg(flags))
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
            algkwargs["SiConditionsTool"] = acc.popToolsAndMerge(ITkStripSiliconConditionsCfg(flags, **sikwargs))
    #Specify correct DetElCollection for ITkStrip
    algkwargs["SCTDetEleCollKey"] = "ITkStripDetectorElementCollection"
    acc.addCondAlgo(CompFactory.SCTSiLorentzAngleCondAlg(name, **algkwargs))

    # Condition tool
    toolkwargs = {}
    #Update ti ITkStrip eventually - tool assumes Pixel or SCT for the moment
    toolkwargs["DetectorName"] = "SCT"
    toolkwargs["DetEleCollKey"] = "ITkStripDetectorElementCollection"
    toolkwargs["SiLorentzAngleCondData"] = "SCTSiLorentzAngleCondData"
    acc.setPrivateTools(CompFactory.SiLorentzAngleTool(name="ITkStripLorentzAngleTool", **toolkwargs))
    return acc
