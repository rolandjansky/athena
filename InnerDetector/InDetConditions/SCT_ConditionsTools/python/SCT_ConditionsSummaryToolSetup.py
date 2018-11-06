# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration

def singleton(cls):
    # Create only one instance for one toolName to use the same configuration
    instances = {} # Dictionary: key: toolName, value instance
    def wrapper(toolName="InDetSCT_ConditionsSummaryTool"):
        if (cls, toolName) not in instances:
            instances[(cls, toolName)] = cls(toolName)
        return instances[(cls, toolName)]
    return wrapper

@singleton
class SCT_ConditionsSummaryToolSetup:
    # Class to simplify setup of SCT_ConditionsSummaryTool

    def __init__(self, toolName):
        self.toolName = toolName
        self.tool = None

    def setTool(self):
        if self.tool is None:
            from SCT_ConditionsTools.SCT_ConditionsToolsConf import SCT_ConditionsSummaryTool
            self.tool = SCT_ConditionsSummaryTool(name = self.toolName)

    def getTool(self):
        return self.tool

    def getToolName(self):
        return self.toolName

    def setup(self):
        self.setTool()
