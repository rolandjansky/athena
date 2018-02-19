# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration

class SCT_ByteStreamErrorsSvcSetup:
    "Class to simplify setup of SCT_ByteStreamErrorsSvc"

    def __init__(self):
        self.configSvc = None
        self.svcName = "SCT_ByteStreamErrorsSvc"
        self.svc = None

    def setSvc(self):
        from AthenaCommon.AppMgr import ServiceMgr
        if not hasattr(ServiceMgr, self.svcName):
            from SCT_ConditionsServices.SCT_ConditionsServicesConf import SCT_ByteStreamErrorsSvc
            if self.configSvc is None:
                ServiceMgr += SCT_ByteStreamErrorsSvc(name = self.svcName)
            else:
                ServiceMgr += SCT_ByteStreamErrorsSvc(name = self.svcName,
                                                      ConfigService = self.configSvc)
        self.svc = getattr(ServiceMgr, self.svcName)

    def getSvc(self):
        return self.svc

    def setConfigSvc(self, configSvc):
        self.configSvc = configSvc

    def getSvcName(self):
        return self.svcName

    def setSvcName(self, svcName):
        self.svcName = svcName

    def setup(self):
        self.setSvc()
