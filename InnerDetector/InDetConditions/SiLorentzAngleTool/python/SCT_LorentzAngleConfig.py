"""Define methods to configure SCTLorentzAngleTool

Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
"""
from AthenaCommon import Logging
from AthenaConfiguration.ComponentFactory import CompFactory
SiLorentzAngleTool=CompFactory.SiLorentzAngleTool
SCTSiLorentzAngleCondAlg=CompFactory.SCTSiLorentzAngleCondAlg
from SCT_ConditionsTools.SCT_DCSConditionsConfig import SCT_DCSConditionsCfg
from SCT_ConditionsTools.SCT_SiliconConditionsConfig import SCT_SiliconConditionsCfg
from SCT_GeoModel.SCT_GeoModelConfig import SCT_GeometryCfg
from MagFieldServices.MagFieldServicesConfig import MagneticFieldSvcCfg

def SCT_LorentzAngleToolCfg(flags, name="SCT_LorentzAngleTool", **kwargs):
    """Return a SiLorentzAngleTool configured for SCT"""
    kwargs.setdefault("DetectorName", "SCT")
    kwargs.setdefault("SiLorentzAngleCondData", "SCTSiLorentzAngleCondData")
    kwargs.setdefault("DetEleCollKey", "SCT_DetectorElementCollection")
    kwargs.setdefault("UseMagFieldCache", True)
    return SiLorentzAngleTool(name, **kwargs)

def SCT_LorentzAngleCfg(flags, name="SCT_SiLorentzAngleCondAlg",
                        forceUseDB=False, forceUseGeoModel=False, **kwargs):
    """Return configured ComponentAccumulator and tool for SCT_LorentzAngle

    SiLorentzAngleTool may be provided in kwargs
    """
    if forceUseDB and forceUseGeoModel:
        msg = Logging.logging.getLogger("SCT_LorentzAngleCfg")
        msg.error("Setting is wrong: both forceUseDB and forceUseGeoModel cannot be True at the same time")
    # construct with field services
    acc = MagneticFieldSvcCfg(flags)
    acc.merge(SCT_GeometryCfg(flags)) # For SCT_DetectorElementCollection used in SCTSiLorentzAngleCondAlg
    tool = kwargs.get("SiLorentzAngleTool", SCT_LorentzAngleToolCfg(flags))
    if not forceUseGeoModel:
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
    # set up SCTSiLorentzAngleCondAlg
    kwargs.setdefault("UseMagFieldCache", tool.UseMagFieldCache)
    kwargs.setdefault("UseMagFieldDcs", not flags.Common.isOnline)
    kwargs.setdefault("UseGeoModel", forceUseGeoModel)
    kwargs.setdefault("useSctDefaults", False)
    acc.addCondAlgo(SCTSiLorentzAngleCondAlg(name, **kwargs))
    acc.setPrivateTools(tool)
    return acc
