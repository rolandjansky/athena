# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration

class SCTLorentzAngleToolSetup:
    "Class to simplify setup of SCTLorentzAngleTool"
    def __init__(self, forceUseDB=False, forceUseGeoModel=False):
        if forceUseDB and forceUseGeoModel:
            from AthenaCommon import Logging
            msg = Logging.logging.getLogger("SCTLorentzAngleToolSetup")
            msg.error("Setting is wrong: both forceUseDB and forceUseGeoModel cannot be True at the same time")
            return

        # Set up SCT_DCSConditiosnSvc if necessary
        if not forceUseGeoModel:
            from SCT_ConditionsServices.SCT_DCSConditionsSvcSetup import SCT_DCSConditionsSvcSetup
            sct_DCSConditionsSvcSetup = SCT_DCSConditionsSvcSetup()
            sct_DCSConditionsSvcSetup.setup()

        # Set up SCT_SiliconConditionsTool
        from SCT_ConditionsTools.SCT_SiliconConditionsToolSetup import SCT_SiliconConditionsToolSetup
        sct_SiliconConditionsToolSetup = SCT_SiliconConditionsToolSetup()
        if forceUseGeoModel:
            sct_SiliconConditionsToolSetup.setUseDB(False)
            sct_SiliconConditionsToolSetup.setForceUseGeoModel(True)
        else:
            sct_SiliconConditionsToolSetup.setDcsSvc(sct_DCSConditionsSvcSetup.getSvc())
        sct_SiliconConditionsToolSetup.setup()
        sctSiliconConditionsTool = sct_SiliconConditionsToolSetup.getTool()
        self.sctSiliconConditionsTool = sctSiliconConditionsTool
        self.SCT_SiliconConditionsTool = sctSiliconConditionsTool

        # Set up SCTSiLorentzAngleCondAlg
        from AthenaCommon.AlgSequence import AthSequencer
        condSeq = AthSequencer("AthCondSeq")
        if not hasattr(condSeq, "SCTSiLorentzAngleCondAlg"):
            from SiLorentzAngleSvc.SiLorentzAngleSvcConf import SCTSiLorentzAngleCondAlg
            from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
            condSeq += SCTSiLorentzAngleCondAlg(name = "SCTSiLorentzAngleCondAlg",
                                                SiConditionsTool = sctSiliconConditionsTool,
                                                UseMagFieldSvc = True,
                                                UseMagFieldDcs = (not athenaCommonFlags.isOnline()),
                                                UseGeoModel = forceUseGeoModel)
        sctSiLorentzAngleCondAlg = condSeq.SCTSiLorentzAngleCondAlg
        if forceUseDB:
            sctSiLorentzAngleCondAlg.useSctDefaults = False

        # Set up SCTLorentzAngleTool
        from AthenaCommon.AppMgr import ToolSvc
        if not hasattr(ToolSvc, "SCTLorentzAngleTool"):
            from SiLorentzAngleSvc.SiLorentzAngleSvcConf import SiLorentzAngleTool
            ToolSvc += SiLorentzAngleTool(name="SCTLorentzAngleTool", DetectorName="SCT")
        sctLorentzAngleTool = ToolSvc.SCTLorentzAngleTool
        # Pass the silicon conditions tool to the Lorentz angle tool
        # Also make sure UseMagFieldTool is True as AtlasGeoModel sets this to False
        # if loaded first.
        sctLorentzAngleTool.UseMagFieldSvc = True
        self.SCTLorentzAngleTool = sctLorentzAngleTool

    # Force the Lorentz angle tool to use SiliconConditions service (which are assumed to use the DB)
    # Default is to decide based on GeoModel.
    def forceUseDB(self) :
        "Force usage of conditions DB"
        self.SCT_SiliconConditionsTool.CheckGeoModel = False
        from AthenaCommon import Logging
        msg = Logging.logging.getLogger("SCTLorentzAngleToolSetup")
        msg.warning("Please set forceUseDB in constructor. Unnecessary service, algorithms, folders are configured")

    # Force to use the defaults from GeoModel. In case it is not possible to use DCS
    def forceUseGeoModel(self) :
        "Force usage of GeoModel defaults"
        self.SCT_SiliconConditionsTool.ForceUseGeoModel = True
        msg.warning("Please set forceUseGeoModel in constructor. Unnecessary service, algorithms, folders are configured")
