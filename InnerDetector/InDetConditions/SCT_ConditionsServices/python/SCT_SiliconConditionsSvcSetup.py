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

        self.useDB = True

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

    def getSvc(self):
        return self.svc

    def getUseDB(self):
        return self.useDB

    def setAlgs(self):
        from AthenaCommon.AlgSequence import AthSequencer
        condSeq = AthSequencer("AthCondSeq")

        if not hasattr(condSeq, self.hvAlgName):
            from SCT_ConditionsServices.SCT_ConditionsServicesConf import SCT_SiliconHVCondAlg
            condSeq += SCT_SiliconHVCondAlg(name = self.hvAlgName)
        self.hvAlg = getattr(condSeq, self.hvAlgName)

        if not hasattr(condSeq, self.tempAlgName):
            from SCT_ConditionsServices.SCT_ConditionsServicesConf import SCT_SiliconTempCondAlg
            condSeq += SCT_SiliconTempCondAlg(name = self.tempAlgName)
        self.tempAlg = getattr(condSeq, self.tempAlgName)

    def setSvc(self):
        from AthenaCommon.AppMgr import ServiceMgr
        if not hasattr(ServiceMgr, self.svcName):
            from SCT_ConditionsServices.SCT_ConditionsServicesConf import SCT_SiliconConditionsSvc
            ServiceMgr += SCT_SiliconConditionsSvc(name = self.svcName)
        self.svc = getattr(ServiceMgr, self.svcName)

    def setUseDB(self, useDB):
        self.useDB = useDB

    def setup(self):
        if self.useDB:
            from SCT_ConditionsServices.SCT_DCSConditionsSvcSetup import SCT_DCSConditionsSvcSetup
            SCT_DCSConditionsSvcSetup = SCT_DCSConditionsSvcSetup()
            SCT_DCSConditionsSvcSetup.setup()
            self.setAlgs()
        self.setSvc()
        self.svc.UseDB = self.useDB
