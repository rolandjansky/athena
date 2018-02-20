# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration

class SCT_ReadCalibDataSvcSetup:
    "Class to simplify setup of SCT_ReadCalibDataSvc and required conditions algorithms"

    def __init__(self):
        self.noiseFolder = "/SCT/DAQ/Calibration/NoiseOccupancyDefects"
        self.gainFolder = "/SCT/DAQ/Calibration/NPtGainDefects"
        self.algName = "SCT_ReadCalibDataCondAlg"
        self.alg = None
        self.svcName = "InDetSCT_ReadCalibDataSvc"
        self.svc = None

    def getNoiseFolder(self):
        return self.noiseFolder

    def getGainFolder(self):
        return self.gainFolder

    def getAlgName(self):
        return self.algName

    def getAlg(self):
        return self.alg

    def setFolders(self):
        from IOVDbSvc.CondDB import conddb
        if not conddb.folderRequested(self.noiseFolder):
            conddb.addFolderSplitMC("SCT", self.noiseFolder, self.noiseFolder, className="CondAttrListCollection")
        if not conddb.folderRequested(self.gainFolder):
            conddb.addFolderSplitMC("SCT", self.gainFolder, self.gainFolder, className="CondAttrListCollection")

    def setAlgs(self):
        from AthenaCommon.AlgSequence import AthSequencer
        condSeq = AthSequencer("AthCondSeq")
        if not hasattr(condSeq, self.algName):
            from SCT_ConditionsAlgorithms.SCT_ConditionsAlgorithmsConf import SCT_ReadCalibDataCondAlg
            condSeq += SCT_ReadCalibDataCondAlg(name = self.algName,
                                                ReadKeyGain = self.gainFolder,
                                                ReadKeyNoise = self.noiseFolder)
        self.alg = getattr(condSeq, self.algName)

    def setSvc(self):
        from AthenaCommon.AppMgr import ServiceMgr
        if not hasattr(ServiceMgr, self.svcName):
            from SCT_ConditionsServices.SCT_ConditionsServicesConf import SCT_ReadCalibDataSvc
            ServiceMgr += SCT_ReadCalibDataSvc(name = self.svcName)
        self.svc = getattr(ServiceMgr, self.svcName)

    def getSvc(self):
        return self.svc

    def getSvcName(self):
        return self.svcName

    def setSvcName(self, svcName):
        self.svcName = svcName

    def setup(self):
        self.setFolders()
        self.setAlgs()
        self.setSvc()
