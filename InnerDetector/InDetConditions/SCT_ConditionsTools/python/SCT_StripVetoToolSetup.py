# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration

class SCT_StripVetoToolSetup:
    "Class to simplify setup of SCT_StripVetoTool and required conditions algorithms"

    def __init__(self):
        self.toolName = "SCT_StripVetoTool"
        self.tool = None

    def setTool(self):
        if self.tool is None:
            from SCT_ConditionsTools.SCT_ConditionsToolsConf import SCT_StripVetoTool
            self.tool = SCT_StripVetoTool(name = self.toolName)

    def getTool(self):
        return self.tool

    def setup(self):
        self.setTool()
