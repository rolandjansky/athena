# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration

class SCT_ConditionsParameterToolSetup:
    "Class to simplify setup of SCT_ConditionsParameterTool and required conditions algorithms"

    def __init__(self):
        self.folder = "/SCT/DAQ/Configuration/Chip"
        self.folderDb = None
        self.dbInstance = "SCT"
        self.algName = "SCT_ConditionsParameterCondAlg"
        self.alg = None
        self.toolName = "SCT_ConditionsParameterTool"
        self.tool = None

    def getFolder(self):
        return self.folder

    def getFolderDb(self):
        return self.folderDb

    def setFolderDb(self, folderDb):
        self.folderDb = folderDb

    def getDbInstance(self):
        return self.dbInstance

    def setDbInstance(self, dbInstance):
        self.dbInstance = dbInstance

    def getAlgName(self):
        return self.algName

    def getAlg(self):
        return self.alg

    def setFolders(self):
        from IOVDbSvc.CondDB import conddb
        if not conddb.folderRequested(self.folder):
            if self.folderDb is None:
                self.folderDb = self.folder
            conddb.addFolder(self.dbInstance, self.folderDb, className="CondAttrListVec")

    def setAlgs(self):
        from AthenaCommon.AlgSequence import AthSequencer
        condSeq = AthSequencer("AthCondSeq")
        if not hasattr(condSeq, self.algName):
            from SCT_ConditionsAlgorithms.SCT_ConditionsAlgorithmsConf import SCT_ConditionsParameterCondAlg
            condSeq += SCT_ConditionsParameterCondAlg(name = self.algName,
                                                      ReadKey = self.folder)
        self.alg = getattr(condSeq, self.algName)

    def setTool(self):
        if self.tool is None:
            from SCT_ConditionsTools.SCT_ConditionsToolsConf import SCT_ConditionsParameterTool
            self.tool = SCT_ConditionsParameterTool(name = self.toolName)

    def getTool(self):
        return self.tool

    def setup(self):
        self.setFolders()
        self.setAlgs()
        self.setTool()
