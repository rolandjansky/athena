# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration

class SCT_ReadCalibChipDataToolSetup:
    "Class to simplify setup of SCT_ReadCalibChipDataTool and required conditions algorithms"

    def __init__(self):
        self.noiseFolder = "/SCT/DAQ/Calibration/ChipNoise"
        self.gainFolder = "/SCT/DAQ/Calibration/ChipGain"
        self.noiseFolderTag = None
        self.gainFolderTag = None
        self.noiseAlgName = "SCT_ReadCalibChipNoiseCondAlg"
        self.gainAlgName = "SCT_ReadCalibChipGainCondAlg"
        self.noiseAlg = None
        self.gainAlg = None
        self.toolName = "InDetSCT_ReadCalibChipDataTool"
        self.tool = None

    def getNoiseFolder(self):
        return self.noiseFolder

    def getGainFolder(self):
        return self.gainFolder

    def getNoiseFolderTag(self):
        return self.noiseFolderTag

    def getGainFolderTag(self):
        return self.gainFolderTag

    def setNoiseFolderTag(self, noiseFolderTag):
        self.noiseFolderTag = noiseFolderTag

    def setGainFolderTag(self, gainFolderTag):
        self.gainFolderTag = gainFolderTag

    def getNoiseAlgName(self):
        return self.noiseAlgName

    def getGainAlgName(self):
        return self.gainAlgName

    def getNoiseAlg(self):
        return self.noiseAlg

    def getGainAlg(self):
        return self.gainAlg

    def setFolders(self):
        from IOVDbSvc.CondDB import conddb
        if not conddb.folderRequested(self.noiseFolder):
            if self.noiseFolderTag is None:
                conddb.addFolderSplitMC("SCT", self.noiseFolder, self.noiseFolder, className="CondAttrListCollection")
            else:
                conddb.addFolderWithTag("SCT", self.noiseFolder, self.noiseFolderTag, className="CondAttrListCollection")
        if not conddb.folderRequested(self.gainFolder):
            if self.gainFolderTag is None:
                conddb.addFolderSplitMC("SCT", self.gainFolder, self.gainFolder, className="CondAttrListCollection")
            else:
                conddb.addFolderWithTag("SCT", self.gainFolder, self.gainFolderTag, className="CondAttrListCollection")

    def setAlgs(self):
        from AthenaCommon.AlgSequence import AthSequencer
        condSeq = AthSequencer("AthCondSeq")
        if not hasattr(condSeq, self.noiseAlgName):
            from SCT_ConditionsAlgorithms.SCT_ConditionsAlgorithmsConf import SCT_ReadCalibChipNoiseCondAlg
            condSeq += SCT_ReadCalibChipNoiseCondAlg(name = self.noiseAlgName,
                                                     ReadKey=self.noiseFolder)
        self.noiseAlg = getattr(condSeq, self.noiseAlgName)
        if not hasattr(condSeq, self.gainAlgName):
            from SCT_ConditionsAlgorithms.SCT_ConditionsAlgorithmsConf import SCT_ReadCalibChipGainCondAlg
            condSeq += SCT_ReadCalibChipGainCondAlg(name = self.gainAlgName,
                                                    ReadKey=self.gainFolder)
        self.gainAlg = getattr(condSeq, self.gainAlgName)

    def setTool(self):
        if self.tool is None:
            from SCT_ConditionsTools.SCT_ConditionsToolsConf import SCT_ReadCalibChipDataTool
            self.tool = SCT_ReadCalibChipDataTool(name = self.toolName)

    def getTool(self):
        return self.tool

    def setToolName(self, toolName):
        self.toolName = toolName

    def getToolName(self):
        return self.toolName

    def setup(self):
        self.setFolders()
        self.setAlgs()
        self.setTool()
