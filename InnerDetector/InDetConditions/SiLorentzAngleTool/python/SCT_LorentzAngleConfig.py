"""Define methods to configure SCTLorentzAngleTool

Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
"""
from AthenaCommon import Logging
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from SiLorentzAngleTool.SiLorentzAngleToolConf import SiLorentzAngleTool
from SiLorentzAngleTool.SiLorentzAngleToolConf import SCTSiLorentzAngleCondAlg
from SCT_ConditionsTools.SCT_DCSConditionsConfig import SCT_DCSConditionsCfg
from SCT_ConditionsTools.SCT_SiliconConditionsConfig import SCT_SiliconConditionsCfg
from SCT_ConditionsTools.SCT_SiliconConditionsConfig import SCT_SiliconConditionsToolCfg
from MagFieldServices.MagFieldServicesConfig import MagneticFieldSvcCfg

def SCT_LorentzAngleToolCfg(flags, name="SCT_LorentzAngleTool", **kwargs):
    """Return a SiLorentzAngleTool configured for SCT"""
    kwargs.setdefault("DetectorName", "SCT")
    kwargs.setdefault("SiLorentzAngleCondData", "SCTSiLorentzAngleCondData")
    kwargs.setdefault("UseMagFieldSvc", True)
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
    acc, svc = MagneticFieldSvcCfg(flags)
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
        acc.merge(DCSAcc)
        SCAcc = SCT_SiliconConditionsCfg(flags, DCSConditionsTool=DCSAcc.popPrivateTools())
    else:
        SCTool = SCT_SiliconConditionsToolCfg(flags, UseDB=False, ForceUseGeoModel=True)
        SCAcc = SCT_SiliconConditionsCfg(flags, SiliconConditionsTool=SCTool)
    SCAcc.popPrivateTools()
    acc.merge(SCAcc)
    # set up SCTSiLorentzAngleCondAlg
    kwargs.setdefault("UseMagFieldSvc", tool.UseMagFieldSvc)
    kwargs.setdefault("UseMagFieldDcs", not flags.Common.isOnline)
    kwargs.setdefault("UseGeoModel", forceUseGeoModel)
    kwargs.setdefault("useSctDefaults", False)
    acc.addCondAlgo(SCTSiLorentzAngleCondAlg(name, **kwargs))
    acc.setPrivateTools(tool)
    return acc

