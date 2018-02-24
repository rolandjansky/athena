# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration

class SCT_ConditionsSummarySvcSetup:
    "Class to simplify setup of SCT_ConditionsSummarySvc and required conditions algorithms"

    def __init__(self):
        self.svcName = "InDetSCT_ConditionsSummarySvc"
        self.svc = None

    def setSvc(self):
        from AthenaCommon.AppMgr import ServiceMgr
        if not hasattr(ServiceMgr, self.svcName):
            from SCT_ConditionsServices.SCT_ConditionsServicesConf import SCT_ConditionsSummarySvc
            ServiceMgr += SCT_ConditionsSummarySvc(name = self.svcName)
        self.svc = getattr(ServiceMgr, self.svcName)

    def getSvc(self):
        return self.svc

    def getSvcName(self):
        return self.svcName

    def setSvcName(self, svcName):
        self.svcName = svcName

    def setup(self):
        self.setSvc()
