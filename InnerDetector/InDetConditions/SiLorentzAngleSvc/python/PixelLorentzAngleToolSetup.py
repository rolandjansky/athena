# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration

class PixelLorentzAngleToolSetup:
    "Class to simplify setup of PixelLorentzAngleTool"
    def __init__(self):
        from AthenaCommon.AppMgr import ToolSvc
        if not hasattr(ToolSvc, "PixelDCSConditionsTool"):
            from PixelConditionsTools.PixelDCSConditionsToolSetup import PixelDCSConditionsToolSetup
            pixelDCSConditionsToolSetup = PixelDCSConditionsToolSetup()
            pixelDCSConditionsToolSetup.setup()

        if not hasattr(ToolSvc, "PixelSiPropertiesTool"):
            from SiPropertiesSvc.PixelSiPropertiesToolSetup import PixelSiPropertiesToolSetup
            pixelSiPropertiesToolSetup = PixelSiPropertiesToolSetup()
            pixelSiPropertiesToolSetup.setup()

        from AthenaCommon.AlgSequence import AthSequencer
        condSeq = AthSequencer("AthCondSeq")
        if not hasattr(condSeq, "PixelSiLorentzAngleCondAlg"):
            from SiLorentzAngleSvc.SiLorentzAngleSvcConf import PixelSiLorentzAngleCondAlg
            from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
            condSeq += PixelSiLorentzAngleCondAlg(name = "PixelSiLorentzAngleCondAlg",
                                                  PixelDCSConditionsTool = ToolSvc.PixelDCSConditionsTool,
                                                  SiPropertiesTool = ToolSvc.PixelSiPropertiesTool,
                                                  UseMagFieldSvc = True,
                                                  UseMagFieldDcs = (not athenaCommonFlags.isOnline()))
        pixelSiLorentzAngleCondAlg = condSeq.PixelSiLorentzAngleCondAlg

        if not hasattr(ToolSvc, "PixelLorentzAngleTool"):
            from SiLorentzAngleSvc.SiLorentzAngleSvcConf import SiLorentzAngleTool
            ToolSvc += SiLorentzAngleTool(name="PixelLorentzAngleTool", DetectorName="Pixel", SiLorentzAngleCondData="PixelSiLorentzAngleCondData")
        pixelLorentzAngleTool = ToolSvc.PixelLorentzAngleTool
        # Pass the silicon conditions tool to the Lorentz angle tool
        # Also make sure UseMagFieldTool is True as AtlasGeoModel sets this to False
        # if loaded first.
        pixelLorentzAngleTool.UseMagFieldSvc = True
        self.PixelLorentzAngleTool = pixelLorentzAngleTool

