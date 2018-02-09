# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration

class SCT_DCSConditionsSvcSetup:
    "Class to simplify setup of SCT_DCSConditionsSvc and required conditions algorithms"

    def __init__(self):
        self.stateFolder = "/SCT/DCS/CHANSTAT"
        self.hvFolder = "/SCT/DCS/HV"
        self.tempFolder = "/SCT/DCS/MODTEMP"

        self.stateAlgName = "SCT_DCSConditionsStatCondAlg"
        self.hvAlgName = "SCT_DCSConditionsHVCondAlg"
        self.tempAlgName = "SCT_DCSConditionsTempCondAlg"

        self.stateAlg = None
        self.hvAlg = None
        self.tempAlg = None

        self.svcName = "InDetSCT_DCSConditionsSvc"
        self.svc = None

    def getStateFolder(self):
        return self.stateFolder

    def getHVFolder(self):
        return self.hvFolder

    def getTempFolder(self):
        return self.tempFolder

    def getStateAlgName(self):
        return self.stateAlgName

    def getHVAlgName(self):
        return self.hvAlgName

    def getTempAlgName(self):
        return self.tempAlgName

    def getStateAlg(self):
        return self.stateAlg

    def getHVAlg(self):
        return self.hvAlg

    def getTempAlg(self):
        return self.tempAlg

    def getSvcName(self):
        return self.svcName

    def getSvc(self):
        return self.svc

    def setFolders(self):
        from IOVDbSvc.CondDB import conddb
        if not conddb.folderRequested(self.stateFolder):
            conddb.addFolder("DCS_OFL", self.stateFolder, className="CondAttrListCollection")
        if not conddb.folderRequested(self.hvFolder):
            conddb.addFolder("DCS_OFL", self.hvFolder, className="CondAttrListCollection")
        if not conddb.folderRequested(self.tempFolder):
            conddb.addFolder("DCS_OFL", self.tempFolder, className="CondAttrListCollection")

    def setAlgs(self):
        from AthenaCommon.AlgSequence import AthSequencer
        condSeq = AthSequencer("AthCondSeq")

        if not hasattr(condSeq, self.stateAlgName):
            from SCT_ConditionsServices.SCT_ConditionsServicesConf import SCT_DCSConditionsStatCondAlg
            condSeq += SCT_DCSConditionsStatCondAlg(name = self.stateAlgName,
                                                    ReadKeyHV = self.hvFolder,
                                                    ReadKeyState = self.stateFolder)
        self.stateAlg = getattr(condSeq, self.stateAlgName)

        if not hasattr(condSeq, self.hvAlgName):
            from SCT_ConditionsServices.SCT_ConditionsServicesConf import SCT_DCSConditionsHVCondAlg
            condSeq += SCT_DCSConditionsHVCondAlg(name = self.hvAlgName,
                                                  ReadKey = self.hvFolder)
        self.hvAlg = getattr(condSeq, self.hvAlgName)

        if not hasattr(condSeq, self.tempAlgName):
            from SCT_ConditionsServices.SCT_ConditionsServicesConf import SCT_DCSConditionsTempCondAlg
            condSeq += SCT_DCSConditionsTempCondAlg(name = self.tempAlgName,
                                                    ReadKey = self.tempFolder)
        self.tempAlg = getattr(condSeq, self.tempAlgName)

    def setSvc(self):
        from AthenaCommon.AppMgr import ServiceMgr
        if not hasattr(ServiceMgr, self.svcName):
            from SCT_ConditionsServices.SCT_ConditionsServicesConf import SCT_DCSConditionsSvc
            ServiceMgr += SCT_DCSConditionsSvc(name = self.svcName)
        self.svc = getattr(ServiceMgr, self.svcName)

    def setup(self):
        self.setFolders()
        self.setAlgs()
        self.setSvc()
