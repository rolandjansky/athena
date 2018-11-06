# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration

class SCT_ReadCalibDataToolSetup:
    "Class to simplify setup of SCT_ReadCalibDataTool and required conditions algorithms"

    def __init__(self):
        self.noiseFolder = "/SCT/DAQ/Calibration/NoiseOccupancyDefects"
        self.gainFolder = "/SCT/DAQ/Calibration/NPtGainDefects"
        self.algName = "SCT_ReadCalibDataCondAlg"
        self.alg = None
        self.toolName = "InDetSCT_ReadCalibDataTool"
        self.tool = None

    def getNoiseFolder(self):
        return self.noiseFolder

    def getGainFolder(self):
        return self.gainFolder

    def getAlgName(self):
        return self.algName

    def getAlg(self):
        return self.alg

    def setFolders(self):
        from IOVDbSvc.CondDB import conddb
        if not conddb.folderRequested(self.noiseFolder):
            conddb.addFolderSplitMC("SCT", self.noiseFolder, self.noiseFolder, className="CondAttrListCollection")
        if not conddb.folderRequested(self.gainFolder):
            conddb.addFolderSplitMC("SCT", self.gainFolder, self.gainFolder, className="CondAttrListCollection")

    def setAlgs(self):
        from AthenaCommon.AlgSequence import AthSequencer
        condSeq = AthSequencer("AthCondSeq")
        if not hasattr(condSeq, self.algName):
            from SCT_ConditionsAlgorithms.SCT_ConditionsAlgorithmsConf import SCT_ReadCalibDataCondAlg
            condSeq += SCT_ReadCalibDataCondAlg(name = self.algName,
                                                ReadKeyGain = self.gainFolder,
                                                ReadKeyNoise = self.noiseFolder)
        self.alg = getattr(condSeq, self.algName)

    def setTool(self):
        if self.tool is None:
            from SCT_ConditionsTools.SCT_ConditionsToolsConf import SCT_ReadCalibDataTool
            self.tool = SCT_ReadCalibDataTool(name = self.toolName)

    def getTool(self):
        return self.tool

    def getToolName(self):
        return self.toolName

    def setToolName(self, toolName):
        self.toolName = toolName

    def setup(self):
        self.setFolders()
        self.setAlgs()
        self.setTool()
