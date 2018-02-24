# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration

class SCT_MajorityConditionsSvcSetup:
    "Class to simplify setup of SCT_MajorityConditionsSvc and required conditions algorithms"

    def __init__(self):
        self.folder = "/SCT/DCS/MAJ"
        self.folderDb = None
        self.dbInstance = "DCS_OFL"
        self.algName = "SCT_MajorityCondAlg"
        self.alg = None
        self.svcName = "InDetSCT_MajorityConditionsSvc"
        self.svc = None

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
            from SCT_ConditionsAlgorithms.SCT_ConditionsAlgorithmsConf import SCT_MajorityCondAlg
            condSeq += SCT_MajorityCondAlg(name = self.algName,
                                           ReadKey = self.folder)
        self.alg = getattr(condSeq, self.algName)

    def setSvc(self):
        from AthenaCommon.AppMgr import ServiceMgr
        if not hasattr(ServiceMgr, self.svcName):
            from SCT_ConditionsServices.SCT_ConditionsServicesConf import SCT_MajorityConditionsSvc
            ServiceMgr += SCT_MajorityConditionsSvc(name = self.svcName)
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
