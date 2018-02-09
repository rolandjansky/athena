# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration

class SCT_ConfigurationConditionsSvcSetup:
    "Class to simplify setup of SCT_ConfigurationConditionsSvcSetup and required conditions algorithms"

    def __init__(self):
        self.channelFolder = "/SCT/DAQ/Config/Chip"
        self.moduleFolder = "/SCT/DAQ/Config/Module"
        self.murFolder = "/SCT/DAQ/Config/MUR"
        self.channelFolderDb = None
        self.moduleFolderDb = None
        self.murFolderDb = None
        self.dbInstance = "SCT"
        self.algName = "SCT_ConfigurationCondAlg"
        self.alg = None
        self.svcName = "InDetSCT_ConfigurationConditionsSvc"
        self.svc = None

    def getChannelFolder(self):
        return self.channelFolder

    def getModuleFolder(self):
        return self.moduleFolder

    def getMurFolder(self):
        return self.murFolder

    def setChannelFolder(self, channelFolder):
        self.channelFolder = channelFolder

    def setModuleFolder(self, moduleFolder):
        self.moduleFolder = moduleFolder

    def setMurFolder(self, murFolder):
        self.murFolder = murFolder

    def getChannelFolderDb(self):
        return self.channelFolderDb

    def getModuleFolderDb(self):
        return self.moduleFolderDb

    def getMurFolderDb(self):
        return self.murFolderDb

    def setChannelFolderDb(self, channelFolderDb):
        self.channelFolderDb = channelFolderDb

    def setModuleFolderDb(self, moduleFolderDb):
        self.moduleFolderDb = moduleFolderDb

    def setMurFolderDb(self, murFolderDb):
        self.murFolderDb = murFolderDb

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
        if not conddb.folderRequested(self.channelFolder):
            if self.channelFolderDb is None:
                conddb.addFolderSplitMC(self.dbInstance, self.channelFolder, self.channelFolder, className="CondAttrListVec")
            else:
                conddb.addFolder(self.dbInstance, self.channelFolderDb, className="CondAttrListVec")
        if not conddb.folderRequested(self.moduleFolder):
            if self.moduleFolderDb is None:
                conddb.addFolderSplitMC(self.dbInstance, self.moduleFolder, self.moduleFolder, className="CondAttrListVec")
            else:
                conddb.addFolder(self.dbInstance, self.moduleFolderDb, className="CondAttrListVec")
        if not conddb.folderRequested(self.murFolder):
            if self.murFolderDb is None:
                conddb.addFolderSplitMC(self.dbInstance, self.murFolder, self.murFolder, className="CondAttrListVec")
            else:
                conddb.addFolder(self.dbInstance, self.murFolderDb, className="CondAttrListVec")

    def setAlgs(self):
        from AthenaCommon.AlgSequence import AthSequencer
        condSeq = AthSequencer("AthCondSeq")
        if not hasattr(condSeq, self.algName):
            from SCT_ConditionsServices.SCT_ConditionsServicesConf import SCT_ConfigurationCondAlg
            condSeq += SCT_ConfigurationCondAlg(name = self.algName,
                                                ReadKeyChannel = self.channelFolder,
                                                ReadKeyModule = self.moduleFolder,
                                                ReadKeyMur = self.murFolder)
        self.alg = getattr(condSeq, self.algName)

    def setSvc(self):
        from AthenaCommon.AppMgr import ServiceMgr
        if not hasattr(ServiceMgr, self.svcName):
            from SCT_ConditionsServices.SCT_ConditionsServicesConf import SCT_ConfigurationConditionsSvc
            ServiceMgr += SCT_ConfigurationConditionsSvc(name = self.svcName)
        self.svc = getattr(ServiceMgr, self.svcName)

    def getSvc(self):
        return self.svc

    def getSvcName(self):
        return self.svcName

    def setSvcName(self, svcName):
        self.svcName = svcName

    def setup(self):
        self.setFolders()
        self.setAlgs()
        self.setSvc()

sct_ConfigurationConditionsSvcSetup = SCT_ConfigurationConditionsSvcSetup()
