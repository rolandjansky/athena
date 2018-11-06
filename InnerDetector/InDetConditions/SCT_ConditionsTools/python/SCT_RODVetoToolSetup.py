# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration

class SCT_RODVetoToolSetup:
    "Class to simplify setup of SCT_RODVetoTool and required conditions algorithm"

    def __init__(self):
        self.algName = "SCT_RODVetoCondAlg"
        self.alg = None
        self.toolName = "SCT_RODVetoTool"
        self.tool = None

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

    def setTool(self):
        if self.tool is None:
            from SCT_ConditionsTools.SCT_ConditionsToolsConf import SCT_RODVetoTool
            self.tool = SCT_RODVetoTool(name = self.toolName)

    def getTool(self):
        return self.tool

    def setup(self):
        self.setAlg()
        self.setTool()
