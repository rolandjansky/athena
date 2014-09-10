#
## @file TrackD3PDMaker/share/VertexGroupD3PD_loadConfig.py
## @brief Setup D3PD flags for vertex group D3PD production
## @author Simone Pagan Griso
## @date Mar, 2012
##

"""
Setup Vertex Group D3PD options for various objects.
"""

from RecExConfig.RecFlags import rec
from TrackD3PDMaker.VertexD3PDAnalysisFlags import VertexD3PDAnalysisFlags
from TrackD3PDMaker.VertexD3PDAnalysisKeys import VertexD3PDAnalysisKeys

from D3PDMakerConfig.D3PDMakerFlags  import D3PDMakerFlags
from TrackD3PDMaker.TrackD3PDMakerFlags import TrackD3PDFlags

### Set flags of D3PD fillers, etc..
## First our own flags
# Set truth flag
VertexD3PDAnalysisFlags.useTruth = rec.doTruth()
# Set storegate collection names (where default may be wrong)
from InDetRecExample.InDetKeys import InDetKeys
if rec.readESD():
  InDetKeys.UnslimmedTracks              = 'Tracks'
  InDetKeys.UnslimmedTracksTruth         = 'TrackTruthCollection'
if rec.readAOD():
  InDetKeys.Tracks = InDetKeys.TrackParticles()
  InDetKeys.TracksTruth = InDetKeys.TrackParticlesTruth()
  #print "InDetKeys.Tracks = "+InDetKeys.Tracks()
  #print "InDetKeys.TracksTruth = "+InDetKeys.TracksTruth()  
  InDetKeys.McEventCollection = 'GEN_AOD'
#print D3PDMakerFlags

# Set default prefix for existing vertex collection
from InDetRecExample.InDetJobProperties import InDetFlags
if (not VertexD3PDAnalysisFlags.useAllVertexCollections()) and (not InDetFlags.useBeamConstraint()):
  #Assume we're dumping NBC vertices
  VertexD3PDAnalysisKeys.LabelDefaultVtx = 'vxnbc'

## Switch off trigger-related objects for other info D3PD dumpers (e.g. MET, Muons, ...)
D3PDMakerFlags.DoTrigger=False

## Set track/vertex details
TrackD3PDFlags.doTruth = rec.doTruth()
TrackD3PDFlags.storeDiagonalCovarianceAsErrors = False
TrackD3PDFlags.storeHitTruthMatching = True
TrackD3PDFlags.storeDetailedTruth = False
TrackD3PDFlags.storeTrackMomentum = True
TrackD3PDFlags.storePullsAndResiduals = True
TrackD3PDFlags.storeTrackInfo = True
TrackD3PDFlags.storeTrackFitQuality = True
TrackD3PDFlags.trackParametersAtGlobalPerigeeLevelOfDetails = 3
TrackD3PDFlags.trackParametersAtPrimaryVertexLevelOfDetails = 2
TrackD3PDFlags.trackParametersAtBeamSpotLevelOfDetails = 2
TrackD3PDFlags.trackParametersAtBeamLineLevelOfDetails = 0
TrackD3PDFlags.storeTrackParametersAtCalo = False
TrackD3PDFlags.storeTrackUnbiasedIPAtPV = True
TrackD3PDFlags.storeTrackPredictionAtBLayer = False
TrackD3PDFlags.storeTrackSummary = True
TrackD3PDFlags.storeTrackSummary.IDHits = True
TrackD3PDFlags.storeTrackSummary.IDHoles = True
TrackD3PDFlags.storeTrackSummary.IDSharedHits = True
TrackD3PDFlags.storeTrackSummary.IDOutliers = False
TrackD3PDFlags.storeTrackSummary.PixelInfoPlus = False
TrackD3PDFlags.storeTrackSummary.SCTInfoPlus = False
TrackD3PDFlags.storeTrackSummary.TRTInfoPlus = False
TrackD3PDFlags.storeTrackSummary.InfoPlus = False
TrackD3PDFlags.storeTrackSummary.MuonHits = False
TrackD3PDFlags.storeTrackSummary.MuonHoles = False
TrackD3PDFlags.storeTrackSummary.ExpectBLayer = True
TrackD3PDFlags.storeTrackSummary.HitSum = True
TrackD3PDFlags.storeTrackSummary.HoleSum = False
TrackD3PDFlags.storeTrackSummary.HitPattern = True
TrackD3PDFlags.storeTrackSummary.SiHits = False
TrackD3PDFlags.storeTrackSummary.TRTRatio = True
TrackD3PDFlags.storeTrackSummary.PixeldEdx = False
TrackD3PDFlags.storeTrackSummary.ElectronPID = False
TrackD3PDFlags.storeBLayerHitsOnTrack = False
TrackD3PDFlags.storePixelHitsOnTrack = False
if TrackD3PDFlags.storePixelHitsOnTrack():
    TrackD3PDFlags.storeBLayerHitsOnTrack = False
#TrackD3PDFlags.storeDetailedTrackPredictionAtBLayer = False
TrackD3PDFlags.storeSCTHitsOnTrack = False
TrackD3PDFlags.storeTRTHitsOnTrack = False
TrackD3PDFlags.storeBLayerOutliersOnTrack = False
TrackD3PDFlags.storePixelOutliersOnTrack = False
TrackD3PDFlags.storeSCTOutliersOnTrack = False
TrackD3PDFlags.storeTRTOutliersOnTrack = False
TrackD3PDFlags.storeBLayerHolesOnTrack = False
TrackD3PDFlags.storePixelHolesOnTrack = False
TrackD3PDFlags.storeSCTHolesOnTrack = False
TrackD3PDFlags.storeTRTHolesOnTrack = False
TrackD3PDFlags.storeVertexAssociation = True
TrackD3PDFlags.vertexPositionLevelOfDetails = 3
TrackD3PDFlags.storeVertexType = True
TrackD3PDFlags.storeVertexFitQuality = True
TrackD3PDFlags.storeVertexKinematics = True
TrackD3PDFlags.storeVertexPurity = False
TrackD3PDFlags.storeVertexTrackAssociation = False
TrackD3PDFlags.storeVertexTrackIndexAssociation = True
if not VertexD3PDAnalysisFlags.useTracks():
    #If not storing tracks, do not store track-vertex association as well
    TrackD3PDFlags.storeVertexTrackAssociation = False
    TrackD3PDFlags.storeVertexTrackIndexAssociation = False

# b-tagging info
from JetTagD3PDMaker.JetTagD3PDMakerFlags import JetTagD3PDFlags
JetTagD3PDFlags.Taggers.set_Value_and_Lock(['MV1', 'JetFitterCOMBNN'])

### Use special VertexD3PDAnalysisFlags to trigger specific options
