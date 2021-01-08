
# =====================================================
# THIS IS WHERE THE USER CONTROLS MAIN TRACK SELECTIONS
# =====================================================
from InDetTrackSelectorTool.InDetTrackSelectorToolConf import InDet__InDetDetailedTrackSelectorTool
InDetTrackSelectorToolEE = InDet__InDetDetailedTrackSelectorTool(name = "InDetDetailedTrackSelectorToolEE",
                                                               pTMin                = 400.0,
                                                               IPd0Max              = 10000.0,
                                                               IPz0Max              = 10000.0,
                                                               z0Max                = 10000.0,
                                                               sigIPd0Max           = 10000.0,
                                                               sigIPz0Max           = 10000.0,
                                                               d0significanceMax    = -1.,
                                                               z0significanceMax    = -1.,
                                                               etaMax               = 9999.,
                                                               useTrackSummaryInfo  = True,
                                                               nHitBLayer           = 0,
                                                               nHitPix              = 1,
                                                               nHitBLayerPlusPix    = 1,
                                                               nHitSct              = 0,
                                                               nHitSi               = 7,
                                                               nHitTrt              = 0,
                                                               nHitTrtHighEFractionMax = 10000.0,
                                                               useSharedHitInfo     = False,
                                                               useTrackQualityInfo  = True,
                                                               fitChi2OnNdfMax      = 10000.0,
                                                               TrtMaxEtaAcceptance  = 1.9,
                                                               TrackSummaryTool     = InDetTrackSummaryTool,
                                                               Extrapolator         = InDetExtrapolator
                                                              )

ToolSvc+=InDetTrackSelectorToolEE
print      InDetTrackSelectorToolEE

#-----------------------------------------------
# SET UP TOOLS NEEDED FOR EGAMMA BREM REFITTING
#-----------------------------------------------

#include( "egammaBremRec/ElectronRefit_jobOptions.py" )
#
#from egammaTrackTools.egammaTrackToolsConf import CaloCluster_OnTrackBuilder
#ccotBuilderTool = CaloCluster_OnTrackBuilder(name = "ccotBuilderTool",
#                                             UseClusterEnergy = False,
#                                             UseClusterPhi = False)
#ToolSvc += ccotBuilderTool
#
#print ccotBuilderTool
#
#from egammaTrackTools.egammaTrackToolsConf import egammaTrkRefitterTool
#electronRefitterToolGSFCalo = egammaTrkRefitterTool(name = 'electronRefitterToolGSFCalo',
#                                                  FitterTool = GSFTrackFitter,
#                                                  CCOTBuilder = ccotBuilderTool,
#                                                  useBeamSpot = False,
#                                                  useClusterPosition = True,
#                                                  OutputLevel = 4
#                                                  )
#
#ToolSvc += electronRefitterToolGSFCalo
#
#print electronRefitterToolGSFCalo
#
#include("egammaBremRec/ElectronAnalysisCommonTools.py")

from TrkParticleCreator.TrkParticleCreatorConf import Trk__TrackParticleCreatorTool
InDetParticleCreatorTool = Trk__TrackParticleCreatorTool(name = "InDetParticleCreatorTool",
                                                         KeepParameters = True,
                                                         TrackSummaryTool = InDetTrackSummaryTool,
                                                         UseTrackSummaryTool = True)
ToolSvc += InDetParticleCreatorTool
