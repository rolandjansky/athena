# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration

class SCT_RadDamageSummarySvcSetup:
    "Class to simplify setup of SCT_RadDamageSummarySvc and required conditions algorithms"

    def __init__(self):
        self.svcName = "InDetSCT_RadDamageSummarySvc"
        self.svc = None

    def setSvc(self):
        from AthenaCommon.AppMgr import ServiceMgr
        if not hasattr(ServiceMgr, self.svcName):
            from SCT_ConditionsServices.SCT_ConditionsServicesConf import SCT_RadDamageSummarySvc
            ServiceMgr += SCT_RadDamageSummarySvc(name = self.svcName)
        self.svc = getattr(ServiceMgr, self.svcName)

    def getSvc(self):
        return self.svc

    def getSvcName(self):
        return self.svcName

    def setSvcName(self, svcName):
        self.svcName = svcName

    def setup(self):
        self.setSvc()
