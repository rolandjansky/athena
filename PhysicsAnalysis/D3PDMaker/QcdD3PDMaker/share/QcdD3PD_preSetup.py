# Backwards compatibility for taus.
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

#from TauTools.TauToolsConf import TauCommonDetailsMerger
#topSequence += TauCommonDetailsMerger("TauCommonDetailsMerger")

from D3PDMakerConfig.D3PDMakerFlags              import D3PDMakerFlags
from TrackD3PDMaker.TrackD3PDMakerFlags          import TrackD3PDFlags
from MissingETD3PDMaker.MissingETD3PDMakerFlags  import *

#for truth info
D3PDMakerFlags.TruthWriteHadrons.set_Value_and_Lock(True)
D3PDMakerFlags.MakeEgammaUserData.set_Value_and_Lock(True)

#for track
TrackD3PDFlags.trackParametersAtGlobalPerigeeLevelOfDetails=1
TrackD3PDFlags.trackParametersAtPrimaryVertexLevelOfDetails=2
TrackD3PDFlags.trackParametersAtBeamSpotLevelOfDetails=1
TrackD3PDFlags.trackParametersAtBeamLineLevelOfDetails=1
TrackD3PDFlags.storeTrackMomentum=True
TrackD3PDFlags.storeTrackSummary=True
#TrackD3PDFlags.doTruth=False
TrackD3PDFlags.doTruth=True
TrackD3PDFlags.storeTrackInfo=False
TrackD3PDFlags.storeTrackPredictionAtBLayer=False
TrackD3PDFlags.storeTrackParametersAtCalo=True
TrackD3PDFlags.storeTrackParametersAtCalo2ndLayer=True
TrackD3PDFlags.storeHitTruthMatching = True

#for vertex
TrackD3PDFlags.vertexPositionLevelOfDetails=3
TrackD3PDFlags.storeVertexKinematics=True
TrackD3PDFlags.storeVertexTrackAssociation=False

#for MET
MissingETD3PDMakerFlags.doMissingETRegions=True
MissingETD3PDMakerFlags.doCellOutEflow=True
MissingETD3PDMakerFlags.doCellOutMiniJet=True

# Preprocessing
from QcdD3PDMaker.QcdD3PDMakerConf import QcdD3PD__QcdD3PDPreProcessing
QcdD3PDPreProcessingAlg = QcdD3PD__QcdD3PDPreProcessing( name = "QcdD3PDPreProcessingAlg",
                                                                     OutputLevel = INFO )
topSequence += QcdD3PDPreProcessingAlg
