# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration

class SCT_MonitorConditionsToolSetup:
    "Class to simplify setup of SCT_MonitorConditionsTool and required conditions algorithms"

    def __init__(self):
        self.folder = "/SCT/Derived/Monitoring"
        self.folderDb = None
        self.dbInstance = "SCT_OFL"
        self.algName = "SCT_MonitorCondAlg"
        self.alg = None
        self.toolName = "InDetSCT_MonitorConditionsTool"
        self.tool = None
        self.outputLevel = None

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

    def setFolder(self):
        from IOVDbSvc.CondDB import conddb
        if not conddb.folderRequested(self.folder):
            if self.folderDb is None:
                self.folderDb = self.folder
            conddb.addFolder(self.dbInstance, self.folderDb, className="CondAttrListCollection")

    def setAlg(self):
        from AthenaCommon.AlgSequence import AthSequencer
        condSeq = AthSequencer("AthCondSeq")
        if not hasattr(condSeq, self.algName):
            from SCT_ConditionsAlgorithms.SCT_ConditionsAlgorithmsConf import SCT_MonitorCondAlg
            condSeq += SCT_MonitorCondAlg(name = self.algName,
                                          ReadKey = self.folder)
        self.alg = getattr(condSeq, self.algName)

    def setTool(self):
        if self.tool is None:
            from SCT_ConditionsTools.SCT_ConditionsToolsConf import SCT_MonitorConditionsTool
            if self.outputLevel is None:
                self.tool = SCT_MonitorConditionsTool(name = self.toolName)
            else:
                self.tool = SCT_MonitorConditionsTool(name = self.toolName,
                                                      OutputLevel = self.outputLevel)

    def getTool(self):
        return self.tool

    def setToolName(self, toolName):
        self.toolName = toolName

    def getToolName(self):
        return self.toolName

    def setOutputLevel(self, outputLevel):
        self.outputLevel = outputLevel

    def setup(self):
        self.setFolder()
        self.setAlg()
        self.setTool()
