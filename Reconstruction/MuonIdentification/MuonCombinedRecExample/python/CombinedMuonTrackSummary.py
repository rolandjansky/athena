# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

###########################################################################
#
#  CombinedMuonTrackSummary:
#   ensure full configuration of InDet and MuonSpectrometer track summaries
#
#==========================================================================

from AthenaCommon.DetFlags  import DetFlags
from AthenaCommon.AppMgr    import ServiceMgr
from AthenaCommon.AppMgr    import ToolSvc
from AthenaCommon.BeamFlags import jobproperties
from MuonCombinedRecExample.MuonCombinedRecFlags import muonCombinedRecFlags
from AthenaCommon.CfgGetter import getPublicTool
from AthenaCommon.GlobalFlags import globalflags

beamFlags                      = jobproperties.Beam

atlasExtrapolator              = getPublicTool('AtlasExtrapolator')
muonTrackSummaryHelper         = getPublicTool('MuonTrackSummaryHelperTool')


from InDetBoundaryCheckTool.InDetBoundaryCheckToolConf import InDet__InDetBoundaryCheckTool
CombinedMuonIDBoundaryCheckTool = InDet__InDetBoundaryCheckTool(
    name="CombinedMuonIDBoundaryCheckTool",
    UsePixel=DetFlags.haveRIO.pixel_on(),
    UseSCT=DetFlags.haveRIO.SCT_on()
)
ToolSvc += CombinedMuonIDBoundaryCheckTool

# load InDetHoleSearchTool
from InDetTrackHoleSearch.InDetTrackHoleSearchConf import InDet__InDetTrackHoleSearchTool
ToolSvc += InDet__InDetTrackHoleSearchTool( \
  name                         = "CombinedMuonIDHoleSearch",
  Extrapolator                 = atlasExtrapolator,
  BoundaryCheckTool            = CombinedMuonIDBoundaryCheckTool,
  CountDeadModulesAfterLastHit = True)

import InDetRecExample.TrackingCommon as TrackingCommon
InDetPixelConditionsSummaryTool = TrackingCommon.getInDetPixelConditionsSummaryTool()


if muonCombinedRecFlags.useDetailedPixelHoleSearch():
  # now get the InDet tools as used for InDet tracks
  #   (duplication for ESD running in case InDet not rerun)
  #   attention: defaults defined first with some DetFlag specific properties added further down
  from InDetTestPixelLayer.InDetTestPixelLayerConf import InDet__InDetTestPixelLayerTool
  ToolSvc += InDet__InDetTestPixelLayerTool( 
    name = "CombinedMuonInDetTestPixelLayerTool",
    Extrapolator = atlasExtrapolator,
    CheckActiveAreas = True,
    CheckDeadRegions = True
    )
  ToolSvc.CombinedMuonIDHoleSearch.PixelLayerTool = ToolSvc.CombinedMuonInDetTestPixelLayerTool




if beamFlags.beamType() == 'cosmics':
  ToolSvc.CombinedMuonIDHoleSearch.Cosmics = True

# default InDetTrackSummaryHelperTool
from InDetTrackSummaryHelperTool.InDetTrackSummaryHelperToolConf import InDet__InDetTrackSummaryHelperTool
ToolSvc += InDet__InDetTrackSummaryHelperTool( \
  name            = "CombinedMuonIDSummaryHelper",
  AssoTool        = None,
  PixelToTPIDTool = None,
  TestBLayerTool  = None,
  DoSharedHits    = False,
  HoleSearch      = ToolSvc.CombinedMuonIDHoleSearch,
  usePixel        = DetFlags.haveRIO.pixel_on(),
  useSCT          = DetFlags.haveRIO.SCT_on(),
  useTRT          = DetFlags.haveRIO.TRT_on(),
  ClusterSplitProbabilityName = TrackingCommon.combinedClusterSplitProbName())

# default CombinedMuonTrackSummary
# @TODO doHolesMuon = False correct ?
from TrkTrackSummaryTool.TrkTrackSummaryToolConf import Trk__TrackSummaryTool
ToolSvc += Trk__TrackSummaryTool( 'CombinedMuonTrackSummary',
                                  doSharedHits             = False,
                                  doHolesInDet             = True,
                                  doHolesMuon              = False,
                                  AddDetailedInDetSummary  = True,
                                  AddDetailedMuonSummary   = True,
                                  InDetSummaryHelperTool   = ToolSvc.CombinedMuonIDSummaryHelper,
                                  TRT_ElectronPidTool      = None,
                                  PixelToTPIDTool          = None,
                                  MuonSummaryHelperTool    = muonTrackSummaryHelper,
                                  PixelExists              = True )

# load tools requiring pixels
if DetFlags.haveRIO.pixel_on():
  # use AssoTool for shared hits (provided we have RDO)
  # commented as this doesn't work...
  # from RecExConfig.RecFlags import rec
  # if rec.readRDO:
  #  #ToolSvc.CombinedMuonIDSummaryHelper.AssoTool  = ToolSvc.InDetPrdAssociationTool
  #  ToolSvc.CombinedMuonTrackSummary.doSharedHits = True
    
  # load BLayer tool
  from InDetTestBLayer.InDetTestBLayerConf import InDet__InDetTestBLayerTool
  ToolSvc += InDet__InDetTestBLayerTool( \
    name                       = "CombinedMuonTestBLayer",
    PixelSummaryTool           = InDetPixelConditionsSummaryTool,
    Extrapolator               = atlasExtrapolator)

  # load PixelToTPID tool
  from PixelToTPIDTool.PixelToTPIDToolConf import InDet__PixelToTPIDTool
  ToolSvc += InDet__PixelToTPIDTool( \
    name                       = "CombinedMuonPixelToTPID")

  # set properties into public tools
  ToolSvc.CombinedMuonIDSummaryHelper.PixelToTPIDTool = ToolSvc.CombinedMuonPixelToTPID
  ToolSvc.CombinedMuonIDSummaryHelper.TestBLayerTool  = ToolSvc.CombinedMuonTestBLayer
  ToolSvc.CombinedMuonTrackSummary.PixelToTPIDTool    = ToolSvc.CombinedMuonPixelToTPID

  
if DetFlags.haveRIO.SCT_on():
  from SCT_ConditionsTools.SCT_ConditionsSummaryToolSetup import SCT_ConditionsSummaryToolSetup
  sct_ConditionsSummaryToolSetup = SCT_ConditionsSummaryToolSetup()
  sct_ConditionsSummaryToolSetup.setup()
  InDetSCT_ConditionsSummaryTool = sct_ConditionsSummaryToolSetup.getTool()
  ToolSvc.CombinedMuonIDBoundaryCheckTool.SctSummaryTool = InDetSCT_ConditionsSummaryTool

