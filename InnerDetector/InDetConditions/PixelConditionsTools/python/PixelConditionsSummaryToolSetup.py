# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration

class PixelConditionsSummaryToolSetup:
    "Class to simplify setup of PixelConditionsSummaryTool and required conditions algorithms"

    def __init__(self):
        self.toolName = "InDetPixelConditionsSummaryTool"
        self.tool = None

    def setTool(self):
        from AthenaCommon.AppMgr import ToolSvc
        if not hasattr(ToolSvc, self.toolName):
            from PixelConditionsTools.PixelConditionsToolsConf import PixelConditionsSummaryTool
            ToolSvc += PixelConditionsSummaryTool(name = self.toolName)
        self.tool = getattr(ToolSvc, self.toolName)

    def getTool(self):
        return self.tool

    def getToolName(self):
        return self.toolName

    def setToolName(self, toolName):
        self.toolName = toolName

    def setup(self):
        self.setTool()
