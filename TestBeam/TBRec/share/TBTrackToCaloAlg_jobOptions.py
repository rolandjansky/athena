# JobOptions to extrapolate a track to Calorimeters

# ------ Specific to this alg :

from TBRec.TBRecCong import TBTrackToCaloAlg
TrackToCaloAlg = TBTrackToCaloAlg("TrackToCaloAlg")
TrackToCaloAlg.ClusterContainerName = "CaloTopoClusterEM"
TrackToCaloAlg.CaloCellContainerName = "AllCalo"
TrackToCaloAlg.ImpactInCaloContainerName = "ImpactInCalo"

# If type is anything else than TrackParticleCandidates, will take Tracks
#TrackToCaloAlg.TrackInputType = "Tracks"
TrackToCaloAlg.TrackInputType = "TrackParticleCandidates"
TrackToCaloAlg.TrackName = "Tracks"
TrackToCaloAlg.TrackParticleName = "TrackParticleCandidate"

theApp += TrackToCaloAlg
# ------ More general part to be used as an example : setup Extrapolator instance

TTC_ExtrapolatorInstance = 'TrackToCaloExtrapolator'
include( "TBRec/TBExtrapolToCaloTool_joboptions.py" )

# configure private ExtrapolTrackToCaloTool tool
TrackToCaloAlg.ExtrapolTrackToCaloTool.ExtrapolatorName = TTC_Extrapolator.name()
TrackToCaloAlg.ExtrapolTrackToCaloTool.ExtrapolatorInstanceName = TTC_Extrapolator.instance()
TrackToCaloAlg.ExtrapolTrackToCaloTool.CaloDepthTool.DepthChoice= "entrance"

# the type of extrapolation is set automatically. To force it add here :
#    doStraightToCalo=False    will use RungeKutta (default for Atlas)
#    doStraightToCalo=True     will use StraightLine (default for ctb)



