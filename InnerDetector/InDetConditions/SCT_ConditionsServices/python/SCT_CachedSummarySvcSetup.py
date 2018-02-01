# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration

class SCT_CachedSummarySvcSetup:
    "Class to simplify setup of SCT_CachedSummarySvc and required conditions algorithms"

    def __init__(self):
        self.svcName = "SCT_CachedSummarySvc"
        self.svc = None

    def setSvc(self):
        from AthenaCommon.AppMgr import ServiceMgr
        if not hasattr(ServiceMgr, self.svcName):
            from SCT_ConditionsServices.SCT_ConditionsServicesConf import SCT_CachedSummarySvc
            ServiceMgr += SCT_CachedSummarySvc(name = self.svcName)
        self.svc = getattr(ServiceMgr, self.svcName)

    def getSvc(self):
        return self.svc

    def setup(self):
        self.setSvc()

sct_CachedSummarySvcSetup = SCT_CachedSummarySvcSetup()
