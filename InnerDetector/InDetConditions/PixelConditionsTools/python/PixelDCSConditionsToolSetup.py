# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration

class PixelDCSConditionsToolSetup:
    "Class to simplify setup of PixelDCSConditionsTool and required conditions algorithms"

    def __init__(self):
        self.stateFolder = "/PIXEL/DCS/FSMSTATE"
        self.statusFolder = "/PIXEL/DCS/FSMSTATUS"
        self.hvFolder = "/PIXEL/DCS/HV"
        self.tempFolder = "/PIXEL/DCS/TEMPERATURE"
        self.dbInstance = "DCS_OFL"

        self.useDB = True

        self.stateAlgName = "PixelDCSCondStateAlg"
        self.hvAlgName = "PixelDCSCondHVAlg"
        self.tempAlgName = "PixelDCSCondTempAlg"

        self.stateAlg = None
        self.hvAlg = None
        self.tempAlg = None

        self.toolName = "InDetPixelDCSConditionsTool"
        self.tool = None

    def getStateFolder(self):
        return self.stateFolder

    def getStatusFolder(self):
        return self.statusFolder

    def getHVFolder(self):
        return self.hvFolder

    def getTempFolder(self):
        return self.tempFolder

    def setStateFolder(self, stateFolder):
        self.stateFolder = stateFolder

    def setStatusFolder(self, statusFolder):
        self.statusFolder = statusFolder

    def setHVFolder(self, hvFolder):
        self.hvFolder = hvFolder

    def setTempFolder(self, tempFolder):
        self.tempFolder = tempFolder

    def getDbInstance(self):
        return self.dbInstance

    def setDbInstance(self, dbInstance):
        self.dbInstance = dbInstance

    def getUseDB(self):
        return self.useDB

    def setUseDB(self, useDB):
        self.useDB = useDB

    def getStateAlgName(self):
        return self.stateAlgName

    def getHVAlgName(self):
        return self.hvAlgName

    def getTempAlgName(self):
        return self.tempAlgName

    def getStateAlg(self):
        return self.stateAlg

    def getHVAlg(self):
        return self.hvAlg

    def getTempAlg(self):
        return self.tempAlg

    def getToolName(self):
        return self.toolName

    def setToolName(self, toolName):
        self.toolName = toolName

    def getTool(self):
        return self.tool

    def setFolders(self):
        from IOVDbSvc.CondDB import conddb

        if (self.useDB):
            if not conddb.folderRequested(self.stateFolder):
                conddb.addFolder(self.dbInstance, self.stateFolder, className="CondAttrListCollection")
            if not conddb.folderRequested(self.statusFolder):
                conddb.addFolder(self.dbInstance, self.statusFolder, className="CondAttrListCollection")
            if not conddb.folderRequested(self.hvFolder):
                conddb.addFolder(self.dbInstance, self.hvFolder, className="CondAttrListCollection")
            if not conddb.folderRequested(self.tempFolder):
                conddb.addFolder(self.dbInstance, self.tempFolder, className="CondAttrListCollection")

    def setAlgs(self):
        from AthenaCommon.AlgSequence import AthSequencer
        condSeq = AthSequencer("AthCondSeq")
        
        if not hasattr(condSeq, self.stateAlgName):
            from PixelConditionsAlgorithms.PixelConditionsAlgorithmsConf import PixelDCSCondStateAlg
            condSeq += PixelDCSCondStateAlg(name = self.stateAlgName)
        self.stateAlg = getattr(condSeq, self.stateAlgName)

        if not hasattr(condSeq, self.hvAlgName):
            from PixelConditionsAlgorithms.PixelConditionsAlgorithmsConf import PixelDCSCondHVAlg
            condSeq += PixelDCSCondHVAlg(name = self.hvAlgName, ReadKey = self.hvFolder)
        self.hvAlg = getattr(condSeq, self.hvAlgName)

        if not hasattr(condSeq, self.tempAlgName):
            from PixelConditionsAlgorithms.PixelConditionsAlgorithmsConf import PixelDCSCondTempAlg
            condSeq += PixelDCSCondTempAlg(name = self.tempAlgName, ReadKey = self.tempFolder)
        self.tempAlg = getattr(condSeq, self.tempAlgName)

    def setTool(self):
        from AthenaCommon.AppMgr import ToolSvc
        if not hasattr(ToolSvc, self.toolName):
            from PixelConditionsTools.PixelConditionsToolsConf import PixelDCSConditionsTool
            ToolSvc += PixelDCSConditionsTool(name = self.toolName, UseDB = self.useDB)
#            ToolSvc += PixelDCSConditionsTool(name = self.toolName, ReadAllDBFolders = self.readAllDBFolders, ReturnHVTemp = self.returnHVTemp)
        self.tool = getattr(ToolSvc, self.toolName)

    def setup(self):
        self.setFolders()
        self.setAlgs()
        self.setTool()
