# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

########################## start of MuonTiming Filter Fragment ##################################
from AthenaCommon.AppMgr import ToolSvc
import AthenaCommon.CfgMgr      as CfgMgr
from AthenaCommon.Include import include
include( "TrackInCaloTools/TrackInCaloTools_jobOptions.py" )
#from TrackInCaloTools import TrackInCaloTools_jobOptions

# CalDb tool
from TRT_ConditionsServices.TRT_ConditionsServicesConf import TRT_CalDbTool
InDetTRTCalDbTool = TRT_CalDbTool(name = "TRT_CalDbTool")

# set up TRT tool
InDetSlidingWindowTrackTimeToolMCP = CfgMgr.InDet__InDetSlidingWindowTrackTimeTool \
   (name              = "InDetSlidingWindowTrackTimeToolMCP",
    TRTCalDbTool      = InDetTRTCalDbTool,
    UseNewEP          = False,
    GlobalOffset      = 0.,
    NumberIterations  = 5,
    WindowSize        = 7)
ToolSvc += InDetSlidingWindowTrackTimeToolMCP

TRT_TrackTimingTool = CfgMgr.InDet__TRT_TrackTimingTool \
   (name = "TRT_TrackTimingTool",
    EventPhaseTool = InDetSlidingWindowTrackTimeToolMCP)
ToolSvc += TRT_TrackTimingTool
#TRT_TrackTimingTool.OutputLevel = 1
#if muonCombinedRecFlags.printConfigurables():
#    print TRT_TrackTimingTool

# set up Calo Tools
from TrackInCaloTools import TrackInCaloTools
ToolSvc += TrackInCaloTools.TrackInCaloTools()
from TrkExTools.AtlasExtrapolator import AtlasExtrapolator
ToolSvc += AtlasExtrapolator()

myTrackInCaloTimingTool = CfgMgr.Rec__TrackInCaloTimingTool()
myTrackInCaloTimingTool.TrackInCaloTool = ToolSvc.TrackInCaloTools
ToolSvc += myTrackInCaloTimingTool
#myTrackInCaloTimingTool.OutputLevel = 1
#if muonCombinedRecFlags.printConfigurables():
#    print myTrackInCaloTimingTool

# set up the MDT and CSC segment tool
SegmentTimingTool = CfgMgr. Muon__SegmentTimingTool("SegmentTimingTool")
ToolSvc += SegmentTimingTool
#SegmentTimingTool.MatchThreshold = 0.
#SegmentTimingTool.NumberOfMatchedCut = 4
#SegmentTimingTool.OutputLevel = 1
#if muonCombinedRecFlags.printConfigurables():
#    print SegmentTimingTool

# set up RPC time retrieval
RpcTimingTool = CfgMgr.Muon__RPC_TimingTool("RPC_TimingTool")
ToolSvc += RpcTimingTool
#RpcTimingTool.OutputLevel = 1

# set up the main tool
myMuonCombinedTimingTool = CfgMgr.Rec__MuonCombinedTimingTool \
    (name            = "MuonCombinedTimingTool",
     InDetTimingTool = TRT_TrackTimingTool,
     CaloTimingTool  = myTrackInCaloTimingTool,
     MDT_TimingTool  = SegmentTimingTool,  # actually MDT+CSC
     RPC_TimingTool  = RpcTimingTool )
ToolSvc += myMuonCombinedTimingTool
#myMuonCombinedTimingTool.OutputLevel = 2
#if muonCombinedRecFlags.printConfigurables():
#    print myMuonCombinedTimingTool
      
#chambert0s = ""
#if D3PDMakerFlags.MuonSGKey() == 'StacoMuonCollection':
#        chambert0s = 'MboyMuonChamberT0s'
#elif D3PDMakerFlags.MuonSGKey() == 'MuidMuonCollection':
#        chambert0s = 'MooreMuonChamberT0s'
#else:
#        chambert0s = 'OtherMuonAnalysis'


myCosmicIdentificationTool = CfgMgr.Rec__CosmicIdentificationTool \
    (name                   = "CosmicIdentificationTool",
     MuonCombinedTimingTool = myMuonCombinedTimingTool )
ToolSvc += myCosmicIdentificationTool

########################## end of MuonCosmic Filter Fragment #########################################
