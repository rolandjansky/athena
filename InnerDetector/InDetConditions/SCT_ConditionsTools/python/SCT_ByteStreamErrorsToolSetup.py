# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration

class SCT_ByteStreamErrorsToolSetup:
    "Class to simplify setup of SCT_ByteStreamErrorsTool"

    def __init__(self):
        self.configTool = None
        self.toolName = "SCT_ByteStreamErrorsTool"
        self.tool = None

    def setTool(self):
        if self.tool is None:
            from SCT_ConditionsTools.SCT_ConditionsToolsConf import SCT_ByteStreamErrorsTool
            if self.configTool is None:
                self.tool = SCT_ByteStreamErrorsTool(name = self.toolName)
            else:
                self.tool = SCT_ByteStreamErrorsTool(name = self.toolName,
                                                     ConfigTool = self.configTool)

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
