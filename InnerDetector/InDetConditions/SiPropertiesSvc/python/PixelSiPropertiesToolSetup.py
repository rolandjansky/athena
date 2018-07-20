# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration

class PixelSiPropertiesToolSetup:
    "Class to simplify setup of PixelSiPropertiesTool and required conditions algorithm"

    def __init__(self):
        self.algName = "PixelSiPropertiesCondAlg"
        self.alg = None
        self.toolName = "PixelSiPropertiesTool"
        self.tool = None
#        self.siliconTool = None

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

#    def setSiliconTool(self, siliconTool):
#        self.siliconTool = siliconTool

    def setAlg(self):
        from AthenaCommon.AppMgr import ToolSvc
        if not hasattr(ToolSvc, "PixelDCSConditionsTool"):
            from PixelConditionsTools.PixelDCSConditionsToolSetup import PixelDCSConditionsToolSetup
            pixelDCSConditionsToolSetup = PixelDCSConditionsToolSetup()
            pixelDCSConditionsToolSetup.setup()

        from AthenaCommon.AlgSequence import AthSequencer
        condSeq = AthSequencer("AthCondSeq")
        if not hasattr(condSeq, self.algName):
            from SiPropertiesSvc.SiPropertiesSvcConf import PixelSiPropertiesCondAlg
            condSeq += PixelSiPropertiesCondAlg(name = self.algName, PixelDCSConditionsTool=ToolSvc.PixelDCSConditionsTool)
        self.alg = getattr(condSeq, self.algName)

    def setTool(self):
        from AthenaCommon.AppMgr import ToolSvc
        if not hasattr(ToolSvc, self.toolName):
            from SiPropertiesSvc.SiPropertiesSvcConf import SiPropertiesTool
            ToolSvc += SiPropertiesTool(name = self.toolName,
                                        DetectorName = "Pixel",
                                        ReadKey = "PixelSiliconPropertiesVector")
        self.tool = getattr(ToolSvc, self.toolName)

    def setup(self):
        self.setAlg()
        self.setTool()
