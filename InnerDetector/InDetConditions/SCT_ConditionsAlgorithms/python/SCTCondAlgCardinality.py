# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

class SCTCondAlgCardinality:
    "Set Cardinality of clonable reentrant SCT condition algorithms"

    def __init__(self):
        self.clonableCondAlgNames = [
            # InnerDetector/InDetConditions/SCT_ConditionsAlgorithms package
            "SCT_ConditionsParameterCondAlg",
            "SCT_ConfigurationCondAlg",
            "SCT_DCSConditionsHVCondAlg",
            "SCT_DCSConditionsStatCondAlg",
            "SCT_DCSConditionsTempCondAlg",
            "SCT_DetectorElementCondAlg",
            "SCT_LinkMaskingCondAlg",
            "SCT_MajorityCondAlg",
            "SCT_ModuleVetoCondAlg",
            "SCT_MonitorCondAlg",
            "SCT_ReadCalibChipGainCondAlg",
            "SCT_ReadCalibChipNoiseCondAlg",
            "SCT_ReadCalibDataCondAlg",
            "SCT_RODVetoCondAlg",
            "SCT_SensorsCondAlg",
            "SCT_SiliconHVCondAlg",
            "SCT_SiliconTempCondAlg",
            "SCT_TdaqEnabledCondAlg",
            # InnerDetector/InDetConditions/SiLorentzAngleTool package
            "SCTSiLorentzAngleCondAlg",
            # InnerDetector/InDetConditions/SiPropertiesTool package
            "SCTSiPropertiesCondAlg",
            # InnerDetector/InDetDetDescr/SCT_Cabling package
            "SCT_CablingCondAlgFromCoraCool",
            "SCT_CablingCondAlgFromText",
            # InnerDetector/InDetRecAlgs/SiSpacePointFormation package
            "InDetSiElementPropertiesTableCondAlg",
            # InnerDetector/InDetRecTools/SiCombinatorialTrackFinderTool_xk package
            "InDetSiDetElementBoundaryLinksSCTCondAlg",
            # InnerDetector/InDetRecTools/SiDetElementsRoadTool_xk package
            "InDet__SiDetElementsRoadCondAlg_xk"
        ]

    def set(self, numThreads):
        if numThreads >= 2:
            from AthenaCommon.AlgSequence import AthSequencer
            condSeq = AthSequencer("AthCondSeq")
            for condAlgName in self.clonableCondAlgNames:
                if hasattr(condSeq, condAlgName):
                    condAlg = getattr(condSeq, condAlgName)
                    condAlg.Cardinality = numThreads

sctCondAlgCardinality = SCTCondAlgCardinality()
