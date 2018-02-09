# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration

class SCT_SiPropertiesSvcSetup:
    "Class to simplify setup of SCT_SiPropertiesSvc and required conditions algorithms"

    def __init__(self):
        self.algName = "SCTSiPropertiesCondAlg"
        self.alg = None
        self.svcName = "SCT_SiPropertiesSvc"
        self.svc = None
        self.siliconSvc = None

    def getAlgName(self):
        return self.algName

    def setAlgName(self, algName):
        self.algName = algName

    def getSvcName(self):
        return self.svcName

    def setSvcName(self, svcName):
        self.svcName = svcName

    def getSvc(self):
        return self.svc

    def setSiliconSvc(self, siliconSvc):
        self.siliconSvc = siliconSvc

    def setAlg(self):
        from AthenaCommon.AlgSequence import AthSequencer
        condSeq = AthSequencer("AthCondSeq")

        if not hasattr(condSeq, self.algName):
            from SiPropertiesSvc.SiPropertiesSvcConf import SCTSiPropertiesCondAlg
            if self.siliconSvc is None:
                condSeq += SCT_SiliconHVCondAlg(name = self.algName)
            else:
                condSeq += SCT_SiliconHVCondAlg(name = self.algName,
                                                SiConditionsServices = self.siliconSvc)
        self.alg = getattr(condSeq, self.algName)

    def setSvc(self):
        from AthenaCommon.AppMgr import ServiceMgr
        if not hasattr(ServiceMgr, self.svcName):
            from SiPropertiesSvc.SiPropertiesSvcConf import SiPropertiesCHSvc
            sctSiPropertiesSvc = SiPropertiesCHSvc(name = self.svcName,
                                                   DetectorName = "SCT",
                                                   ReadKey = "SCTSiliconPropertiesVector")
        self.svc = getattr(ServiceMgr, self.svcName)

    def setup(self):
        self.setAlg()
        self.setSvc()
