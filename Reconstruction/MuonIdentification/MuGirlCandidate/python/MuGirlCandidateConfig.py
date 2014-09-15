# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from MuGirlCandidate.MuGirlCandidateConf import MuGirlNS__CandidateTool
from AthenaCommon.Configurable import *
from AthenaCommon.AppMgr import ToolSvc, ServiceMgr
from MuonRecExample import MuonRecTools

class MuGirlNS__CandidateToolConfig(MuGirlNS__CandidateTool):
    __slots__ = {}

    def __init__(self, name = Configurable.DefaultName):
        super(MuGirlNS__CandidateToolConfig, self).__init__(name)

        global ToolSvc, ServiceMgr
        from RegionSelector.RegSelSvcDefault import RegSelSvcDefault
        MuGirlCandidateRegSelSvc = RegSelSvcDefault()
        MuGirlCandidateRegSelSvc.enableMuon = True
        ServiceMgr += MuGirlCandidateRegSelSvc
        self.doCSC = True
        self.RegionSelector = MuGirlCandidateRegSelSvc
        from DCMathSegmentMaker.DCMathSegmentMakerConf import Muon__MdtMathSegmentFinder
        finder_candidate = Muon__MdtMathSegmentFinder("MuGirlCandidateSegmentFinder")
        ToolSvc += finder_candidate

        #from DCMathSegmentMaker.DCMathSegmentMakerConf import Muon__DCMathSegmentMaker
        #maker_candidate = Muon__DCMathSegmentMaker("MuGirlCandidateSegmentMaker")
        #maker_candidate.MdtSegmentFinder = finder_candidate
#       maker_candidate.CurvedErrorScaling=True
        #ToolSvc += maker_candidate
        #from MdtDriftCircleOnTrackCreator.MdtDriftCircleOnTrackCreatorConf import Muon__MdtDriftCircleOnTrackCreator
        #DriftCircleOnTrack = Muon__MdtDriftCircleOnTrackCreator("MuGirlCandidateDriftCircleOnTrack")
        #ToolSvc += DriftCircleOnTrack
        #self.MdtSegmentMaker = maker_candidate
        self.MdtSegmentMaker = MuonRecTools.getPublicTool("DCMathSegmentMaker")
        self.MdtDriftCircleOnTrackCreator = MuonRecTools.getPublicTool("MdtDriftCircleOnTrackCreator")
        #self.MdtDriftCircleOnTrackCreator = DriftCircleOnTrack
        import MuonRecExample.MuonReadCalib # setup the calibration service
        #from CscSegmentMakers.Csc2dSegmentMaker import Csc2dSegmentMaker 
        #from CscSegmentMakers.Csc4dSegmentMaker import Csc4dSegmentMaker
        self.CscSegmentMaker = MuonRecTools.getPublicTool("Csc4dSegmentMaker")
        #from MuidCaloEnergyTools.MuidCaloEnergyToolsConf import Rec__MuidCaloEnergyTool
        #ToolSvc += Rec__MuidCaloEnergyTool(name = 'MuidCaloEnergyToolParam',EnergyLossMeasurement = False)
        #ToolSvc += Rec__MuidCaloEnergyTool(name = 'MuidCaloEnergyTool')
        from MuonRecExample import MuonPrdProviderToolsConfig
        from AthenaCommon.DetFlags import DetFlags

        if DetFlags.haveRDO.MDT_on():
             self.MdtRdoToPrepDataTool = ToolSvc.MdtPrepDataProviderTool
        else:
             self.MdtRdoToPrepDataTool = None
        if DetFlags.haveRDO.RPC_on():
             self.RpcRdoToPrepDataTool = ToolSvc.RpcPrepDataProviderTool
        else:
             self.RpcRdoToPrepDataTool = None
        if DetFlags.haveRDO.TGC_on():
             self.TgcRdoToPrepDataTool = ToolSvc.TgcPrepDataProviderTool
        else:
             self.TgcRdoToPrepDataTool = None
        if DetFlags.haveRDO.CSC_on():
             self.CscRdoToPrepDataTool = ToolSvc.CscPrepDataProviderTool
        else:
             self.CscRdoToPrepDataTool = None

        #from CscClusterization.CscThresholdClusterBuilderTool import CscThresholdClusterBuilderTool
        self.CscClusterProviderTool = MuonRecTools.getPublicTool("CscThresholdClusterBuilderTool")
MuGirlNS__CandidateTool = MuGirlNS__CandidateToolConfig()
