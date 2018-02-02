# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration

class SCT_StripVetoSvcSetup:
    "Class to simplify setup of SCT_StripVetoSvc and required conditions algorithms"

    def __init__(self):
        self.svcName = "SCT_StripVetoSvc"
        self.svc = None

    def setSvc(self):
        from AthenaCommon.AppMgr import ServiceMgr
        if not hasattr(ServiceMgr, self.svcName):
            from SCT_ConditionsServices.SCT_ConditionsServicesConf import SCT_StripVetoSvc
            ServiceMgr += SCT_StripVetoSvc(name = self.svcName)
        self.svc = getattr(ServiceMgr, self.svcName)

    def getSvc(self):
        return self.svc

    def setup(self):
        self.setSvc()

sct_StripVetoSvcSetup = SCT_StripVetoSvcSetup()
