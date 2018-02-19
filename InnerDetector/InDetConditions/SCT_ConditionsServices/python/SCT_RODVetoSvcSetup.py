# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration

class SCT_RODVetoSvcSetup:
    "Class to simplify setup of SCT_RODVetoSvc and required conditions algorithms"

    def __init__(self):
        self.svcName = "SCT_RODVetoSvc"
        self.svc = None

    def setSvc(self):
        from AthenaCommon.AppMgr import ServiceMgr
        if not hasattr(ServiceMgr, self.svcName):
            from SCT_ConditionsServices.SCT_ConditionsServicesConf import SCT_RODVetoSvc
            ServiceMgr += SCT_RODVetoSvc(name = self.svcName)
        self.svc = getattr(ServiceMgr, self.svcName)

    def getSvc(self):
        return self.svc

    def setup(self):
        self.setSvc()
