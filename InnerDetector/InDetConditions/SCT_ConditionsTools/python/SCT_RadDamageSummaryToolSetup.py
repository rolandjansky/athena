# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration

class SCT_RadDamageSummaryToolSetup:
    "Class to simplify setup of SCT_RadDamageSummaryTool and required conditions algorithms"

    def __init__(self):
        self.toolName = "InDetSCT_RadDamageSummaryTool"
        self.tool = None

    def setTool(self):
        from AthenaCommon.AppMgr import ToolSvc
        if not hasattr(ToolSvc, self.toolName):
            from SCT_ConditionsTools.SCT_ConditionsToolsConf import SCT_RadDamageSummaryTool
            ToolSvc += SCT_RadDamageSummaryTool(name = self.toolName)
        self.tool = getattr(ToolSvc, self.toolName)

    def getTool(self):
        return self.tool

    def getToolName(self):
        return self.toolName

    def setToolName(self, toolName):
        self.toolName = toolName

    def setup(self):
        self.setTool()
