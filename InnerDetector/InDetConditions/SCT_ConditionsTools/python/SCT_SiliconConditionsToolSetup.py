# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

class SCT_SiliconConditionsToolSetup:
    "Class to simplify setup of SCT_SiliconConditionsTool and required conditions algorithms"

    def __init__(self):
        self.hvAlgName = "SCT_SiliconHVCondAlg"
        self.tempAlgName = "SCT_SiliconTempCondAlg"

        self.hvAlg = None
        self.tempAlg = None

        self.toolName = "SCT_SiliconConditionsTool"
        self.tool = None

        self.dcsTool = None

        self.useDB = True
        self.forceUseGeoModel = False

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
            from SCT_ConditionsAlgorithms.SCT_ConditionsAlgorithmsConf import SCT_SiliconHVCondAlg
            if self.dcsTool is None:
                condSeq += SCT_SiliconHVCondAlg(name = self.hvAlgName)
            else:
                condSeq += SCT_SiliconHVCondAlg(name = self.hvAlgName,
                                                UseState = self.dcsTool.ReadAllDBFolders,
                                                DCSConditionsTool = self.dcsTool)
        self.hvAlg = getattr(condSeq, self.hvAlgName)

        if not hasattr(condSeq, self.tempAlgName):
            from SCT_ConditionsAlgorithms.SCT_ConditionsAlgorithmsConf import SCT_SiliconTempCondAlg
            if self.dcsTool is None:
                condSeq += SCT_SiliconTempCondAlg(name = self.tempAlgName)
            else:
                condSeq += SCT_SiliconTempCondAlg(name = self.tempAlgName,
                                                  UseState = self.dcsTool.ReadAllDBFolders,
                                                  DCSConditionsTool = self.dcsTool)
        self.tempAlg = getattr(condSeq, self.tempAlgName)

    def setTool(self):
        if self.tool is None:
            from SCT_ConditionsTools.SCT_ConditionsToolsConf import SCT_SiliconConditionsTool
            self.tool = SCT_SiliconConditionsTool(name = self.toolName,
                                                  UseDB = self.useDB,
                                                  ForceUseGeoModel = self.forceUseGeoModel)

    def setUseDB(self, useDB):
        self.useDB = useDB

    def setForceUseGeoModel(self, forceUseGeoModel):
        self.forceUseGeoModel = forceUseGeoModel

    def setup(self):
        if self.useDB:
            self.setAlgs()
        self.setTool()
        self.tool.UseDB = self.useDB
