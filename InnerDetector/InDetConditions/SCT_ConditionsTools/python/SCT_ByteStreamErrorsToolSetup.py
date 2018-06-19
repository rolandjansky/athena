# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration

class SCT_ByteStreamErrorsToolSetup:
    "Class to simplify setup of SCT_ByteStreamErrorsTool"

    def __init__(self):
        self.configTool = None
        self.toolName = "SCT_ByteStreamErrorsTool"
        self.tool = None

    def setTool(self):
        from AthenaCommon.AppMgr import ToolSvc
        if not hasattr(ToolSvc, self.toolName):
            from SCT_ConditionsTools.SCT_ConditionsToolsConf import SCT_ByteStreamErrorsTool
            if self.configTool is None:
                ToolSvc += SCT_ByteStreamErrorsTool(name = self.toolName)
            else:
                ToolSvc += SCT_ByteStreamErrorsTool(name = self.toolName,
                                                    ConfigTool = self.configTool)
        self.tool = getattr(ToolSvc, self.toolName)

    def getTool(self):
        return self.tool

    def setConfigTool(self, configTool):
        self.configTool = configTool

    def getToolName(self):
        return self.toolName

    def setToolName(self, toolName):
        self.toolName = toolName

    def setup(self):
        self.setTool()
