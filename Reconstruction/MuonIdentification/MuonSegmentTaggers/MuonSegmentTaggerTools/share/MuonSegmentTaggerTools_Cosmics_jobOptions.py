include.block ( "MuonSegmentTaggerTools/MuonSegmentTaggerTools_Cosmics_jobOptions.py" )
#-----------------------------------------------------------------
from TrkExTools.AtlasExtrapolator import AtlasExtrapolator
MuTagExtrapolator = AtlasExtrapolator()
from TrackToSegmentTools.TrackToSegmentToolsConf import TrackToSegmentTool
ConfiguredForAllStationsTrackToSegmentTool = TrackToSegmentTool(
    name                  = "ConfiguredForAllStationsTrackToSegmentTool" ,
    IExtrapolator         = MuTagExtrapolator                            ,
    addMultipleScattering = 1                                            ,
    ReverseIDTrack        = 0                                            )
ToolSvc += ConfiguredForAllStationsTrackToSegmentTool
#print ConfiguredForAllStationsTrackToSegmentTool

#--------------------------------------------------------------------------------------------------------------------------
from TrkTrackSummaryTool.AtlasTrackSummaryTool import AtlasTrackSummaryTool
TrkTrackSummaryToolFORMuTagIMOInDetDetailedTrackSelectorTool = AtlasTrackSummaryTool()
ToolSvc += TrkTrackSummaryToolFORMuTagIMOInDetDetailedTrackSelectorTool

from InDetTrackSelectorTool.InDetTrackSelectorToolConf import InDet__InDetDetailedTrackSelectorTool

from AthenaCommon.BeamFlags import jobproperties
MuTagIMOInDetDetailedTrackSelectorTool = InDet__InDetDetailedTrackSelectorTool(
      name                 = "MuTagIMOInDetDetailedTrackSelectorTool",
      pTMin                = 0*MeV,
      IPd0Max              = 9999.0*mm,
      IPz0Max              = 9999.0*mm,
      z0Max                = 9999.0*mm,
      useTrackSummaryInfo  = False,
      useTrackQualityInfo  = False,
      TrackSummaryTool = TrkTrackSummaryToolFORMuTagIMOInDetDetailedTrackSelectorTool,
      Extrapolator = MuTagExtrapolator)

ToolSvc +=MuTagIMOInDetDetailedTrackSelectorTool
#---#print MuTagIMOInDetDetailedTrackSelectorTool
#--------------------------------------------------------------------------------------------------------------------------

from MuTagTools.MuTagToolsConf import SegmentsFilterTool
ConfiguredForAllStationsSegmentsFilterTool = SegmentsFilterTool(
    name = "ConfiguredForAllStationsSegmentsFilterTool",
 RejectSegmentsAgainstCbMuonContainer             = 0                            ,
 SegmentShouldNotBeInTracksOfThisCbMuonContainer  = "NoContainer"                ,
 ApplyQualityFactorCut                            = 0                            ,
 QualityFactorCut                                 = 9.                           ,
 ApplyNberofDigitsCut                             = 0                            ,
 NberofDigitsCut                                  = 4                            ,
 ApplyNberofPrecisionDigitsCut                    = 0                            ,
 NberofPrecisionDigitsCut                         = 4                            ,
 ApplyNberofDifferentDigitsCut                    = 0                            ,
 NberofDifferentDigitsCut                         = 4                            ,
 SelectedStationList                              = [  "BIL", "BIS", "BML", "BMS", "BOL", "BOS",
                                                       "BIR", "BIM", "BMF", "BOF", "BOG", "BOH", "BEE",
                                                       "EIL", "EIS", "EML", "EMS", "EOL", "EOS",
                                                       "CSS", "CSL"             ],          
 SelectSegmentsInsideEtaRegion                    = 0                            ,
 EtaMinCut                                        = 0.                           ,
 EtaMaxCut                                        = 3.                           ,
 ApplyNberofMultiLayersCut                        = 0                            ,
 NberofHitsPerMultiLayer                          = 2                            ,
 ApplyNberofHolesCut                              = 0                            ,
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
    CutP                                      = -1                           ,
    requireInDetQualityCut                    = False                        ,
    QualityEstimator                          = 1                            ,
    minPixelHits                              = 1                            ,
    minSctHits                                = 3                            ,
    maxD0                                     = 999999.                      ,
    DoPtCut                                   = False                        ,
    CutPt                                     = 2000.                        ,
    ITrackSelectorTool                        = MuTagIMOInDetDetailedTrackSelectorTool
    )

ToolSvc += ConfiguredForAllStationsTrackParticleFilterTool
#print ConfiguredForAllStationsTrackParticleFilterTool

from MuonSegmentTaggerTools.MuonSegmentTaggerToolsConf import MuTagMatchingTool
ConfiguredForIMOMatchingTool = MuTagMatchingTool(
    name              = "ConfiguredForIMOMatchingTool"          ,
#    IExtrapolator     = 'Trk::Extrapolator/CosmicsExtrapolator' ,
    AssumeLocalErrors = True                                    ,
    PhiCut            = 30.                                     ,
    ThetaCut          = 30.                                     ,
    ThetaAngleCut     = 50.                                     ,
    GlobalThetaCut    = 0.5                                     ,
    GlobalPhiCut      = 1.                                      ,
    CombinedPullCut   = 10.0                                    ,
    ExtrapolatePerigee= True

    )
ToolSvc += ConfiguredForIMOMatchingTool
#print ConfiguredForIMOMatchingTool

from MuonSegmentTaggerTools.MuonSegmentTaggerToolsConf import MuTagAmbiguitySolverTool
ConfiguredMuTagAmbiguitySolverTool = MuTagAmbiguitySolverTool(
    name              = "ConfiguredMuTagAmbiguitySolverTool"   ,
    RejectOuterEndcap = False                                  ,
    RejectMatchPhi = False
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
    name                         = "ConfiguredForAllStationsMuTagIMOTool",
    DoBidirectionalExtrapolation = True  ,
    DoOverviewTable              = True  ,
    DoPtDependentPullCut         = False ,
    TriggerHitCut                = False
    )
ToolSvc += ConfiguredForAllStationsMuTagIMOTool
#print ConfiguredForAllStationsMuTagIMOTool
