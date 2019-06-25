# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

class PixelConditionsSummaryToolSetup:
    "Class to simplify setup of PixelConditionsSummaryTool and required conditions algorithms"

    def __init__(self):
        self.toolName = "PixelConditionsSummaryTool"
        self.tool = None
        self.useConditions = True
        self.useDCSState = False
        self.useByteStream = False
        self.useTDAQ = False
        self.useDeadMap = True

    def setUseConditions(self, useConditions):
        self.useConditions = useConditions

    def getUseConditions(self):
        return self.useConditions

    def setUseDCSState(self, useDCSState):
        self.useDCSState = useDCSState

    def getUseDCSState(self):
        return self.useDCSState

    def setUseByteStream(self, useByteStream):
        self.useByteStream = useByteStream

    def getUseByteStream(self):
        return self.useByteStream

    def setUseTDAQ(self, useTDAQ):
        self.useTDAQ = useTDAQ

    def getUseTDAQ(self):
        return self.useTDAQ

    def setUseDeadMap(self, useDeadMap):
        self.useDeadMap = useDeadMap

    def getUseDeadMap(self):
        return self.useDeadMap

    def getTool(self):
        return self.tool

    def getToolName(self):
        return self.toolName

    def setToolName(self, toolName):
        self.toolName = toolName

    def setTool(self):
        from IOVDbSvc.CondDB import conddb
        from AthenaCommon.AlgSequence import AthSequencer
        condSeq = AthSequencer("AthCondSeq")

        if (self.useDCSState):
            if not conddb.folderRequested("/PIXEL/DCS/FSMSTATE"):
                conddb.addFolder("DCS_OFL", "/PIXEL/DCS/FSMSTATE", className="CondAttrListCollection")
            if not conddb.folderRequested("/PIXEL/DCS/FSMSTATUS"):
                conddb.addFolder("DCS_OFL", "/PIXEL/DCS/FSMSTATUS", className="CondAttrListCollection")

            if not hasattr(condSeq, "PixelDCSCondStateAlg"):
                from PixelConditionsAlgorithms.PixelConditionsAlgorithmsConf import PixelDCSCondStateAlg
                condSeq += PixelDCSCondStateAlg(name="PixelDCSCondStateAlg")

        if (self.useTDAQ):
            if not conddb.folderRequested("/TDAQ/Resources/ATLAS/PIXEL/Modules"):
                conddb.addFolder("TDAQ_ONL", "/TDAQ/Resources/ATLAS/PIXEL/Modules", className="CondAttrListCollection")

            if not hasattr(condSeq, "PixelTDAQCondAlg"):
                from PixelConditionsAlgorithms.PixelConditionsAlgorithmsConf import PixelTDAQCondAlg
                condSeq += PixelTDAQCondAlg(name="PixelTDAQCondAlg", ReadKey="/TDAQ/Resources/ATLAS/PIXEL/Modules")

        if (self.useDeadMap):
            if not (conddb.folderRequested("/PIXEL/PixMapOverlay") or conddb.folderRequested("/PIXEL/Onl/PixMapOverlay")):
                conddb.addFolderSplitOnline("PIXEL","/PIXEL/Onl/PixMapOverlay","/PIXEL/PixMapOverlay", className='CondAttrListCollection')

            if not hasattr(condSeq, "PixelConfigCondAlg"):
                from PixelConditionsAlgorithms.PixelConditionsAlgorithmsConf import PixelConfigCondAlg
                condSeq += PixelConfigCondAlg(name="PixelConfigCondAlg", 
                                              UseDeadMap=self.useDeadMap,
                                              ReadDeadMapKey="/PIXEL/PixMapOverlay",
                                              UseCalibConditions=True)

        if self.tool is None:
            from PixelConditionsTools.PixelConditionsToolsConf import PixelConditionsSummaryTool
            self.tool = PixelConditionsSummaryTool(name=self.toolName, 
                                                   UseDCSState=self.useDCSState, 
                                                   UseByteStream=self.useByteStream, 
                                                   UseTDAQ=self.useTDAQ, 
                                                   UseDeadMap=self.useDeadMap)

    def setup(self):
        self.setTool()

