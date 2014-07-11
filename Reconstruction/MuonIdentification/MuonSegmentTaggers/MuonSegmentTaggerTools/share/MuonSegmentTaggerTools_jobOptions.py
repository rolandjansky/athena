include.block ( "MuonSegmentTaggerTools/MuonSegmentTaggerTools_jobOptions.py" )
#-----------------------------------------------------------------
from TrkExTools.AtlasExtrapolator import AtlasExtrapolator
MuTagExtrapolator = AtlasExtrapolator()
from TrackToSegmentTools.TrackToSegmentToolsConf import TrackToSegmentTool
ConfiguredForAllStationsTrackToSegmentTool = TrackToSegmentTool(
    name                  = "ConfiguredForAllStationsTrackToSegmentTool" ,
    IExtrapolator         = MuTagExtrapolator                            ,
    addMultipleScattering = 0                                            ,
    ReverseIDTrack        = 0                                            )
ToolSvc += ConfiguredForAllStationsTrackToSegmentTool
#print ConfiguredForAllStationsTrackToSegmentTool

#--------------------------------------------------------------------------------------------------------------------------
from TrkTrackSummaryTool.AtlasTrackSummaryTool import AtlasTrackSummaryTool
TrkTrackSummaryToolFORMuTagIMOInDetDetailedTrackSelectorTool = AtlasTrackSummaryTool()
ToolSvc += TrkTrackSummaryToolFORMuTagIMOInDetDetailedTrackSelectorTool

from InDetTrackSelectorTool.InDetTrackSelectorToolConf import InDet__InDetDetailedTrackSelectorTool

from AthenaCommon.GlobalFlags import globalflags
from RecExConfig.RecFlags import rec

MuTagIMOInDetDetailedTrackSelectorTool = InDet__InDetDetailedTrackSelectorTool(
      name                 = "MuTagIMOInDetDetailedTrackSelectorTool",
      pTMin                = 2000*MeV,
      IPd0Max              = 50.0*mm,
      IPz0Max              = 9999.0*mm,
      z0Max                = 9999.0*mm,
      useTrackSummaryInfo  = False,
      nHitBLayer           = 0,
      nHitPix              = 1,
      nHitBLayerPlusPix    = 0,
      nHitSct              = 3,
      nHitSi               = 4,
      nHitTrt              = 0,
      useTrackQualityInfo  = False,
      TrackSummaryTool = TrkTrackSummaryToolFORMuTagIMOInDetDetailedTrackSelectorTool,
      Extrapolator = MuTagExtrapolator)

ToolSvc +=MuTagIMOInDetDetailedTrackSelectorTool
#---#print MuTagIMOInDetDetailedTrackSelectorTool
#--------------------------------------------------------------------------------------------------------------------------

#if rec.Commissioning == True :
#  ToolSvc.MuTagIMOInDetDetailedTrackSelectorTool.useTrackSummaryInfo = False

from MuTagTools.MuTagToolsConf import SegmentsFilterTool
ConfiguredForAllStationsSegmentsFilterTool = SegmentsFilterTool(
    name = "ConfiguredForAllStationsSegmentsFilterTool",
 RejectSegmentsAgainstCbMuonContainer             = 0                            ,
 SegmentShouldNotBeInTracksOfThisCbMuonContainer  = "NoContainer"                ,
 ApplyQualityFactorCut                            = 0                            ,
 QualityFactorCut                                 = 9.                           ,
 ApplyNberofDigitsCut                             = 0                            ,
 NberofDigitsCut                                  = 4                            ,
 ApplyNberofPrecisionDigitsCut                    = 1                            ,
 NberofPrecisionDigitsCut                         = 4                            ,
 ApplyNberofDifferentDigitsCut                    = 0                            ,
 NberofDifferentDigitsCut                         = 4                            ,
 SelectedStationList                              = [  "BIL", "BIS", "BML", "BMS", "BOL", "BOS",
                                                       "BIR", "BIM", "BMF", "BOF", "BOG", "BOH", "BEE",
                                                       "EIL", "EIS", "EML", "EMS", "EOL", "EOS",
                                                       "CSS", "CSL", "EEL", "EES"             ],          
 SelectSegmentsInsideEtaRegion                    = 0                            ,
 EtaMinCut                                        = 0.                           ,
 EtaMaxCut                                        = 3.                           ,
 ApplyNberofMultiLayersCut                        = 0                            ,
 NberofHitsPerMultiLayer                          = 2                            ,
 ApplyNberofHolesCut                              = 1                            ,
 NberofHoles                                      = 3                            )
ToolSvc += ConfiguredForAllStationsSegmentsFilterTool
#print ConfiguredForAllStationsSegmentsFilterTool

from MuTagTools.MuTagToolsConf import TrackParticleFilterTool
ConfiguredForAllStationsTrackParticleFilterTool = TrackParticleFilterTool(
    name = "ConfiguredForAllStationsTrackParticleFilterTool"                 ,
    RejectTracksAgainstCbMuonContainer        = 0                            ,
    TracksShouldNotBeInThisCbMuonContainer    = "NoContainer"                ,
    RejectTracksAgainstMuTagMuonContainer     = 1                            ,
    TracksShouldNotBeInThisMuTagMuonContainer = "MuTagIMOMuons"              ,
    SelectTracksInsideEtaRegion               = 0                            ,
    EtaMinCut                                 = 0.                           ,
    EtaMaxCut                                 = 3.                           ,
    CutP                                      = 3000                         ,
    requireInDetQualityCut                    = True                         ,
    QualityEstimator                          = 1                            ,
    minPixelHits                              = 1                            ,
    minSctHits                                = 3                            ,
    maxD0                                     = 9999.                        ,
    DoPtCut                                   = True                         ,
    CutPt                                     = 2000.                        ,
    ITrackSelectorTool                        = MuTagIMOInDetDetailedTrackSelectorTool
    )

ToolSvc += ConfiguredForAllStationsTrackParticleFilterTool

#print ConfiguredForAllStationsTrackParticleFilterTool

from MuonSegmentTaggerTools.MuonSegmentTaggerToolsConf import MuTagMatchingTool
ConfiguredForIMOMatchingTool = MuTagMatchingTool(
    name              = "ConfiguredForIMOMatchingTool" ,
    AssumeLocalErrors = True                           ,
    PhiCut            =  30.                           ,
    GlobalPhiCut      =  1.                            ,
    ThetaCut          =  5.                            ,
    GlobalThetaCut    =  0.5                           ,
    ThetaAngleCut     =  5.                            ,
    DoDistanceCut     = True                           ,
    CombinedPullCut   = 3.0
    )
ToolSvc += ConfiguredForIMOMatchingTool

#print ConfiguredForIMOMatchingTool

from MuonSegmentTaggerTools.MuonSegmentTaggerToolsConf import MuTagAmbiguitySolverTool
ConfiguredMuTagAmbiguitySolverTool = MuTagAmbiguitySolverTool(
    name = "ConfiguredMuTagAmbiguitySolverTool",
    RejectOuterEndcap = True                   ,
    RejectMatchPhi = True
    )
ToolSvc += ConfiguredMuTagAmbiguitySolverTool
#print ConfiguredMuTagAmbiguitySolverTool

from MuonSegmentTaggerTools.MuonSegmentTaggerToolsConf import DoMuTaggedSegmentTool
ConfiguredForAllStationsDoMuTaggedSegmentTool = DoMuTaggedSegmentTool(
    name = "ConfiguredForAllStationsDoMuTaggedSegmentTool"
    )
ToolSvc += ConfiguredForAllStationsDoMuTaggedSegmentTool
#print ConfiguredForAllStationsDoMuTaggedSegmentTool

from MuTagTools.MuTagToolsConf import Trk__MuTagToTrackTool
ConfiguredMuTagToSegmentsTrackTool = Trk__MuTagToTrackTool(
    name            = "ConfiguredMuTagToSegmentsTrackTool",
    AddSegmentHits  = False
)

ToolSvc += ConfiguredMuTagToSegmentsTrackTool
#print ConfiguredMuTagToSegmentsTrackTool

from MuTagTools.MuTagToolsConf import UpdateMuTagContainersTool
ConfiguredUpdateMuTagIMOContainersTool = UpdateMuTagContainersTool(
    name             = "ConfiguredUpdateMuTagIMOContainersTool" ,
    MuTagToTrackTool = "Trk::MuTagToTrackTool/ConfiguredMuTagToSegmentsTrackTool"
    )

ToolSvc += ConfiguredUpdateMuTagIMOContainersTool
#print ConfiguredUpdateMuTagIMOContainersTool

from MuonSegmentTaggerTools.MuonSegmentTaggerToolsConf import MuTagIMOTool
ConfiguredForAllStationsMuTagIMOTool = MuTagIMOTool (
    name            = "ConfiguredForAllStationsMuTagIMOTool",
    DoOverviewTable      = True                             ,
    DoPtDependentPullCut = True                             ,                   
    TriggerHitCut        = True                                               
    )
ToolSvc += ConfiguredForAllStationsMuTagIMOTool

    
#print ConfiguredForAllStationsMuTagIMOTool
