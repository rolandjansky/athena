# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

__doc__ = "ToolFactories to instantiate InDet tools for egamma with default configuration"
__author__ = "Bruno Lenzi"

from egammaRec.Factories import FcnWrapper, ToolFactory, PublicToolFactory
#########
# Tools for extrapolating to the calo
def configureExtrapolator( egammaExtrapolator ):

  from AthenaCommon.AppMgr import ToolSvc
  
  # this turns off dynamic calculation of eloss in calorimeters
  egammaExtrapolator.DoCaloDynamic = False 
    
  # all left to MaterialEffects/EnergyLossUpdators

  from TrkExTools.TrkExToolsConf import Trk__MaterialEffectsUpdator as MaterialEffectsUpdator
  
  AtlasMaterialEffectsUpdator = MaterialEffectsUpdator(name = 'AtlasMaterialEffectsUpdator')
  NoElossMaterialEffectsUpdator = MaterialEffectsUpdator(name = 'NoElossMaterialEffectsUpdator')
  NoElossMaterialEffectsUpdator.EnergyLoss = False

  if not hasattr(ToolSvc,'AtlasMaterialEffectsUpdator'):
    ToolSvc += AtlasMaterialEffectsUpdator 
  if  not hasattr(ToolSvc,'NoElossMaterialEffectsUpdator'):  
    ToolSvc += NoElossMaterialEffectsUpdator

  MyUpdators = []
  MyUpdators += [AtlasMaterialEffectsUpdator] 
  MyUpdators += [NoElossMaterialEffectsUpdator]
  
  MySubUpdators = []
  MySubUpdators += [AtlasMaterialEffectsUpdator.name()] # for ID
  MySubUpdators += [NoElossMaterialEffectsUpdator.name()] # for Calo
  MySubUpdators += [NoElossMaterialEffectsUpdator.name()] # for muon spectrometer
  
  egammaExtrapolator.MaterialEffectsUpdators = MyUpdators
  egammaExtrapolator.SubMEUpdators = MySubUpdators

###############
#egammaExtrapolator
from TrkExTools.AtlasExtrapolator import AtlasExtrapolator
egammaExtrapolator = PublicToolFactory(AtlasExtrapolator,
                                       postInit=[configureExtrapolator],
                                       name = 'egammaExtrapolator')

#################################################################
# egamma InDet summary tool to be used conversion finding
#
# load association tool from Inner Detector to handle pixel ganged
# ambiguities
from InDetAssociationTools.InDetAssociationToolsConf import InDet__InDetPRD_AssociationToolGangedPixels
egammaInDetPrdAssociationTool = PublicToolFactory( InDet__InDetPRD_AssociationToolGangedPixels,
                                                   name                           = "egammaInDetPrdAssociationTool",
                                                   PixelClusterAmbiguitiesMapName = 'PixelClusterAmbiguitiesMap')

# Loading Configurable HoleSearchTool
#
from InDetTrackHoleSearch.InDetTrackHoleSearchConf import InDet__InDetTrackHoleSearchTool
egammaInDetHoleSearchTool = PublicToolFactory( InDet__InDetTrackHoleSearchTool,
                                               name = "egammaInDetHoleSearchTool",
                                               Extrapolator = egammaExtrapolator)
#
# Load the InDetTrackSummaryHelperTool
#
from AthenaCommon.DetFlags import DetFlags
from InDetTrackSummaryHelperTool.InDetTrackSummaryHelperToolConf import InDet__InDetTrackSummaryHelperTool
egammaInDetTrackSummaryHelperTool = PublicToolFactory( InDet__InDetTrackSummaryHelperTool,
                                                       name         = "egammaInDetSummaryHelper",
                                                       AssoTool     = egammaInDetPrdAssociationTool,
                                                       DoSharedHits = False,
                                                       HoleSearch   = egammaInDetHoleSearchTool,
                                                       usePixel     = DetFlags.haveRIO.pixel_on(),
                                                       useSCT       = DetFlags.haveRIO.SCT_on(),
                                                       useTRT       = DetFlags.haveRIO.TRT_on())

#
from TrkTrackSummaryTool.TrkTrackSummaryToolConf import Trk__TrackSummaryTool
egammaInDetTrackSummaryTool = PublicToolFactory( Trk__TrackSummaryTool,
                                                 name = "egammaInDetTrackSummaryTool",
                                                 InDetSummaryHelperTool = egammaInDetTrackSummaryHelperTool,
                                                 doSharedHits           = False,
                                                 InDetHoleSearchTool    = egammaInDetHoleSearchTool)

#################################################################
# Configure InDet__ConversionFinderUtils  and InDet__SingleTrackConversionTool 
# Helper Tool
def getTrkExtrapolator():
  import egammaRec.EMCommonRefitter
  return egammaRec.EMCommonRefitter.egTrkExtrapolator

from InDetConversionFinderTools import InDetConversionFinderToolsConf
InDet__ConversionFinderUtils = PublicToolFactory( InDetConversionFinderToolsConf.InDet__ConversionFinderUtils)

#
# Single track conversion tool
#
InDet__SingleTrackConversionTool = PublicToolFactory( InDetConversionFinderToolsConf.InDet__SingleTrackConversionTool,
                                                      name                       = "CPSingleTrackConversionTool",
                                                      ConversionFinderHelperTool = InDet__ConversionFinderUtils,
                                                      TrackSummaryTool           = egammaInDetTrackSummaryTool,
                                                      Extrapolator               = FcnWrapper(getTrkExtrapolator),
                                                      MinInitialHitRadius        = 70.,
                                                      MinRatioOfHLhits           = 0.95)

