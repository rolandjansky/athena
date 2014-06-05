from JetTagD3PDMaker.JetTagD3PDMakerKeys import JetTagD3PDKeys
from JetTagD3PDMaker.JetTagD3PDMakerFlags import JetTagD3PDFlags
from D3PDMakerConfig.D3PDMakerFlags  import D3PDMakerFlags
from TrackD3PDMaker.TrackD3PDMakerFlags import TrackD3PDFlags
from TruthD3PDMaker.TruthD3PDMakerFlags import TruthD3PDFlags
from TruthD3PDMaker.TruthD3PDMakerKeys import TruthD3PDKeys
from RecExConfig.RecFlags import rec


### switch off all trigger objects included by MET
D3PDMakerFlags.DoTrigger=False

### configuration for the primary vertex object
TrackD3PDFlags.vertexPositionLevelOfDetails = 3
TrackD3PDFlags.storeDiagonalCovarianceAsErrors = True
TrackD3PDFlags.storeVertexFitQuality = True
TrackD3PDFlags.storeVertexKinematics = True
TrackD3PDFlags.storeVertexPurity = False
TrackD3PDFlags.storeVertexTrackAssociation = True
TrackD3PDFlags.storeVertexTrackIndexAssociation = True

### configuration for the track object

TrackD3PDFlags.doTruth = True
TrackD3PDFlags.storeHitTruthMatching = True
TrackD3PDFlags.storeDetailedTruth = False
TrackD3PDFlags.trackParametersAtGlobalPerigeeLevelOfDetails = 3
TrackD3PDFlags.trackParametersAtPrimaryVertexLevelOfDetails = 3
TrackD3PDFlags.trackParametersAtBeamSpotLevelOfDetails = 2
TrackD3PDFlags.storeTrackUnbiasedIPAtPV = True
TrackD3PDFlags.storeTrackMomentum = True
TrackD3PDFlags.storeTrackInfo = True
TrackD3PDFlags.storeTrackFitQuality = True
TrackD3PDFlags.storeTrackSummary = True
TrackD3PDFlags.storeTrackSummary.IDHits = True
TrackD3PDFlags.storeTrackSummary.IDHoles = True
TrackD3PDFlags.storeTrackSummary.IDSharedHits = True
TrackD3PDFlags.storeTrackSummary.IDOutliers = True
TrackD3PDFlags.storeTrackSummary.PixelInfoPlus = True
TrackD3PDFlags.storeTrackSummary.SCTInfoPlus = True
TrackD3PDFlags.storeTrackSummary.TRTInfoPlus = False
TrackD3PDFlags.storeTrackSummary.InfoPlus = False
TrackD3PDFlags.storeTrackSummary.MuonHits = False
TrackD3PDFlags.storeTrackSummary.MuonHoles = False
TrackD3PDFlags.storeTrackSummary.ExpectBLayer = True
TrackD3PDFlags.storeTrackSummary.HitSum = False
TrackD3PDFlags.storeTrackSummary.HoleSum = False
TrackD3PDFlags.storeTrackSummary.HitPattern = True
TrackD3PDFlags.storeTrackSummary.SiHits = True
TrackD3PDFlags.storeTrackSummary.TRTRatio = False
TrackD3PDFlags.storeTrackSummary.PixeldEdx = False
TrackD3PDFlags.storeTrackSummary.ElectronPID = False
TrackD3PDFlags.storePullsAndResiduals = True
TrackD3PDFlags.storeBLayerHitsOnTrack = True
TrackD3PDFlags.storePixelHitsOnTrack = True
if TrackD3PDFlags.storePixelHitsOnTrack():
    TrackD3PDFlags.storeBLayerHitsOnTrack = False
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
TrackD3PDFlags.storeTrackPredictionAtBLayer = True


### configure association labels
JetTagD3PDFlags.TrackAssocLabel = JetTagD3PDKeys.TrackGetterLabel()
JetTagD3PDFlags.MuonAssocLabel = JetTagD3PDKeys.MuonInJetGetterLabel()
JetTagD3PDFlags.Muon2AssocLabel = JetTagD3PDKeys.Muon2InJetGetterLabel()
JetTagD3PDFlags.ElectronAssocLabel = JetTagD3PDKeys.ElectronInJetGetterLabel()
JetTagD3PDFlags.PhotonAssocLabel = JetTagD3PDKeys.PhotonInJetGetterLabel()
JetTagD3PDFlags.JFVxOnJetAxisAssocLabel = JetTagD3PDKeys.JFVxOnJetAxisGetterLabel()
JetTagD3PDFlags.JFTwoTrackVertexAssocLabel = JetTagD3PDKeys.JFTwoTrackVertexGetterLabel()
TruthD3PDFlags.GenParticleAssocLabel = TruthD3PDKeys.GenParticleGetterLabel()
TruthD3PDFlags.GenVertexAssocLabel = TruthD3PDKeys.GenVertexGetterLabel()
TruthD3PDFlags.GenEventAssocLabel = TruthD3PDKeys.GenEventGetterLabel()
TruthD3PDFlags.TruthTrackAssocLabel = TruthD3PDKeys.TruthTrackGetterLabel()
JetTagD3PDFlags.MSVVtxInfoAssocLabel = JetTagD3PDKeys.MSVVtxInfoInJetGetterLabel()

### configure jet tag flags
JetTagD3PDFlags.SkimD3PD = True
JetTagD3PDFlags.FilterMinNJets = 1
JetTagD3PDFlags.Taggers = JetTagD3PDFlags.AllTaggers()
JetTagD3PDFlags.JetFitterFlipTwoTrackVertexAssoc = True
JetTagD3PDFlags.JetFitterFlipVxOnJetAxisAssoc = True
JetTagD3PDFlags.JetFitterVertexFinderInfo = True
JetTagD3PDFlags.JetVKalVxBadTrack = True
JetTagD3PDFlags.InfoBaseCalib = True
JetTagD3PDFlags.JetFitterTagFlipInfo = True
JetTagD3PDFlags.AddPixCond = True

JetTagD3PDFlags.JetElectronAssoc = True
JetTagD3PDFlags.JetPhotonAssoc = True
JetTagD3PDFlags.SoftElectronInfo = True
JetTagD3PDFlags.SoftMuonInfo = True
JetTagD3PDFlags.AddElectronInJetsObject = True
JetTagD3PDFlags.AddPhotonInJetsObject = True

JetTagD3PDFlags.NewGbbNNInfo=True
JetTagD3PDFlags.QGInfo=True
JetTagD3PDFlags.JetFitterCharmTagInfo=True
