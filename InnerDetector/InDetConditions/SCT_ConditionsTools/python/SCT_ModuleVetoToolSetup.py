# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration

class SCT_ModuleVetoToolSetup:
    "Class to simplify setup of SCT_ModuleVetoTool and required conditions algorithms"

    def __init__(self):
        self.folder = "/SCT/Manual/BadModules"
        self.folderDb = None
        self.folderTag = None
        self.dbInstance = "SCT_OFL"
        self.algName = "SCT_ModuleVetoCondAlg"
        self.alg = None
        self.toolName = "InDetSCT_ModuleVetoTool"
        self.tool = None
        self.useDB = True

    def getFolder(self):
        return self.folder

    def getFolderDb(self):
        return self.folderDb

    def setFolderTag(self, folderTag):
        self.folderTag = folderTag

    def getFolderTag(self):
        return self.folderTag

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
            if self.folderTag is None:
                conddb.addFolder(self.dbInstance, self.folderDb, className="AthenaAttributeList")
            else:
                conddb.addFolderWithTag(self.dbInstance, self.folderDb, self.folderTag, className="AthenaAttributeList")

    def setAlgs(self):
        from AthenaCommon.AlgSequence import AthSequencer
        condSeq = AthSequencer("AthCondSeq")
        if not hasattr(condSeq, self.algName):
            from SCT_ConditionsAlgorithms.SCT_ConditionsAlgorithmsConf import SCT_ModuleVetoCondAlg
            condSeq += SCT_ModuleVetoCondAlg(name = self.algName,
                                              ReadKey = self.folder)
        self.alg = getattr(condSeq, self.algName)

    def setTool(self):
        if self.tool is None:
            from SCT_ConditionsTools.SCT_ConditionsToolsConf import SCT_ModuleVetoTool
            self.tool = SCT_ModuleVetoTool(name = self.toolName)

    def getTool(self):
        return self.tool

    def getToolName(self):
        return self.toolName

    def setToolName(self, toolName):
        self.toolName = toolName

    def getUseDB(self):
        return self.useDB

    def setUseDB(self, useDB):
        self.useDB = useDB

    def setup(self):
        if self.useDB:
            self.setFolders()
            self.setAlgs()
        self.setTool()
