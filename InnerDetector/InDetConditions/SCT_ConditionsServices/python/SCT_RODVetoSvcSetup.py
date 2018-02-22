# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration

class SCT_RODVetoSvcSetup:
    "Class to simplify setup of SCT_RODVetoSvc and required conditions algorithm"

    def __init__(self):
        self.algName = "SCT_RODVetoCondAlg"
        self.alg = None
        self.svcName = "SCT_RODVetoSvc"
        self.svc = None

    def setAlg(self):
        # In this case, conditions algorithm is scheduled in NOT AthCondSeq but AthAlgSeq 
        # because conditions data are written in event store.
        from AthenaCommon.AlgSequence import AthSequencer
        job = AthSequencer("AthAlgSeq")
        if not hasattr(job, self.algName):
            from SCT_ConditionsAlgorithms.SCT_ConditionsAlgorithmsConf import SCT_RODVetoCondAlg
            job += SCT_RODVetoCondAlg(name = self.algName)
        self.alg = getattr(job, self.algName)

    def getAlg(self):
        return self.alg

    def setSvc(self):
        from AthenaCommon.AppMgr import ServiceMgr
        if not hasattr(ServiceMgr, self.svcName):
            from SCT_ConditionsServices.SCT_ConditionsServicesConf import SCT_RODVetoSvc
            ServiceMgr += SCT_RODVetoSvc(name = self.svcName)
        self.svc = getattr(ServiceMgr, self.svcName)

    def getSvc(self):
        return self.svc

    def setup(self):
        self.setAlg()
        self.setSvc()
