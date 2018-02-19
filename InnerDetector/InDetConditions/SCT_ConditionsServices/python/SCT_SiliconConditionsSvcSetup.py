# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration

class SCT_SiliconConditionsSvcSetup:
    "Class to simplify setup of SCT_SiliconConditionsSvc and required conditions algorithms"

    def __init__(self):
        self.hvAlgName = "SCT_SiliconHVCondAlg"
        self.tempAlgName = "SCT_SiliconTempCondAlg"

        self.hvAlg = None
        self.tempAlg = None

        self.svcName = "SCT_SiliconConditionsSvc"
        self.svc = None

        self.dcsSvc = None

        self.useDB = True
        self.forceUseGeoModel = False

    def getHVAlgName(self):
        return self.hvAlgName

    def getTempAlgName(self):
        return self.tempAlgName

    def getHVAlg(self):
        return self.hvAlg

    def getTempAlg(self):
        return self.tempAlg

    def getSvcName(self):
        return self.svcName

    def setSvcName(self, svcName):
        self.svcName = svcName

    def getSvc(self):
        return self.svc

    def setDcsSvc(self, dcsSvc):
        self.dcsSvc = dcsSvc

    def getUseDB(self):
        return self.useDB

    def setAlgs(self):
        from AthenaCommon.AlgSequence import AthSequencer
        condSeq = AthSequencer("AthCondSeq")

        if not hasattr(condSeq, self.hvAlgName):
            from SCT_ConditionsServices.SCT_ConditionsServicesConf import SCT_SiliconHVCondAlg
            if self.dcsSvc is None:
                condSeq += SCT_SiliconHVCondAlg(name = self.hvAlgName)
            else:
                condSeq += SCT_SiliconHVCondAlg(name = self.hvAlgName,
                                                UseState = self.dcsSvc.ReadAllDBFolders,
                                                DCSConditionsSvc = self.dcsSvc)
        self.hvAlg = getattr(condSeq, self.hvAlgName)

        if not hasattr(condSeq, self.tempAlgName):
            from SCT_ConditionsServices.SCT_ConditionsServicesConf import SCT_SiliconTempCondAlg
            if self.dcsSvc is None:
                condSeq += SCT_SiliconTempCondAlg(name = self.tempAlgName)
            else:
                condSeq += SCT_SiliconTempCondAlg(name = self.tempAlgName,
                                                  UseState = self.dcsSvc.ReadAllDBFolders,
                                                  DCSConditionsSvc = self.dcsSvc)
        self.tempAlg = getattr(condSeq, self.tempAlgName)

    def setSvc(self):
        from AthenaCommon.AppMgr import ServiceMgr
        if not hasattr(ServiceMgr, self.svcName):
            from SCT_ConditionsServices.SCT_ConditionsServicesConf import SCT_SiliconConditionsSvc
            ServiceMgr += SCT_SiliconConditionsSvc(name = self.svcName,
                                                   UseDB = self.useDB,
                                                   ForceUseGeoModel = self.forceUseGeoModel)
        self.svc = getattr(ServiceMgr, self.svcName)

    def setUseDB(self, useDB):
        self.useDB = useDB

    def setForceUseGeoModel(self, forceUseGeoModel):
        self.forceUseGeoModel = forceUseGeoModel

    def setup(self):
        if self.useDB:
            self.setAlgs()
        self.setSvc()
        self.svc.UseDB = self.useDB
