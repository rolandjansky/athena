# JobOptions to extrapolate a track to Calorimeters

# ------ Specific to this alg :

from TBRec.TBRecConf import TBTrackToCaloAlg
MuToCaloAlg = TBTrackToCaloAlg( "MuToCaloAlg" )

MuToCaloAlg.ClusterContainerName = "CaloTopoClusterEM"
MuToCaloAlg.CaloCellContainerName = "AllCalo"
MuToCaloAlg.ImpactInCaloContainerName = "MuImpactInCalo"

# If type is anything else than TrackParticleCandidates, will take Tracks
# MuToCaloAlg.TrackInputType = "TrackParticleCandidates"
# MuToCaloAlg.TrackParticleName = "TrackParticleCandidate"
MuToCaloAlg.TrackInputType = "Tracks"
MuToCaloAlg.TrackName = "ConvertedMBoyTracks"

theApp += MuToCaloAlg

# ------ More general part to be used as an example : setup Extrapolator instance

TTC_ExtrapolatorInstance = 'MuToCaloExtrapolator'
include( "TBRec/ExtrapolToCaloTool_joboptions.py" )

# configure private ExtrapolTrackToCaloTool tool
MuToCaloAlg.ExtrapolTrackToCaloTool.ExtrapolatorName = TTC_Extrapolator.name()
MuToCaloAlg.ExtrapolTrackToCaloTool.ExtrapolatorInstanceName = TTC_Extrapolator.instance()
MuToCaloAlg.ExtrapolTrackToCaloTool.CaloDepthTool.DepthChoice= "entrance"

# the type of extrapolation is set automatically. To force it add here :
#    doStraightToCalo=False    will use RungeKutta (default for Atlas)
#    doStraightToCalo=True     will use StraightLine (default for ctb)
