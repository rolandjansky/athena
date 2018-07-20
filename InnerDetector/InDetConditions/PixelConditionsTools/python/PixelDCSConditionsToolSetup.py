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

        self.toolName = "PixelDCSConditionsTool"
        self.tool = None

    def getUseDB(self):
        return self.useDB

    def setUseDB(self, useDB):
        self.useDB = useDB

    def getToolName(self):
        return self.toolName

    def setToolName(self, toolName):
        self.toolName = toolName

    def getTool(self):
        return self.tool

    def setFolders(self):
        from AthenaCommon.GlobalFlags import globalflags
        from IOVDbSvc.CondDB import conddb

        from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
        if athenaCommonFlags.isOnline():
            self.hvFolder = "/PIXEL/HLT/DCS/HV"
            self.tempFolder = "/PIXEL/HLT/DCS/TEMPERATURE"
            self.dbInstance = "PIXEL_ONL"

        if (self.useDB):
            if not conddb.folderRequested(self.hvFolder):
                conddb.addFolder(self.dbInstance, self.hvFolder, className="CondAttrListCollection")
            if not conddb.folderRequested(self.tempFolder):
                conddb.addFolder(self.dbInstance, self.tempFolder, className="CondAttrListCollection")
            if ((globalflags.DataSource=='data')):
                if not conddb.folderRequested(self.stateFolder):
                    conddb.addFolder(self.dbInstance, self.stateFolder, className="CondAttrListCollection")
                if not conddb.folderRequested(self.statusFolder):
                    conddb.addFolder(self.dbInstance, self.statusFolder, className="CondAttrListCollection")

    def setAlgs(self):
        from AthenaCommon.GlobalFlags import globalflags
        from AthenaCommon.AlgSequence import AthSequencer
        condSeq = AthSequencer("AthCondSeq")
        
        if ((globalflags.DataSource=='data')):
            if not hasattr(condSeq, self.stateAlgName):
                from PixelConditionsAlgorithms.PixelConditionsAlgorithmsConf import PixelDCSCondStateAlg
                condSeq += PixelDCSCondStateAlg(name = self.stateAlgName)

        if not hasattr(condSeq, self.hvAlgName):
            from PixelConditionsAlgorithms.PixelConditionsAlgorithmsConf import PixelDCSCondHVAlg
            condSeq += PixelDCSCondHVAlg(name = self.hvAlgName, ReadKey = self.hvFolder)

        if not hasattr(condSeq, self.tempAlgName):
            from PixelConditionsAlgorithms.PixelConditionsAlgorithmsConf import PixelDCSCondTempAlg
            condSeq += PixelDCSCondTempAlg(name = self.tempAlgName, ReadKey = self.tempFolder)

    def setTool(self):
        from AthenaCommon.GlobalFlags import globalflags
        from AthenaCommon.AppMgr import ToolSvc
        if not hasattr(ToolSvc, self.toolName):
            from PixelConditionsTools.PixelConditionsToolsConf import PixelDCSConditionsTool
            ToolSvc += PixelDCSConditionsTool(name=self.toolName, UseDB=self.useDB, IsDATA=(globalflags.DataSource=='data'))
        self.tool = getattr(ToolSvc, self.toolName)

    def setup(self):
        self.setFolders()
        self.setAlgs()
        self.setTool()
