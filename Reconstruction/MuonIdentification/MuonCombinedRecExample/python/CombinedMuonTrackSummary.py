# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

###########################################################################
#
#  CombinedMuonTrackSummary:
#   ensure full configuration of InDet and MuonSpectrometer track summaries
#
#==========================================================================

from AthenaCommon.DetFlags  import DetFlags
from AthenaCommon.AppMgr    import ToolSvc
from AthenaCommon.BeamFlags import jobproperties
from MuonCombinedRecExample.MuonCombinedRecFlags import muonCombinedRecFlags
from AthenaCommon.CfgGetter import getPublicTool

beamFlags                      = jobproperties.Beam

atlasExtrapolator              = getPublicTool('AtlasExtrapolator')
muonTrackSummaryHelper         = getPublicTool('MuonTrackSummaryHelperTool')

from AthenaConfiguration.AllConfigFlags import ConfigFlags
from InDetRecExample import TrackingCommon

if ConfigFlags.Muon.MuonTrigger:
  # @TODO change name ?
  TestPixelLayerTool = TrackingCommon.getInDetTrigTestPixelLayerTool("CombinedMuonPixelLayerToolDefault",
                                                                     Extrapolator = atlasExtrapolator)
else :
  TestPixelLayerTool = TrackingCommon.getInDetTestPixelLayerTool("CombinedMuonPixelLayerToolDefault",
                                                                 Extrapolator = atlasExtrapolator)

from InDetBoundaryCheckTool.InDetBoundaryCheckToolConf import InDet__InDetBoundaryCheckTool
CombinedMuonIDBoundaryCheckTool = InDet__InDetBoundaryCheckTool(
    name="CombinedMuonIDBoundaryCheckTool",
    UsePixel=DetFlags.haveRIO.pixel_on(),
    UseSCT=DetFlags.haveRIO.SCT_on(),
    PixelLayerTool = TestPixelLayerTool
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
  DoSharedHits    = False,
  HoleSearch      = ToolSvc.CombinedMuonIDHoleSearch,
  usePixel        = DetFlags.haveRIO.pixel_on(),
  useSCT          = DetFlags.haveRIO.SCT_on(),
  useTRT          = DetFlags.haveRIO.TRT_on(),
  ClusterSplitProbabilityName = TrackingCommon.combinedClusterSplitProbName())

#@TODO the value of the ClusterSplitProbabilityName depends on the input ID tracks that have been used
#      for the combined muons. So, this tool is not universally usable. For example online muons are
#      based on tracks for which the cluster splitting probabilities are not computed. For such cases
#      the property should be set to an empty string.

# default CombinedMuonTrackSummary
# @TODO doHolesMuon = False correct ?
from TrkTrackSummaryTool.TrkTrackSummaryToolConf import Trk__TrackSummaryTool
ToolSvc += Trk__TrackSummaryTool( 'CombinedMuonTrackSummary',
                                  doSharedHits             = False,
                                  doHolesInDet             = True,
                                  doHolesMuon              = False,
                                  AddDetailedMuonSummary   = True,
                                  InDetSummaryHelperTool   = ToolSvc.CombinedMuonIDSummaryHelper,
                                  MuonSummaryHelperTool    = muonTrackSummaryHelper,
                                  PixelExists              = True )

  
if DetFlags.haveRIO.SCT_on():
  from SCT_ConditionsTools.SCT_ConditionsSummaryToolSetup import SCT_ConditionsSummaryToolSetup
  sct_ConditionsSummaryToolSetup = SCT_ConditionsSummaryToolSetup()
  sct_ConditionsSummaryToolSetup.setup()
  InDetSCT_ConditionsSummaryTool = sct_ConditionsSummaryToolSetup.getTool()
  ToolSvc.CombinedMuonIDBoundaryCheckTool.SctSummaryTool = InDetSCT_ConditionsSummaryTool

