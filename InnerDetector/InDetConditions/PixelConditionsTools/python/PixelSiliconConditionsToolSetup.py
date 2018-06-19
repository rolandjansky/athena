# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration

class PixelSiliconConditionsToolSetup:
    "Class to simplify setup of PixelSiliconConditionsTool and required conditions algorithms"

    def __init__(self):
        self.hvAlgName = "PixelDCSCondHVAlg"
        self.tempAlgName = "PixelDCSCondTempAlg"

        self.hvAlg = None
        self.tempAlg = None

        self.toolName = "PixelSiliconConditionsTool"
        self.tool = None

        self.dcsTool = None

        self.useDB = True

    def getHVAlgName(self):
        return self.hvAlgName

    def getTempAlgName(self):
        return self.tempAlgName

    def getHVAlg(self):
        return self.hvAlg

    def getTempAlg(self):
        return self.tempAlg

    def getToolName(self):
        return self.toolName

    def setToolName(self, toolName):
        self.toolName = toolName

    def getTool(self):
        return self.tool

    def setDcsTool(self, dcsTool):
        self.dcsTool = dcsTool

    def getUseDB(self):
        return self.useDB

    def setAlgs(self):
        from AthenaCommon.AlgSequence import AthSequencer
        condSeq = AthSequencer("AthCondSeq")

        if not hasattr(condSeq, self.hvAlgName):
            from PixelConditionsAlgorithms.PixelConditionsAlgorithmsConf import PixelDCSCondHVAlg
            if self.dcsTool is None:
                condSeq += PixelDCSCondHVAlg(name = self.hvAlgName)
            else:
                condSeq += PixelDCSCondHVAlg(name = self.hvAlgName, UseState = self.dcsTool.ReadAllDBFolders, DCSConditionsTool = self.dcsTool)
        self.hvAlg = getattr(condSeq, self.hvAlgName)

        if not hasattr(condSeq, self.tempAlgName):
            from PixelConditionsAlgorithms.PixelConditionsAlgorithmsConf import PixelDCSCondTempAlg
            if self.dcsTool is None:
                condSeq += PixelDCSCondTempAlg(name = self.tempAlgName)
            else:
                condSeq += PixelDCSCondTempAlg(name = self.tempAlgName, UseState = self.dcsTool.ReadAllDBFolders, DCSConditionsTool = self.dcsTool)
        self.tempAlg = getattr(condSeq, self.tempAlgName)

    def setTool(self):
        from AthenaCommon.AppMgr import ToolSvc
        if not hasattr(ToolSvc, self.toolName):
            from PixelConditionsTools.PixelConditionsToolsConf import PixelSiliconConditionsTool
            ToolSvc += PixelSiliconConditionsTool(name = self.toolName, UseDB = self.useDB)
        self.tool = getattr(ToolSvc, self.toolName)

    def setUseDB(self, useDB):
        self.useDB = useDB

    def setup(self):
        if self.useDB:
            self.setAlgs()
        self.setTool()
        self.tool.UseDB = self.useDB
