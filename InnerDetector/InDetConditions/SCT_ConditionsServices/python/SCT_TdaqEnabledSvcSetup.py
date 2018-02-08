# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration

class SCT_TdaqEnabledSvcSetup:
    "Class to simplify setup of SCT_TdaqEnabledSvc and required conditions algorithms"

    def __init__(self):
        self.folder = "/TDAQ/Resources/ATLAS/SCT/Robins"
        self.folderDb = None
        self.folderTag = None
        self.dbInstance = "TDAQ"
        self.algName = "SCT_TdaqEnabledCondAlg"
        self.alg = None
        self.svcName = "SCT_TdaqEnabledSvc"
        self.svc = None
        self.eventInfoKey = "ByteStreamEventInfo"

    def getFolder(self):
        return self.folder

    def setFolder(self, folder):
        self.folder = folder

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
                conddb.addFolder(self.dbInstance, self.folderDb, className="CondAttrListCollection")
            else:
                conddb.addFolderWithTag(self.dbInstance, self.folderDb, self.folderTag, className="CondAttrListCollection")

    def setAlgs(self):
        from AthenaCommon.AlgSequence import AthSequencer
        condSeq = AthSequencer("AthCondSeq")
        if not hasattr(condSeq, self.algName):
            from SCT_ConditionsServices.SCT_ConditionsServicesConf import SCT_TdaqEnabledCondAlg
            condSeq += SCT_TdaqEnabledCondAlg(name = self.algName,
                                              ReadKey = self.folder,
                                              EventInfoKey = self.eventInfoKey)
        self.alg = getattr(condSeq, self.algName)

    def setSvc(self):
        from AthenaCommon.AppMgr import ServiceMgr
        if not hasattr(ServiceMgr, self.svcName):
            from SCT_ConditionsServices.SCT_ConditionsServicesConf import SCT_TdaqEnabledSvc
            ServiceMgr += SCT_TdaqEnabledSvc(name = self.svcName,
                                             EventInfoKey = self.eventInfoKey)
        self.svc = getattr(ServiceMgr, self.svcName)

    def getSvc(self):
        return self.svc

    def setSvcName(self, svcName):
        self.svcName = svcName

    def getSvcName(self):
        return self.svcName

    def setEventInfoKey(self, eventInfoKey):
        self.eventInfoKey = eventInfoKey

    def getEventInfoKey(self):
        return self.eventInfoKey

    def setup(self):
        self.setFolders()
        self.setAlgs()
        self.setSvc()

sct_TdaqEnabledSvcSetup = SCT_TdaqEnabledSvcSetup()
