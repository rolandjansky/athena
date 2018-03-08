# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration

class SCT_FlaggedConditionToolSetup:
    "Class to simplify setup of SCT_FlaggedConditionTool and required conditions algorithms"

    def __init__(self):
        self.toolName = "InDetSCT_FlaggedConditionTool"
        self.tool = None

    def setTool(self):
        from AthenaCommon.AppMgr import ToolSvc
        if not hasattr(ToolSvc, self.toolName):
            from SCT_ConditionsServices.SCT_ConditionsServicesConf import SCT_FlaggedConditionTool
            ToolSvc += SCT_FlaggedConditionTool(name = self.toolName)
        self.tool = getattr(ToolSvc, self.toolName)

    def getTool(self):
        return self.tool

    def getToolName(self):
        return self.toolName

    def setToolName(self, toolName):
        self.toolName = toolName

    def setup(self):
        self.setTool()
