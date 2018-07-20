# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration

class PixelConditionsSummaryToolSetup:
    "Class to simplify setup of PixelConditionsSummaryTool and required conditions algorithms"

    def __init__(self):
        self.toolName = "PixelConditionsSummaryTool"
        self.tool = None
        self.useDCS = False
        self.useBS = False
        self.useTDAQ = False

    def setUseDCS(self, useDCS):
        self.useDCS = useDCS

    def getUseDCS(self):
        return self.useDCS

    def setUseBS(self, useBS):
        self.useBS = useBS

    def getUseBS(self):
        return self.useBS

    def setUseTDAQ(self, useTDAQ):
        self.useTDAQ = useTDAQ

    def getUseTDAQ(self):
        return self.useTDAQ

    def getTool(self):
        return self.tool

    def getToolName(self):
        return self.toolName

    def setToolName(self, toolName):
        self.toolName = toolName

    def setTool(self):
        from AthenaCommon.AppMgr import ToolSvc
        if not hasattr(ToolSvc, "PixelDCSConditionsTool"):
            from PixelConditionsTools.PixelDCSConditionsToolSetup import PixelDCSConditionsToolSetup
            pixelDCSConditionsToolSetup = PixelDCSConditionsToolSetup()
            pixelDCSConditionsToolSetup.setup()

        if not hasattr(ToolSvc, self.toolName):
            from PixelConditionsTools.PixelConditionsToolsConf import PixelConditionsSummaryTool
            ToolSvc += PixelConditionsSummaryTool(name=self.toolName, PixelDCSConditionsTool=ToolSvc.PixelDCSConditionsTool, UseDCS=self.useDCS, UseByteStream=self.useBS, UseTDAQ=self.useTDAQ)
        self.tool = getattr(ToolSvc, self.toolName)

    def setup(self):
        self.setTool()

