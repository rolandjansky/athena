# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration

class SCT_SiPropertiesToolSetup:
    "Class to simplify setup of SCT_SiPropertiesTool and required conditions algorithm"

    def __init__(self):
        self.algName = "SCTSiPropertiesCondAlg"
        self.alg = None
        self.toolName = "SCT_SiPropertiesTool"
        self.tool = None
        self.siliconTool = None

    def getAlgName(self):
        return self.algName

    def setAlgName(self, algName):
        self.algName = algName

    def getAlg(self):
        return self.alg

    def getToolName(self):
        return self.toolName

    def setToolName(self, toolName):
        self.toolName = toolName

    def getTool(self):
        return self.tool

    def setSiliconTool(self, siliconTool):
        self.siliconTool = siliconTool

    def setAlg(self):
        from AthenaCommon.AlgSequence import AthSequencer
        condSeq = AthSequencer("AthCondSeq")

        if not hasattr(condSeq, self.algName):
            from SiPropertiesSvc.SiPropertiesSvcConf import SCTSiPropertiesCondAlg
            if self.siliconTool is None:
                condSeq += SCTSiPropertiesCondAlg(name = self.algName)
            else:
                condSeq += SCTSiPropertiesCondAlg(name = self.algName,
                                                  SiConditionsTool = self.siliconTool)
        self.alg = getattr(condSeq, self.algName)

    def setTool(self):
        from AthenaCommon.AppMgr import ToolSvc
        if not hasattr(ToolSvc, self.toolName):
            from SiPropertiesSvc.SiPropertiesSvcConf import SiPropertiesTool
            ToolSvc += SiPropertiesTool(name = self.toolName,
                                        DetectorName = "SCT",
                                        ReadKey = "SCTSiliconPropertiesVector")
        self.tool = getattr(ToolSvc, self.toolName)

    def setup(self):
        self.setAlg()
        self.setTool()
