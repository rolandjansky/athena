# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration

class SCT_MonitorConditionsToolSetup:
    "Class to simplify setup of SCT_MonitorConditionsTool and required conditions algorithms"

    def __init__(self):
        self.folder = "/SCT/Derived/Monitoring"
        self.folderDb = None
        self.dbInstance = "SCT_OFL"
        self.algName = "SCT_MonitorConditionsCondAlg"
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

    def setFolders(self):
        from IOVDbSvc.CondDB import conddb
        if not conddb.folderRequested(self.folder):
            if self.folderDb is None:
                self.folderDb = self.folder
            conddb.addFolder(self.dbInstance, self.folderDb, className="CondAttrListCollection")

    def setAlgs(self):
        from AthenaCommon.AlgSequence import AthSequencer
        condSeq = AthSequencer("AthCondSeq")
        if not hasattr(condSeq, self.algName):
            from SCT_ConditionsAlgorithms.SCT_ConditionsAlgorithmsConf import SCT_MonitorConditionsCondAlg
            condSeq += SCT_MonitorConditionsCondAlg(name = self.algName,
                                              ReadKey = self.folder)
        self.alg = getattr(condSeq, self.algName)

    def setTool(self):
        from AthenaCommon.AppMgr import ToolSvc
        if not hasattr(ToolSvc, self.toolName):
            from SCT_ConditionsTools.SCT_ConditionsToolsConf import SCT_MonitorConditionsTool
            if self.outputLevel is None:
                ToolSvc += SCT_MonitorConditionsTool(name = self.toolName)
            else:
                ToolSvc += SCT_MonitorConditionsTool(name = self.toolName,
                                                     OutputLevel = self.outputLevel)
        self.tool = getattr(ToolSvc, self.toolName)

    def getTool(self):
        return self.tool

    def setToolName(self, toolName):
        self.toolName = toolName

    def getToolName(self):
        return self.toolName

    def setOutputLevel(self, outputLevel):
        self.outputLevel = outputLevel

    def setup(self):
        self.setFolders()
        self.setAlgs()
        self.setTool()
