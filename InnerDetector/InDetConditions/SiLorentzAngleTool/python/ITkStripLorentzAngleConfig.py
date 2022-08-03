"""Define methods to configure ITkStripLorentzAngleTool

Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
"""
from AthenaConfiguration.ComponentFactory import CompFactory
from MagFieldServices.MagFieldServicesConfig import AtlasFieldCacheCondAlgCfg
from SCT_ConditionsTools.ITkStripConditionsToolsConfig import ITkStripSiliconConditionsCfg
from StripGeoModelXml.ITkStripGeoModelConfig import ITkStripReadoutGeometryCfg

def ITkStripLorentzAngleToolCfg(flags, name="ITkStripLorentzAngleTool", **kwargs):
    """Return a SiLorentzAngleTool configured for ITk Strip"""
    acc = ITkStripLorentzAngleCondAlgCfg(flags)
    #Update to ITkStrip eventually - tool assumes Pixel or SCT for the moment
    kwargs.setdefault("DetectorName", "SCT")
    kwargs.setdefault("SiLorentzAngleCondData", "SCTSiLorentzAngleCondData")
    kwargs.setdefault("DetEleCollKey", "ITkStripDetectorElementCollection")
    acc.setPrivateTools(CompFactory.SiLorentzAngleTool(name="ITkStripLorentzAngleTool", **kwargs))
    return acc

def ITkStripLorentzAngleCondAlgCfg(flags, name="ITkStripSiLorentzAngleCondAlg",
                                   forceUseGeoModel=False, **kwargs):
    # Condition algorithm
    # construct with field services
    acc = AtlasFieldCacheCondAlgCfg(flags)
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
    return acc
