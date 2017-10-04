# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

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
muonTrackSummaryHelper         = getPublicTool('MuonTrackSummaryHelper')


# load InDetHoleSearchTool
from InDetTrackHoleSearch.InDetTrackHoleSearchConf import InDet__InDetTrackHoleSearchTool
ToolSvc += InDet__InDetTrackHoleSearchTool( \
  name                         = "CombinedMuonIDHoleSearch",
  Extrapolator                 = atlasExtrapolator,
  usePixel                     = DetFlags.haveRIO.pixel_on(),
  useSCT                       = DetFlags.haveRIO.SCT_on(),
  CountDeadModulesAfterLastHit = True)

if muonCombinedRecFlags.useDetailedPixelHoleSearch():
  # now get the InDet tools as used for InDet tracks
  #   (duplication for ESD running in case InDet not rerun)
  #   attention: defaults defined first with some DetFlag specific properties added further down
  from InDetTestPixelLayer.InDetTestPixelLayerConf import InDet__InDetTestPixelLayerTool
  ToolSvc += InDet__InDetTestPixelLayerTool( 
    name = "CombinedMuonInDetTestPixelLayerTool",
    Extrapolator = atlasExtrapolator,
    PixelSummarySvc = ServiceMgr.PixelConditionsSummarySvc,
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
  useTRT          = DetFlags.haveRIO.TRT_on() )
 
# default CombinedMuonTrackSummary
from TrkTrackSummaryTool.TrkTrackSummaryToolConf import Trk__TrackSummaryTool
ToolSvc += Trk__TrackSummaryTool( 'CombinedMuonTrackSummary',
                                  doSharedHits             = False,
                                  doHolesMuon              = False,
                                  AddDetailedInDetSummary  = True,
                                  AddDetailedMuonSummary   = True,
                                  InDetSummaryHelperTool   = ToolSvc.CombinedMuonIDSummaryHelper,
                                  TRT_ElectronPidTool      = None,
                                  PixelToTPIDTool          = None,
                                  MuonSummaryHelperTool    = muonTrackSummaryHelper,
                                  InDetHoleSearchTool      = ToolSvc.CombinedMuonIDHoleSearch,
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
    PixelSummarySvc            = ServiceMgr.PixelConditionsSummarySvc,
    Extrapolator               = atlasExtrapolator)

  # load PixelToTPID tool
  from PixelToTPIDTool.PixelToTPIDToolConf import InDet__PixelToTPIDTool
  ToolSvc += InDet__PixelToTPIDTool( \
    name                       = "CombinedMuonPixelToTPID",
    ReadFromCOOL               = True)

  # set properties into public tools
  ToolSvc.CombinedMuonIDHoleSearch.PixelSummarySvc    = ServiceMgr.PixelConditionsSummarySvc  
  ToolSvc.CombinedMuonIDSummaryHelper.PixelToTPIDTool = ToolSvc.CombinedMuonPixelToTPID
  ToolSvc.CombinedMuonIDSummaryHelper.TestBLayerTool  = ToolSvc.CombinedMuonTestBLayer
  ToolSvc.CombinedMuonTrackSummary.PixelToTPIDTool    = ToolSvc.CombinedMuonPixelToTPID

  
if DetFlags.haveRIO.SCT_on():
  ToolSvc.CombinedMuonIDHoleSearch.SctSummarySvc      = ServiceMgr.InDetSCT_ConditionsSummarySvc

# check configuration
#print ToolSvc.CombinedMuonIDHoleSearch
#print ToolSvc.CombinedMuonIDSummaryHelper
#print ToolSvc.CombinedMuonTrackSummary
#import sys
#sys.exit()

#class CombinedMuonTrackSummary( Trk__TrackSummaryTool ):

#  # constructor
#  def __init__(self, name = 'CombinedMuonTrackSummary'):
#    global ToolSvc
#    # call the base class constructor
#    self.trackSummary = ToolSvc.CombinedMuonTrackSummary





