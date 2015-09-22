# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

__doc__ = "ToolFactories to instantiate InDet tools for egamma with default configuration"
__author__ = "Bruno Lenzi"

from egammaRec.Factories import FcnWrapper, ToolFactory

#########
# What used to be egammaRec/python/EMCommonExtrapolator

def configureExtrapolator( egammaExtrapolator ):
  from AthenaCommon.AppMgr import ToolSvc
  
  egammaExtrapolator.DoCaloDynamic = False # this turns off dynamic calculation of eloss in calorimeters
  # all left to MaterialEffects/EnergyLossUpdators

  from TrkExTools.TrkExToolsConf import Trk__MaterialEffectsUpdator as MaterialEffectsUpdator
  AtlasMaterialEffectsUpdator = MaterialEffectsUpdator(name = 'AtlasMaterialEffectsUpdator')
  ToolSvc += AtlasMaterialEffectsUpdator #default material effects updator
  NoElossMaterialEffectsUpdator = MaterialEffectsUpdator(name = 'NoElossMaterialEffectsUpdator')
  NoElossMaterialEffectsUpdator.EnergyLoss = False
  ToolSvc += NoElossMaterialEffectsUpdator

  # setup MaterialEffectsUpdator arrays
  MyUpdators = []
  MyUpdators += [AtlasMaterialEffectsUpdator] # for ID
  MyUpdators += [NoElossMaterialEffectsUpdator] # for Calo
  # MyUpdators += [NoElossMaterialEffectsUpdator] # for muon

  MySubUpdators = []
  MySubUpdators += [AtlasMaterialEffectsUpdator.name()] # for ID
  MySubUpdators += [NoElossMaterialEffectsUpdator.name()] # for Calo
  MySubUpdators += [NoElossMaterialEffectsUpdator.name()] # for muon

  egammaExtrapolator.MaterialEffectsUpdators = MyUpdators
  egammaExtrapolator.SubMEUpdators = MySubUpdators

from TrkExTools.AtlasExtrapolator import AtlasExtrapolator
egammaExtrapolator = ToolFactory(AtlasExtrapolator,
  postInit=[configureExtrapolator],
  name = 'egammaExtrapolator')


#########
# What used to be egammaRec/python/EMCommonTrackSummary.py

#
# ----------- load association tool from Inner Detector to handle pixel ganged ambiguities
#
from InDetAssociationTools.InDetAssociationToolsConf import InDet__InDetPRD_AssociationToolGangedPixels
egammaInDetPrdAssociationTool = ToolFactory( InDet__InDetPRD_AssociationToolGangedPixels,
  doPrint = True,
  name                           = "egammaInDetPrdAssociationTool",
  PixelClusterAmbiguitiesMapName = 'PixelClusterAmbiguitiesMap')

#
# Loading Configurable HoleSearchTool
#
from InDetTrackHoleSearch.InDetTrackHoleSearchConf import InDet__InDetTrackHoleSearchTool
egammaInDetHoleSearchTool = ToolFactory( InDet__InDetTrackHoleSearchTool,
                                         doPrint = True,
                                         name = "egammaInDetHoleSearchTool",
                                         Extrapolator = egammaExtrapolator)

#
# Load the InDetTrackSummaryHelperTool
#
from AthenaCommon.DetFlags import DetFlags
from InDetTrackSummaryHelperTool.InDetTrackSummaryHelperToolConf import InDet__InDetTrackSummaryHelperTool
egammaInDetTrackSummaryHelperTool = ToolFactory( InDet__InDetTrackSummaryHelperTool,
                       doPrint = True,
                       name         = "egammaInDetSummaryHelper",
								       AssoTool     = egammaInDetPrdAssociationTool,
								       DoSharedHits = False,
								       HoleSearch   = egammaInDetHoleSearchTool,
                       usePixel     = DetFlags.haveRIO.pixel_on(),
                       useSCT       = DetFlags.haveRIO.SCT_on(),
                       useTRT       = DetFlags.haveRIO.TRT_on() )

#
from TrkTrackSummaryTool.TrkTrackSummaryToolConf import Trk__TrackSummaryTool
egammaInDetTrackSummaryTool = ToolFactory( Trk__TrackSummaryTool,
                doPrint = True,
                name = "egammaInDetTrackSummaryTool",
						    InDetSummaryHelperTool = egammaInDetTrackSummaryHelperTool,
						    doSharedHits           = False,
						    InDetHoleSearchTool    = egammaInDetHoleSearchTool)

#########
# What used to be egammaRec/python/EMCommonSingleTrackConversionTool.py

def getTrkExtrapolator():
  import egammaRec.EMCommonRefitter
  return egammaRec.EMCommonRefitter.egTrkExtrapolator

# Helper Tool
#
from InDetConversionFinderTools import InDetConversionFinderToolsConf
InDet__ConversionFinderUtils = ToolFactory( InDetConversionFinderToolsConf.InDet__ConversionFinderUtils)

#
# Single track conversion tool
#
InDet__SingleTrackConversionTool = ToolFactory( InDetConversionFinderToolsConf.InDet__SingleTrackConversionTool,
  name                       = "CPSingleTrackConversionTool",
  ConversionFinderHelperTool = InDet__ConversionFinderUtils,
  TrackSummaryTool           = egammaInDetTrackSummaryTool,
  Extrapolator               = FcnWrapper(getTrkExtrapolator),
  MinInitialHitRadius        = 70.,
  MinRatioOfHLhits           = 0.95)

