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
            from SiPropertiesTool.SiPropertiesToolConf import SCTSiPropertiesCondAlg
            if self.siliconTool is None:
                condSeq += SCTSiPropertiesCondAlg(name = self.algName)
            else:
                condSeq += SCTSiPropertiesCondAlg(name = self.algName,
                                                  SiConditionsTool = self.siliconTool)
        self.alg = getattr(condSeq, self.algName)

    def setTool(self):
        if self.tool is None:
            from SiPropertiesTool.SiPropertiesToolConf import SiPropertiesTool
            self.tool = SiPropertiesTool(name = self.toolName,
                                         DetectorName = "SCT",
                                         ReadKey = "SCTSiliconPropertiesVector")

    def setup(self):
        self.setAlg()
        self.setTool()
