# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from TrigInDetTrackFitter.TrigInDetTrackFitter_Config import ConfiguredTrigL2LowPtTrackFitter
from TrigInDetTrackFitter.TrigInDetTrackFitter_Config import ConfiguredTrigL2_Extrapolator
from TrigInDetTrackFitter.TrigInDetTrackFitter_Config import ConfiguredTrigL2_InDetRotCreator


from TrigTRT_TrackExtensionTool.TrigTRT_TrackExtensionToolConf import TrigTRT_TrackExtensionTool
from TrigInDetTrackFitter.TrigInDetTrackFitterConf import TrigInDetBremDetectionTool
from TrigTRT_TrackExtensionTool.TrigTRT_TrackExtensionToolConf import TrigTRT_CombinedExtensionTool
from TrigOfflineDriftCircleTool.TrigOfflineDriftCircleTool_Config import ConfiguredTrigTRT_DCCollByteStreamTool

from AthenaCommon.AppMgr import ToolSvc

ideTool = TrigInDetBremDetectionTool(name="TrigBremDetectorAndEstimator")
ToolSvc += ideTool

class TrigTRT_TrackExtensionTool_eGamma(TrigTRT_TrackExtensionTool):
    __slots__ = ()
    def __init__(self, name = "TrigTRT_TrackExtensionTool_eGamma"):
        TrigTRT_TrackExtensionTool.__init__(self, name)
        self.OfflineClusters=False
        self.doBremmCorrection=True
        self.TrigBremTool=ideTool



class ConfiguredTrigTRT_TrackExtensionTool(TrigTRT_CombinedExtensionTool):
    __slots__ = ()
    def __init__(self, name = "ConfiguredTrigTRT_TrackExtensionTool"):
        TrigTRT_CombinedExtensionTool.__init__(self, name)
        offlineExtrapolator = ConfiguredTrigL2_Extrapolator()
        offlineRotCreator = ConfiguredTrigL2_InDetRotCreator()
        lowPtFitter = ConfiguredTrigL2LowPtTrackFitter()
        from AthenaCommon.AppMgr import ToolSvc
        ToolSvc += offlineExtrapolator
        ToolSvc += offlineRotCreator
        ToolSvc += lowPtFitter
        self.ROT_CreatorTool = offlineRotCreator
        self.TrackExtrapolatorTool = offlineExtrapolator
        self.useROTs = False
        self.LowPtTrackFitter = lowPtFitter





    



