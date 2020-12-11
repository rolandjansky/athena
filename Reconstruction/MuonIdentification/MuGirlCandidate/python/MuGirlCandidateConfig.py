# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from MuGirlCandidate.MuGirlCandidateConf import MuGirlNS__CandidateTool
from AthenaCommon.Configurable import *
from AthenaCommon.AppMgr import ToolSvc, ServiceMgr
from MuonRecExample import MuonRecTools
from AtlasGeoModel.MuonGMJobProperties import MuonGeometryFlags

class MuGirlNS__CandidateToolConfig(MuGirlNS__CandidateTool):
    __slots__ = {}

    def __init__(self, name = Configurable.DefaultName):
        super(MuGirlNS__CandidateToolConfig, self).__init__(name)

        global ToolSvc, ServiceMgr
        from RegionSelector.RegSelSvcDefault import RegSelSvcDefault
        MuGirlCandidateRegSelSvc = RegSelSvcDefault()
        MuGirlCandidateRegSelSvc.enableMuon = True
        ServiceMgr += MuGirlCandidateRegSelSvc
        if MuonGeometryFlags.hasCSC(): self.doCSC = True
        self.RegionSelector = MuGirlCandidateRegSelSvc
        from DCMathSegmentMaker.DCMathSegmentMakerConf import Muon__MdtMathSegmentFinder
        finder_candidate = Muon__MdtMathSegmentFinder("MuGirlCandidateSegmentFinder")
        ToolSvc += finder_candidate

        self.MdtSegmentMaker = MuonRecTools.getPublicTool("DCMathSegmentMaker")
        self.MdtDriftCircleOnTrackCreator = MuonRecTools.getPublicTool("MdtDriftCircleOnTrackCreator")
        import MuonRecExample.MuonReadCalib # setup the calibration service
        if MuonGeometryFlags.hasCSC(): self.CscSegmentMaker = MuonRecTools.getPublicTool("Csc4dSegmentMaker")
        else: self.CscSegmentMaker = None
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
        if MuonGeometryFlags.hasCSC() and DetFlags.haveRDO.CSC_on():
             self.CscRdoToPrepDataTool = ToolSvc.CscPrepDataProviderTool
        else:
             self.CscRdoToPrepDataTool = None

        if MuonGeometryFlags.hasCSC(): self.CscClusterProviderTool = MuonRecTools.getPublicTool("CscThresholdClusterBuilderTool")
        else: self.CscClusterProviderTool = None
MuGirlNS__CandidateTool = MuGirlNS__CandidateToolConfig()
