# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration

class PixelLorentzAngleToolSetup:
    "Class to simplify setup of PixelLorentzAngleTool"
    def __init__(self):
        from PixelConditionsTools.PixelDCSConditionsToolSetup import PixelDCSConditionsToolSetup
        pixelDCSConditionsToolSetup = PixelDCSConditionsToolSetup()

        # CHECK CHECK!!! For HLT
        from AthenaCommon.AthenaCommonFlags import athenaCommonFlags

        if athenaCommonFlags.isOnline():
            from AthenaCommon.GlobalFlags import globalflags
            pixelDCSConditionsToolSetup.setDbInstance("PIXEL_ONL")
            pixelDCSConditionsToolSetup.setHVFolder("/PIXEL/HLT/DCS/HV")
            pixelDCSConditionsToolSetup.setTempFolder("/PIXEL/HLT/DCS/TEMPERATURE")

        pixelDCSConditionsToolSetup.setup()

        from SiPropertiesSvc.PixelSiPropertiesToolSetup import PixelSiPropertiesToolSetup
        pixelSiPropertiesToolSetup = PixelSiPropertiesToolSetup()
        pixelSiPropertiesToolSetup.setSiliconTool(pixelDCSConditionsToolSetup.getTool())
        pixelSiPropertiesToolSetup.setup()

        # Set up PixelSiLorentzAngleCondAlg
        from AthenaCommon.AlgSequence import AthSequencer
        condSeq = AthSequencer("AthCondSeq")
        if not hasattr(condSeq, "PixelSiLorentzAngleCondAlg"):
            from SiLorentzAngleSvc.SiLorentzAngleSvcConf import PixelSiLorentzAngleCondAlg
            from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
            condSeq += PixelSiLorentzAngleCondAlg(name = "PixelSiLorentzAngleCondAlg",
                                                  PixelDCSConditionsTool = pixelDCSConditionsToolSetup.getTool(),
                                                  SiPropertiesTool = pixelSiPropertiesToolSetup.getTool(),
                                                  UseMagFieldSvc = True,
                                                  UseMagFieldDcs = (not athenaCommonFlags.isOnline()))
        pixelSiLorentzAngleCondAlg = condSeq.PixelSiLorentzAngleCondAlg

        # Set up PixelLorentzAngleTool
        from AthenaCommon.AppMgr import ToolSvc
        if not hasattr(ToolSvc, "PixelLorentzAngleTool"):
            from SiLorentzAngleSvc.SiLorentzAngleSvcConf import SiLorentzAngleTool
            ToolSvc += SiLorentzAngleTool(name="PixelLorentzAngleTool", DetectorName="Pixel", SiLorentzAngleCondData="PixelSiLorentzAngleCondData")
        pixelLorentzAngleTool = ToolSvc.PixelLorentzAngleTool
        # Pass the silicon conditions tool to the Lorentz angle tool
        # Also make sure UseMagFieldTool is True as AtlasGeoModel sets this to False
        # if loaded first.
        pixelLorentzAngleTool.UseMagFieldSvc = True
        self.PixelLorentzAngleTool = pixelLorentzAngleTool

