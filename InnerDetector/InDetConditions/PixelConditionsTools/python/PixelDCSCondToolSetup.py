# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration

class PixelDCSCondToolSetup:
    "Class to simplify setup of PixelDCSCondTool and required conditions algorithms"

    def __init__(self):
        self.stateFolder = "/PIXEL/DCS/FSMSTATUS"
        self.hvFolder = "/PIXEL/DCS/HV"
        self.tempFolder = "/PIXEL/DCS/TEMPERATURE"
        self.dbInstance = "DCS_OFL"

        self.readAllDBFolders = True
        self.returnHVTemp = True

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

    def getHVFolder(self):
        return self.hvFolder

    def getTempFolder(self):
        return self.tempFolder

    def setStateFolder(self, stateFolder):
        self.stateFolder = stateFolder

    def setHVFolder(self, hvFolder):
        self.hvFolder = hvFolder

    def setTempFolder(self, tempFolder):
        self.tempFolder = tempFolder

    def getDbInstance(self):
        return self.dbInstance

    def setDbInstance(self, dbInstance):
        self.dbInstance = dbInstance

    def getReadAllDBFolders(self):
        return self.readAllDBFolders

    def setReadAllDBFolders(self, readAllDBFolders):
        self.readAllDBFolders = readAllDBFolders

    def getReturnHVTemp(self):
        return self.returnHVTemp

    def setReturnHVTemp(self, returnHVTemp):
        self.returnHVTemp = returnHVTemp

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

        if ((self.readAllDBFolders and self.returnHVTemp) or (not self.readAllDBFolders and not self.returnHVTemp)):
            if not conddb.folderRequested(self.stateFolder):
                conddb.addFolder(self.dbInstance, self.stateFolder, className="CondAttrListCollection")

        if ((self.readAllDBFolders and self.returnHVTemp) or self.returnHVTemp):
            if not conddb.folderRequested(self.hvFolder):
                conddb.addFolder(self.dbInstance, self.hvFolder, className="CondAttrListCollection")
            if not conddb.folderRequested(self.tempFolder):
                conddb.addFolder(self.dbInstance, self.tempFolder, className="CondAttrListCollection")

    def setAlgs(self):
        from AthenaCommon.AlgSequence import AthSequencer
        condSeq = AthSequencer("AthCondSeq")
        
        if ((self.readAllDBFolders and self.returnHVTemp) or (not self.readAllDBFolders and not self.returnHVTemp)):
            if not hasattr(condSeq, self.stateAlgName):
                from PixelConditionsAlgorithms.PixelConditionsAlgorithmsConf import PixelDCSCondStateAlg
                condSeq += PixelDCSCondStateAlg(name = self.stateAlgName, ReadKeyState = self.stateFolder)
            self.stateAlg = getattr(condSeq, self.stateAlgName)

        if ((self.readAllDBFolders and self.returnHVTemp) or self.returnHVTemp):
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
            from PixelConditionsTools.PixelConditionsToolsConf import PixelDCSCondTool
            ToolSvc += PixelDCSCondTool(name = self.toolName, ReadAllDBFolders = self.readAllDBFolders, ReturnHVTemp = self.returnHVTemp)
        self.tool = getattr(ToolSvc, self.toolName)

    def setup(self):
        self.setFolders()
        self.setAlgs()
        self.setTool()
